#include "network.h"
#include "tls_budget.h"
#include "currency_csv.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "ntp_packet.h"
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cassert>
#include <algorithm>
cyw43_t cyw43_state;static uint32_t ticks;static bool driver;static tcp_pcb *connection;static udp_pcb *datagram;static uint8_t request_ntp[48];static bool ntp_pending;static unsigned ntp_requests;static int mode,cancels;static unsigned joins;static uint32_t joining;static const char *stage;static void (*pending_dns)(const char *,const ip_addr_t *,void *);static void *pending_token;
uint64_t get_absolute_time(){return ticks;}
uint32_t to_ms_since_boot(uint64_t value){return value;}
void sleep_ms(unsigned){if(mode!=12&&!(mode==28&&joins<2)&&!(mode==29&&!connection)&&mode!=30&&mode!=2&&mode!=10&&mode!=13&&!((mode==16||mode==22||mode==23)&&datagram)&&mode!=17&&mode!=18&&!(mode>=24&&mode<=27))usleep(1000);}
int cyw43_arch_init(){if(mode==8)return -1;driver=true;if(pending_dns){auto callback=pending_dns;pending_dns=nullptr;callback("api.frankfurter.dev",nullptr,pending_token);}return 0;}
void cyw43_arch_deinit(){driver=false;assert(!connection&&!datagram);}
void cyw43_arch_enable_sta_mode(){}
int cyw43_arch_wifi_connect_async(const char *,const char *,uint32_t auth){assert(auth==CYW43_AUTH_WPA2_AES_PSK);if(!joins)joining=ticks;++joins;return 0;}
int cyw43_tcpip_link_status(cyw43_t *,int)
{
    if(mode==2)return CYW43_LINK_DOWN;
    if(mode==9)return joins<2?CYW43_LINK_NONET:CYW43_LINK_UP;
    if(mode==10)return CYW43_LINK_NONET;
    if(mode==11)return CYW43_LINK_BADAUTH;
    if(mode==12||mode==30)return CYW43_LINK_FAIL;
    if(mode==28)return joins<2?CYW43_LINK_FAIL:CYW43_LINK_UP;
    if(mode==29)return ticks-joining<6?CYW43_LINK_FAIL:CYW43_LINK_UP;
    if(mode==13)return CYW43_LINK_NOIP;
    if(mode==14){uint32_t age=ticks-joining;return age<2?CYW43_LINK_DOWN:age<4?CYW43_LINK_JOIN:age<6?CYW43_LINK_NOIP:CYW43_LINK_UP;}
    return CYW43_LINK_UP;
}
tcp_pcb *tcp_new_ip_type(int){assert(!connection);connection=static_cast<tcp_pcb *>(calloc(1,sizeof(tcp_pcb)));connection->fd=-1;return connection;}
void tcp_arg(tcp_pcb *p,void *v){p->context=v;}void tcp_recv(tcp_pcb *p,recv_fn f){p->receive=f;}void tcp_err(tcp_pcb *p,error_fn f){p->error=f;}
void tcp_abort(tcp_pcb *p){if(p->fd>=0)close(p->fd);connection=nullptr;free(p);}
void tcp_recved(tcp_pcb *,size_t){}size_t tcp_sndbuf(tcp_pcb *){return 512;}
err_t tcp_output(tcp_pcb *){return ERR_OK;}
err_t tcp_write(tcp_pcb *p,const void *data,size_t size,int){if(mode==7)return ERR_MEM;int n=send(p->fd,data,size,0);return n==int(size)?ERR_OK:-3;}
err_t tcp_connect(tcp_pcb *p,const ip_addr_t *ip,unsigned port,err_t (*ready)(void *,tcp_pcb *,err_t))
{
    p->fd=socket(AF_INET,SOCK_STREAM,0);sockaddr_in target{};target.sin_family=AF_INET;target.sin_port=htons(port);target.sin_addr.s_addr=ip->address;
    timeval timeout{15,0};setsockopt(p->fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof timeout);setsockopt(p->fd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof timeout);
    if(connect(p->fd,reinterpret_cast<sockaddr *>(&target),sizeof target))return -3;fcntl(p->fd,F_SETFL,O_NONBLOCK);return ready(p->context,p,ERR_OK);
}
err_t dns_gethostbyname(const char *host,ip_addr_t *out,void (*callback)(const char *,const ip_addr_t *,void *),void *token)
{
    if(strstr(host,"time.")){out->address=0x0100007f;return ERR_OK;}
    if(mode==5){pending_dns=callback;pending_token=token;return ERR_INPROGRESS;}
    if(mode==4)return -3;addrinfo hints{},*result=nullptr;hints.ai_family=AF_INET;hints.ai_socktype=SOCK_STREAM;if(getaddrinfo(host,nullptr,&hints,&result))return -3;out->address=reinterpret_cast<sockaddr_in *>(result->ai_addr)->sin_addr.s_addr;freeaddrinfo(result);return ERR_OK;
}
size_t pbuf_copy_partial(pbuf *p,void *out,size_t count,size_t offset){memcpy(out,p->data+offset,count);return count;}void pbuf_free(pbuf *p){if(p->owned){free(p->data);free(p);}}
uint64_t get_rand_64(){uint64_t value;arc4random_buf(&value,sizeof value);return value;}
udp_pcb *udp_new_ip_type(int){if(mode==26)return nullptr;assert(!datagram);datagram=static_cast<udp_pcb *>(calloc(1,sizeof(udp_pcb)));return datagram;}
void udp_recv(udp_pcb *p,udp_recv_fn f,void *context){p->receive=f;p->context=context;}
void udp_remove(udp_pcb *p){assert(p==datagram);free(p);datagram=nullptr;ntp_pending=false;}
err_t udp_connect(udp_pcb *p,const ip_addr_t *peer,uint16_t port){p->peer=*peer;p->port=port;return ERR_OK;}
pbuf *pbuf_alloc(int,uint16_t size,int){auto *p=static_cast<pbuf *>(calloc(1,sizeof(pbuf)));p->data=static_cast<unsigned char *>(malloc(size));p->tot_len=size;p->owned=true;return p;}
err_t pbuf_take(pbuf *p,const void *data,size_t size){assert(size<=p->tot_len);memcpy(p->data,data,size);return ERR_OK;}
err_t udp_send(udp_pcb *,pbuf *p){assert(p->tot_len==48);memcpy(request_ntp,p->data,48);++ntp_requests;ntp_pending=true;return mode==27?ERR_MEM:ERR_OK;}
void deliver_ntp()
{
    if(!datagram||!datagram->receive||!ntp_pending||mode==17||mode==19)return;ntp_pending=false;
    uint8_t data[48]{};data[0]=0x24;data[1]=2;memcpy(data+24,request_ntp+40,8);uint32_t seconds=uint32_t(int64_t(time(nullptr))+2208988800LL);pico_ntp_packet::word(data+32,seconds);pico_ntp_packet::word(data+40,seconds);
    if((mode==16&&ntp_requests==1)||mode==18)data[24]^=1;
    if(mode==24)data[1]=0;if(mode==25)data[0]|=0xc0;
    ip_addr_t peer=datagram->peer;if(mode==22&&ntp_requests==1)peer.address^=1;uint16_t port=mode==23&&ntp_requests==1?999:123;
    pbuf packet{48,data};datagram->receive(datagram->context,datagram,&packet,&peer,port);
}
void cyw43_arch_poll()
{
    ticks+=(mode==12||(mode==28&&joins<2)||mode==2||mode==10||mode==13||((mode==16||mode==22||mode==23)&&datagram)||mode==17||mode==18||(mode>=24&&mode<=27))?1000:1;if(mode==12||mode==28||mode==30){pico_network_wifi_event(0,7,1,17);pico_network_wifi_event(0,0,1,0);pico_network_wifi_event(1,3,1,99);}
    deliver_ntp();if(!connection||connection->fd<0||!connection->receive)return;
    if(mode==6){auto *failed=connection;auto callback=failed->error;void *context=failed->context;tcp_abort(failed);callback(context,-3);return;}
    unsigned char data[512];int n=recv(connection->fd,data,sizeof data,MSG_DONTWAIT);if(n<0)return;
    if(!n){connection->receive(connection->context,connection,nullptr,ERR_OK);return;}
    pbuf packet{uint16_t(n),data};assert(connection->receive(connection->context,connection,&packet,ERR_OK)==ERR_OK);
}
extern "C" int mbedtls_hardware_poll(void *,unsigned char *out,size_t length,size_t *actual){arc4random_buf(out,length);*actual=length;return 0;}
int cancelled(void *){return (mode==30&&ticks>=2)||((mode==3||mode==5||mode==7)&&++cancels>15)||(mode==19&&datagram);}
void progress(void *,const char *text){stage=text;auto status=pico_network_get_status();assert(bool(status.radio_on)==driver);assert(bool(status.connected)==(driver&&cyw43_tcpip_link_status(&cyw43_state,CYW43_ITF_STA)==CYW43_LINK_UP));}
int main(int argc,char **argv)
{
    if(argc<2)return 2;mode=atoi(argv[1]);int64_t epoch=time(nullptr);if(mode==1)epoch=1577836800;if((mode>=15&&mode<=19)||(mode>=22&&mode<=27))epoch=0;
    auto before=pico_network_get_status();assert(!before.radio_on&&!before.connected&&!driver);
    char body[2048];pico_network_result result;bool ok=pico_network_rates_dated("host test transport","test-only-password",epoch,mode==20?"2024-01-02":mode==21?"2199-01-01":nullptr,body,sizeof body,&result,cancelled,progress,nullptr);
    printf("Transport mode %d: success=%d code=%d TLS=%d peak=%zu session=%zu; %s; last=%s\n",mode,ok,result.code,result.tls_error,result.tls_peak,result.session_bytes,result.message,stage?stage:"");
    pico_network_wifi_event(0,7,1,99); // Late callbacks must not access a deleted session.
    auto after=pico_network_get_status();assert(!after.radio_on&&!after.connected);
    assert(!driver&&!connection&&!pico_tls_used());if(!mode||mode==9||mode==14||mode==15||mode==16||mode==20||mode==22||mode==23||mode==28||mode==29){pico_currency_rates rates;assert(ok&&pico_currency_csv(body,strlen(body),rates,mode==20?"2024-01-02":nullptr));}else assert(!ok);
    if(mode==21)assert(result.code==9&&strstr(result.message,"future"));
    if(mode==22||mode==23)assert(result.ntp_status==1&&ntp_requests==2);
    if(mode>=24&&mode<=27)assert(result.code==10&&result.ntp_status==-1&&!result.utc_time);
    if(mode==15||mode==16)assert(result.ntp_status==1&&result.utc_time>=time(nullptr)-5&&ntp_requests==(mode==16?2u:1u));
    if(mode==17||mode==18)assert(result.code==10&&result.ntp_status==-1&&!result.utc_time&&ntp_requests==2);
    if(mode==19)assert(result.code==1&&!datagram&&!result.utc_time);
    if(mode==20)assert(strstr(body,"2024-01-02")&&result.ntp_status==0);
    if(mode>=15)printf("Time diagnostic: ntp=%d utc=%lld requests=%u\n",result.ntp_status,(long long)result.utc_time,ntp_requests);
    if(mode==9)assert(result.join_attempts==2&&result.link_status==CYW43_LINK_UP);
    if(mode==10)assert(result.join_ms>=35000&&result.join_attempts>1&&strstr(result.message,"discovery"));
    if(mode==11)assert(result.join_ms<1000&&result.join_attempts==1&&strstr(result.message,"authentication"));
    if(mode==12)assert(strstr(result.message,"association")&&result.join_attempts==3&&result.join_ms>=3000&&result.join_ms<35000);
    if(mode==12||mode==28||mode==30)assert(result.radio_event_valid&&result.radio_event==7&&result.radio_status==1&&result.radio_reason==17);
    if(mode==28)assert(result.join_attempts==2);
    if(mode==29)assert(result.join_attempts==1);
    if(mode==30)assert(result.code==1&&result.join_attempts==1);
    if(mode==13)assert(result.join_ms>=35000&&strstr(result.message,"DHCP"));
    if(mode==14)assert(result.join_ms>=6&&result.join_attempts==1);
    printf("Join diagnostic: status=%d elapsed=%u requests=%u\n",result.link_status,unsigned(result.join_ms),result.join_attempts);
    if(mode==5){mode=0;cancels=0;bool recovered=pico_network_rates("host test transport","test-only-password",time(nullptr),body,sizeof body,&result,cancelled,progress,nullptr);if(!recovered)fprintf(stderr,"Recovery failed %d %s\n",result.code,result.message);assert(recovered&&!driver&&!connection&&!pico_tls_used());puts("PASS: stale DNS callback ignored after cancellation and reconnect");}
    return 0;
}
