// On-demand Pico 2 W HTTPS client. GPL-3.0; SDK/lwIP/Mbed TLS retain their licenses.
#include "network.h"
#include "tls_budget.h"
#include "https_client.h"
#include "network/ca_root.h"
#include "pico/cyw43_arch.h"
#include "lwip/dns.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "pico/rand.h"
#include "ntp_packet.h"
#include "background_time.h"
#include "compute_gate.h"
#include "currency_rates.h"
#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/platform_time.h"
#include "mbedtls/version.h"
static_assert(MBEDTLS_VERSION_MAJOR==3&&MBEDTLS_VERSION_MINOR==6&&MBEDTLS_VERSION_PATCH>=7,"Use the pinned maintained TLS release");
static_assert(MBEDTLS_SSL_IN_CONTENT_LEN>=16384,"Preserve full inbound TLS records");
#ifndef MBEDTLS_HAVE_TIME_DATE
#error "HTTPS certificate date checks must be enabled"
#endif
#include <cstdio>
#include <cstring>
#include <new>
#include <algorithm>
#include <malloc.h>
namespace {
uint32_t network_heap_used(){return uint32_t(mallinfo().uordblks);}
#include "radio_owner.inc"
#include "background_network.inc"
constexpr char host[]="api.frankfurter.dev";
// Fixed quote list keeps both network responses and saved tables bounded.
constexpr char request[]="GET /v2/rates.csv?base=EUR&quotes=USD,CAD,GBP,JPY,CHF,AUD,NZD,CNY,INR,MXN,BRL,KRW,SEK,NOK,DKK,PLN,CZK,HUF,TRY,ZAR,SGD,HKD,ILS,AED,SAR,THB,IDR,PHP,MYR HTTP/1.1\r\nHost: api.frankfurter.dev\r\nAccept: text/csv\r\nAccept-Encoding: identity\r\nConnection: close\r\nUser-Agent: DB48X-PicoCalc-port17\r\n\r\n";
struct session
{
    udp_pcb *udp=nullptr;uint8_t ntp_request[48]{};bool ntp_ready=false,dns_failed=false;int ntp_status=0;uint32_t ntp_sent=0,utc_at=0;
    char rate_date[12]{};
    tcp_pcb *tcp=nullptr;bool connected=false,closed=false,dns_done=false;
    unsigned char ring[3072];size_t read=0,available=0;
    ip_addr_t address{};uintptr_t token=0;
    uint32_t started=0;int64_t utc=0;int link_status=-99;uint32_t join_ms=0;unsigned join_attempts=0;
    bool joining=false,radio_event_valid=false;uint32_t radio_event=0,radio_status=0,radio_reason=0;
    int failure=0;const char *message="Network error";
    pico_network_cancel_fn cancel; pico_network_progress_fn progress;void *context;
    mbedtls_ssl_context ssl;mbedtls_ssl_config config;mbedtls_x509_crt ca;mbedtls_ctr_drbg_context rng;mbedtls_entropy_context entropy;
    session(pico_network_cancel_fn c,pico_network_progress_fn p,void *v):cancel(c),progress(p),context(v)
    {mbedtls_ssl_init(&ssl);mbedtls_ssl_config_init(&config);mbedtls_x509_crt_init(&ca);mbedtls_ctr_drbg_init(&rng);mbedtls_entropy_init(&entropy);}
    void fail(int code,const char *text){if(!failure){failure=code;message=text;}}
    void stage(const char *text){if(progress)progress(context,text);}
    bool pump()
    {
        if(failure)return false;if(cancel&&cancel(context)){fail(1,"Cancelled; previous rates retained");return false;}
        if(uint32_t(to_ms_since_boot(get_absolute_time())-started)>=90000){fail(2,"Network timeout (90 s); previous rates retained");return false;}
        cyw43_arch_poll();sleep_ms(1);return !failure;
    }
    ~session()
    {
        if(udp){udp_recv(udp,nullptr,nullptr);udp_remove(udp);udp=nullptr;}
        if(tcp){tcp_arg(tcp,nullptr);tcp_recv(tcp,nullptr);tcp_err(tcp,nullptr);tcp_abort(tcp);tcp=nullptr;}
        mbedtls_ssl_free(&ssl);mbedtls_ssl_config_free(&config);mbedtls_x509_crt_free(&ca);mbedtls_ctr_drbg_free(&rng);mbedtls_entropy_free(&entropy);

    }
};
static_assert(sizeof(session)<=8192,"Update the Arm TLS coexistence test for the session size");
session *active=nullptr;uintptr_t next_token=0;
time_t connection_time(time_t *out)
{
    time_t value=active?time_t(active->utc+uint32_t(to_ms_since_boot(get_absolute_time())-active->utc_at)/1000):0;
    if(out)*out=value;return value;
}
void resolved(const char *,const ip_addr_t *address,void *token)
{
    // DNS can finish after cancellation on a later initialization. No saved
    // pointer to a freed session is ever dereferenced by this callback.
    if(!active||active->token!=reinterpret_cast<uintptr_t>(token))return;
    if(!address)active->dns_failed=true;else{active->address=*address;active->dns_done=true;}
}
bool resolve(session &s,const char *name,uint32_t timeout)
{
    s.dns_done=s.dns_failed=false;s.token=++next_token;if(!s.token)s.token=++next_token;
    err_t code=dns_gethostbyname(name,&s.address,resolved,reinterpret_cast<void *>(s.token));
    if(code==ERR_OK)s.dns_done=true;else if(code!=ERR_INPROGRESS)return false;
    uint32_t since=to_ms_since_boot(get_absolute_time());
    while(!s.dns_done&&!s.dns_failed&&uint32_t(to_ms_since_boot(get_absolute_time())-since)<timeout)if(!s.pump())return false;
    bool ok=s.dns_done&&!s.dns_failed;++s.token;return ok;
}
void ntp_reply(void *context,udp_pcb *,pbuf *packet,const ip_addr_t *address,uint16_t port)
{
    auto &s=*static_cast<session *>(context);
    if(packet&&port==123&&ip_addr_cmp(address,&s.address)&&packet->tot_len>=48&&packet->tot_len<=512&&!s.ntp_ready)
    {
        uint8_t data[48];pbuf_copy_partial(packet,data,48,0);int64_t utc;uint32_t now=to_ms_since_boot(get_absolute_time());
        if(pico_ntp_packet::reply(data,packet->tot_len,s.ntp_request,uint32_t(now-s.ntp_sent),utc))
        {s.utc=utc;s.utc_at=now;s.ntp_ready=true;s.ntp_status=1;}
    }
    if(packet)pbuf_free(packet);
}
bool sync_time(session &s)
{
    const char *servers[]={"time.cloudflare.com","time.google.com"};
    for(const char *server:servers)
    {
        s.stage("Synchronizing network time (NTP)");if(!resolve(s,server,4000)){if(s.failure)return false;continue;}
        s.udp=udp_new_ip_type(IPADDR_TYPE_V4);if(!s.udp)continue;
        s.ntp_ready=false;pico_ntp_packet::request(s.ntp_request,get_rand_64()|1);
        udp_recv(s.udp,ntp_reply,&s);err_t code=udp_connect(s.udp,&s.address,123);pbuf *request=nullptr;
        if(code==ERR_OK){request=pbuf_alloc(PBUF_TRANSPORT,48,PBUF_RAM);if(!request)code=ERR_MEM;}
        if(request){code=pbuf_take(request,s.ntp_request,48);s.ntp_sent=to_ms_since_boot(get_absolute_time());if(code==ERR_OK)code=udp_send(s.udp,request);pbuf_free(request);}
        uint32_t since=to_ms_since_boot(get_absolute_time());
        while(code==ERR_OK&&!s.ntp_ready&&uint32_t(to_ms_since_boot(get_absolute_time())-since)<4000)if(!s.pump())break;
        udp_recv(s.udp,nullptr,nullptr);udp_remove(s.udp);s.udp=nullptr;
        if(s.failure)return false;if(s.ntp_ready)return true;
    }
    s.ntp_status=-1;s.fail(10,"NTP time unavailable; choose manual UTC in Update options");return false;
}
void tcp_error(void *context,err_t)
{auto &s=*static_cast<session *>(context);s.tcp=nullptr;s.fail(5,"TCP connection failed; previous rates retained");}
err_t tcp_connected(void *context,tcp_pcb *,err_t error)
{auto &s=*static_cast<session *>(context);if(error!=ERR_OK)s.fail(5,"TCP connection failed");else s.connected=true;return ERR_OK;}
err_t received(void *context,tcp_pcb *,pbuf *packet,err_t error)
{
    auto &s=*static_cast<session *>(context);if(!packet){s.closed=true;return ERR_OK;}if(error!=ERR_OK){pbuf_free(packet);s.fail(5,"TCP receive failed");return ERR_OK;}
    if(packet->tot_len>sizeof s.ring-s.available)return ERR_MEM;
    size_t write=(s.read+s.available)%sizeof s.ring;size_t first=std::min<size_t>(packet->tot_len,sizeof s.ring-write);
    pbuf_copy_partial(packet,s.ring+write,first,0);if(first<packet->tot_len)pbuf_copy_partial(packet,s.ring,packet->tot_len-first,first);
    s.available+=packet->tot_len;pbuf_free(packet);return ERR_OK;
}
int tls_send(void *context,const unsigned char *data,size_t length)
{
    auto &s=*static_cast<session *>(context);
    while(s.tcp&&!s.closed&&!s.failure)
    {
        size_t count=std::min<size_t>(length,tcp_sndbuf(s.tcp));if(count){err_t code=tcp_write(s.tcp,data,count,TCP_WRITE_FLAG_COPY);if(code==ERR_OK){tcp_output(s.tcp);return int(count);}if(code!=ERR_MEM){s.fail(5,"TCP send failed");break;}}
        if(!s.pump())break;
    }
    return MBEDTLS_ERR_SSL_INTERNAL_ERROR;
}
int tls_receive(void *context,unsigned char *out,size_t length)
{
    auto &s=*static_cast<session *>(context);while(!s.available&&!s.closed&&!s.failure)if(!s.pump())break;
    if(s.failure)return MBEDTLS_ERR_SSL_INTERNAL_ERROR;if(!s.available)return 0;
    size_t count=std::min(length,s.available),first=std::min(count,sizeof s.ring-s.read);memcpy(out,s.ring+s.read,first);if(first<count)memcpy(out+first,s.ring,count-first);
    s.read=(s.read+count)%sizeof s.ring;s.available-=count;if(s.tcp)tcp_recved(s.tcp,count);return int(count);
}
bool download(session &s,const char *ssid,const char *password,char *out,size_t capacity,int &tls_error)
{
    s.stage("Connecting to Wi-Fi / DHCP");
    wifi_join_io io{ssid,password,true,s.progress,s.context};pico_wifi_join join;
    uint32_t began=to_ms_since_boot(get_absolute_time());join.start(began,io);
    while(join.stage!=pico_wifi_join::phase::connected&&join.stage!=pico_wifi_join::phase::failed)
    {
        if(!s.pump())return false;
        join.tick(to_ms_since_boot(get_absolute_time()),io);
    }
    s.join_ms=uint32_t(to_ms_since_boot(get_absolute_time())-began);s.join_attempts=join.attempts;
    if(radio.diagnostics.count)
    {
        auto &last=radio.diagnostics.attempts[radio.diagnostics.count-1];s.link_status=last.link_status;
        s.radio_event_valid=last.event_valid;s.radio_event=last.event;s.radio_status=last.status;s.radio_reason=last.reason;
    }
    if(join.stage!=pico_wifi_join::phase::connected)
    {
        const char *message=join.reason==pico_wifi_join::failure::handshake?"WPA2 handshake failed after retries; see Details":
            join.reason==pico_wifi_join::failure::discovery?"Network not found within connection window":
            join.reason==pico_wifi_join::failure::dhcp?"Wi-Fi joined but DHCP did not supply an IP address":
            "Wi-Fi connection did not complete after retries";
        s.fail(3,message);return false;
    }
    s.joining=false;
    if(!s.utc&&!sync_time(s))return false;
    if(*s.rate_date)
    {
        time_t now=time_t(s.utc+uint32_t(to_ms_since_boot(get_absolute_time())-s.utc_at)/1000);tm calendar=*gmtime(&now);char today[12];snprintf(today,sizeof today,"%04d-%02d-%02d",calendar.tm_year+1900,calendar.tm_mon+1,calendar.tm_mday);
        if(strcmp(s.rate_date,today)>0){s.fail(9,"Rate date is in the future; old rates retained");return false;}
    }
    radio.stage(PICO_NET_HTTPS);s.stage("Resolving HTTPS provider");if(!resolve(s,host,8000)){if(!s.failure)s.fail(4,"DNS lookup failed; previous rates retained");return false;}
    s.tcp=tcp_new_ip_type(IPADDR_TYPE_V4);if(!s.tcp){s.fail(6,"Not enough network memory");return false;}tcp_arg(s.tcp,&s);tcp_err(s.tcp,tcp_error);tcp_recv(s.tcp,received);
    s.stage("Connecting to HTTPS server");if(tcp_connect(s.tcp,&s.address,443,tcp_connected)!=ERR_OK){s.fail(5,"TCP connection could not start");return false;}while(!s.connected)if(!s.pump())return false;
    char dated[416];const char *http_request=request;
    if(*s.rate_date)
    {
        const char *space=strstr(request," HTTP/");int length=snprintf(dated,sizeof dated,"%.*s&date=%s%s",int(space-request),request,s.rate_date,space);
        if(length<0||size_t(length)>=sizeof dated){s.fail(9,"Invalid rate request");return false;}http_request=dated;
    }
    return pico_https_get(s,host,http_request,out,capacity,tls_error,tls_send,tls_receive);
}
}
namespace { pico_compute_gate compute_gate; }
extern "C" int pico_compute_network_begin(){
    if(!compute_gate.begin(to_ms_since_boot(get_absolute_time()),active!=nullptr,network_heap_used()))return 0;
    if(boot_time){boot_time->close_request();boot_time->paused=true;}
    return 1;
}
extern "C" void pico_compute_network_end(){
    if(!compute_gate.active)return;uint32_t now=to_ms_since_boot(get_absolute_time());uint32_t elapsed=compute_gate.end(now);
    if(boot_time){boot_time->lifetime+=elapsed;boot_time->due_at+=elapsed;boot_time->paused=false;if(!boot_time->waiting)boot_time->flow.start(now,*boot_time);}
}
extern "C" void pico_network_wifi_event(unsigned interface,unsigned event,unsigned status,unsigned reason)
{
    radio.event(interface,event,status,reason);
}
extern "C" void pico_network_time_start(const char *ssid,const char *password)
{if(!active)start_boot_time(ssid,password);}
extern "C" int64_t pico_network_time_poll(void)
{return active||compute_gate.active?0:poll_boot_time();}
extern "C" void pico_network_time_cancel(void){stop_boot_time();}
extern "C" void pico_network_credentials_saved(const char *ssid,const char *password)
{if(boot_time&&(!ssid||!password||strcmp(ssid,boot_time->ssid)||strcmp(password,boot_time->password)))stop_boot_time();}
extern "C" void pico_network_get_diagnostics(pico_network_diagnostics *out)
{
    if(!out)return;*out=radio.diagnostics;out->clock_pending=boot_time!=nullptr;
    if(radio.live&&out->count)out->attempts[out->count-1].elapsed_ms=uint32_t(to_ms_since_boot(get_absolute_time())-out->attempts[out->count-1].started_ms);
    if(boot_time&&boot_time->waiting)
    {int32_t remaining=int32_t(boot_time->due_at-to_ms_since_boot(get_absolute_time()));out->next_retry_ms=remaining>0?uint32_t(remaining):0;}
    else out->next_retry_ms=0;
}
extern "C" const uint32_t pico_network_memory_profile[]={
    sizeof(boot_time_client)+sizeof(radio_owner)+sizeof(wifi_join_io)+sizeof(pico_wifi_join)+2*sizeof(uintptr_t)+sizeof(pico_compute_gate),sizeof(boot_time_client)};
extern "C" pico_network_status pico_network_get_status(void)
{
    pico_network_status status{};
    if(radio.initialized){status.radio_on=1;status.connected=radio.station&&cyw43_tcpip_link_status(&cyw43_state,CYW43_ITF_STA)==CYW43_LINK_UP;}
    return status;
}
extern "C" int pico_network_rates_dated(const char *ssid,const char *password,int64_t utc,const char *rate_date,char *body,size_t capacity,pico_network_result *result,pico_network_cancel_fn cancel,pico_network_progress_fn progress,void *context)
{
    if(compute_gate.active){if(result){*result={};result->code=10;snprintf(result->message,sizeof result->message,"Calculation in progress; retry after it finishes");}return 0;}
    *result={};if(!ssid||!*ssid||!password||!body||!capacity||(utc&&(utc<1577836800||utc>2145916799))||(rate_date&&*rate_date&&!pico_currency_rates::date_valid(rate_date))||active){result->code=9;snprintf(result->message,sizeof result->message,"Check Wi-Fi setup and UTC date/time (2020-2037)");return 0;}
    if(!pico_tls_budget_reset()){result->code=6;snprintf(result->message,sizeof result->message,"TLS memory still in use");return 0;}
    session *state=new(std::nothrow) session(cancel,progress,context);if(!state){result->code=6;snprintf(result->message,sizeof result->message,"Not enough free heap for networking");return 0;}
    pause_boot_time(ssid,password);active=state;state->utc=utc;if(rate_date)snprintf(state->rate_date,sizeof state->rate_date,"%s",rate_date);state->started=state->utc_at=to_ms_since_boot(get_absolute_time());state->token=++next_token;if(!state->token)state->token=++next_token;
    auto previous_time=mbedtls_time;mbedtls_platform_set_time(connection_time);
    bool ok=download(*state,ssid,password,body,capacity,result->tls_error);
    if(!ok&&!state->failure)state->fail(5,"HTTPS request did not complete; previous rates retained");
    if(radio.diagnostics.count)
    {
        const auto &last=radio.diagnostics.attempts[radio.diagnostics.count-1];state->link_status=last.link_status;
        if(!state->join_attempts)state->join_attempts=last.attempt;
        state->radio_event_valid=last.event_valid;state->radio_event=last.event;state->radio_status=last.status;state->radio_reason=last.reason;
    }
    result->radio_event_valid=state->radio_event_valid;result->radio_event=state->radio_event;result->radio_status=state->radio_status;result->radio_reason=state->radio_reason;
    result->code=state->failure;result->ntp_status=state->ntp_status;result->utc_time=state->utc?state->utc+uint32_t(to_ms_since_boot(get_absolute_time())-state->utc_at)/1000:0;result->link_status=state->link_status;result->join_ms=state->join_ms;result->join_attempts=state->join_attempts;result->tls_peak=pico_tls_peak();result->session_bytes=sizeof(session);
    snprintf(result->message,sizeof result->message,"%s",ok?"HTTPS download complete":state->message);
    radio.finish(ok?PICO_NET_SUCCESS:state->failure==1?PICO_NET_CANCELLED:PICO_NET_FAILED);
    delete state;active=nullptr;mbedtls_platform_set_time(previous_time);
    if(pico_tls_used()){ok=false;result->code=6;snprintf(result->message,sizeof result->message,"TLS cleanup failed; restart before reconnecting");}
    finish_foreground_time(result->utc_time!=0,result->code==1);
    return ok;
}

extern "C" mbedtls_ms_time_t mbedtls_ms_time(void){return to_ms_since_boot(get_absolute_time());}

extern "C" int pico_network_rates(const char *ssid,const char *password,int64_t utc,char *body,size_t capacity,pico_network_result *result,pico_network_cancel_fn cancel,pico_network_progress_fn progress,void *context)
{return pico_network_rates_dated(ssid,password,utc,nullptr,body,capacity,result,cancel,progress,context);}
