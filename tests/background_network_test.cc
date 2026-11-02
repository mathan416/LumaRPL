#include <cassert>
#include <algorithm>
#include "network.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <new>
#include "background_time.h"
#include "ntp_packet.h"
static bool fail_client_allocation=false;
void *operator new(size_t n,const std::nothrow_t &) noexcept
{if(fail_client_allocation){fail_client_allocation=false;return nullptr;}try{return ::operator new(n);}catch(...){return nullptr;}}
using err_t=int;
constexpr int ERR_OK=0,ERR_INPROGRESS=-5,ERR_MEM=-1;
constexpr int CYW43_LINK_NOIP=2,CYW43_LINK_UP=3,CYW43_LINK_NONET=-2,CYW43_LINK_FAIL=-1,CYW43_LINK_BADAUTH=-3,CYW43_ITF_STA=0;
constexpr unsigned CYW43_AUTH_WPA2_AES_PSK=7,CYW43_AUTH_OPEN=0;
constexpr int IPADDR_TYPE_V4=0,PBUF_TRANSPORT=0,PBUF_RAM=0;
struct ip_addr_t{unsigned value=0;};
struct udp_pcb;
struct pbuf{uint16_t tot_len=48;uint8_t data[512]{};};
using udp_callback=void(*)(void *,udp_pcb *,pbuf *,const ip_addr_t *,uint16_t);
struct udp_pcb{udp_callback callback=nullptr;void *context=nullptr;};
using dns_callback=void(*)(const char *,const ip_addr_t *,void *);
struct lookup{dns_callback callback;void *token;};
static lookup lookups[100];static unsigned lookup_count,joined,inits,deinits,sockets,packets;
static uint32_t now;static int cyw43_state,link_status,init_result,join_result,dns_result=ERR_INPROGRESS,send_result;
static bool socket_fail,packet_fail;
static unsigned auth;
uint32_t get_absolute_time(){return now;}uint32_t to_ms_since_boot(uint32_t n){return n;}
uint64_t get_rand_64(){return 0x123456789abcdef1ULL;}
int cyw43_arch_init(){++inits;return init_result;}void cyw43_arch_deinit(){++deinits;}
void cyw43_arch_enable_sta_mode(){}void cyw43_arch_disable_sta_mode(){}void cyw43_arch_poll(){}
int cyw43_arch_wifi_connect_async(const char *,const char *,unsigned mode){++joined;auth=mode;return join_result;}
int cyw43_tcpip_link_status(int *,int){return link_status;}
err_t dns_gethostbyname(const char *,ip_addr_t *out,dns_callback callback,void *token)
{assert(lookup_count<100);lookups[lookup_count++]={callback,token};out->value=42;return dns_result;}
bool ip_addr_cmp(const ip_addr_t *a,const ip_addr_t *b){return a->value==b->value;}
udp_pcb *udp_new_ip_type(int){if(socket_fail)return nullptr;++sockets;return new udp_pcb;}
void udp_recv(udp_pcb *p,udp_callback callback,void *context){p->callback=callback;p->context=context;}
void udp_remove(udp_pcb *p){--sockets;delete p;}err_t udp_connect(udp_pcb *,const ip_addr_t *,int){return ERR_OK;}
pbuf *pbuf_alloc(int,size_t,int){if(packet_fail)return nullptr;++packets;return new pbuf;}
err_t pbuf_take(pbuf *p,const void *data,size_t length){memcpy(p->data,data,length);return ERR_OK;}
void pbuf_free(pbuf *p){--packets;delete p;}
void pbuf_copy_partial(pbuf *p,void *out,size_t length,size_t at){memcpy(out,p->data+at,length);}
err_t udp_send(udp_pcb *,pbuf *){return send_result;}
uint32_t network_heap_used(){return 100;}
#include "../pico/radio_owner.inc"
#include "../pico/background_network.inc"
static void reset()
{
    stop_boot_time();assert(!sockets&&!packets);radio=radio_owner{};now=100;joined=inits=deinits=lookup_count=0;
    link_status=init_result=join_result=send_result=0;dns_result=ERR_INPROGRESS;socket_fail=packet_fail=false;
}
static void tick(uint32_t ms=5){now+=ms;poll_boot_time();}
static void start(){unsigned previous=joined;start_boot_time("Test network","test-password");assert(boot_time&&joined==previous+1&&auth==CYW43_AUTH_WPA2_AES_PSK);}
static void dns_ok(unsigned at){ip_addr_t ip{42};lookups[at].callback("time",&ip,lookups[at].token);}
static void waiting_reply()
{
    link_status=CYW43_LINK_UP;tick();dns_ok(lookup_count-1);tick();
    assert(boot_time->flow.stage==pico_background_time::phase::reply&&sockets==1);
    assert(*boot_time->ssid&&*boot_time->password);
}
static void deliver(bool valid=true,unsigned source=42,uint16_t port=123)
{
    auto *p=pbuf_alloc(0,48,0);p->data[0]=0x24;p->data[1]=2;
    memcpy(p->data+24,boot_time->ntp_request+40,8);if(!valid)p->data[24]^=1;
    pico_ntp_packet::word(p->data+32,3976214400U);pico_ntp_packet::word(p->data+40,3976214400U);
    ip_addr_t ip{source};auto *u=boot_time->udp;u->callback(u->context,u,p,&ip,port);
}
static void session_failure()
{
    link_status=CYW43_LINK_BADAUTH;
    for(unsigned i=0;i<400&&boot_time&&!boot_time->waiting;++i)tick(100);
    assert(boot_time&&boot_time->waiting&&!radio.station&&radio.initialized);
}
int main()
{
    reset();start_boot_time("","");start_boot_time(nullptr,"");assert(!boot_time&&!inits);
    fail_client_allocation=true;start_boot_time("Test","password");assert(!boot_time&&!inits);
    init_result=1;start_boot_time("Test","password");assert(!boot_time&&inits==1&&!deinits);
    reset();start_boot_time("Open","");assert(auth==CYW43_AUTH_OPEN);stop_boot_time();assert(deinits==1);
    reset();start();for(unsigned i=0;i<500;++i)tick();waiting_reply();
    deliver(false);tick();assert(boot_time);deliver(true,43);tick();assert(boot_time);deliver(true,42,999);tick();assert(boot_time);
    deliver();now+=5;assert(poll_boot_time()==1767225600LL);assert(!boot_time&&deinits==1&&!sockets&&!packets);assert(!poll_boot_time());
    // A transient handshake status may recover without another request.
    reset();start();link_status=CYW43_LINK_BADAUTH;tick(500);assert(joined==1);link_status=CYW43_LINK_UP;tick();assert(joined==1);
    reset();start();radio.event(0,46,4,15);radio.event(0,0,1,0);assert(radio.diagnostics.attempts[0].event==46&&radio.diagnostics.attempts[0].reason==15);session_failure();assert(joined==3&&inits==1&&!deinits);
    auto due=boot_time->due_at;now=due-5;poll_boot_time();assert(joined==3);tick();assert(joined==4&&inits==1);
    session_failure();due=boot_time->due_at;assert(due-now==120000);now=due;poll_boot_time();session_failure();
    due=boot_time->due_at;assert(due-now==300000);now=due;poll_boot_time();
    for(unsigned i=0;i<400&&boot_time;++i)tick(100);
    assert(!boot_time&&joined==12&&inits==1&&deinits==1);
    // Discovery and DHCP have independent grace/deadline behavior.
    reset();start();link_status=CYW43_LINK_NONET;tick();tick(4990);assert(joined==1&&radio.station);tick(10);assert(!radio.station);tick(1250);assert(joined==2);
    reset();start();link_status=CYW43_LINK_NOIP;for(unsigned i=0;i<350;++i)tick(100);assert(boot_time->waiting&&joined==1);
    reset();start();link_status=CYW43_LINK_UP;tick();tick(4000);assert(lookup_count==2);dns_ok(0);assert(!boot_time->dns_ready);dns_ok(1);tick();deliver();now+=5;assert(poll_boot_time()==1767225600LL&&!boot_time);
    // Manual takeover invalidates outstanding DNS; failure resumes its timer.
    reset();start();link_status=CYW43_LINK_UP;tick();auto pending=lookups[0];
    pause_boot_time("Test network","test-password");assert(boot_time->paused&&radio.foreground&&!radio.station);
    ip_addr_t ip{42};pending.callback("time",&ip,pending.token);assert(!boot_time->dns_ready);
    finish_foreground_time(false,false);assert(boot_time->waiting&&!radio.foreground);due=boot_time->due_at;
    now+=100;pause_boot_time("Test network","test-password");finish_foreground_time(false,false);assert(boot_time->due_at==due);
    pause_boot_time("Test network","test-password");finish_foreground_time(true,false);assert(!boot_time&&!radio.initialized);
    reset();start();pause_boot_time("Test network","test-password");finish_foreground_time(false,true);assert(!boot_time&&!radio.initialized);
    reset();start();pause_boot_time("Other","password");assert(!boot_time);finish_foreground_time(false,false);assert(!radio.initialized);
    // Only manual attempt three gets a full driver reset.
    reset();wifi_join_io io{"Test","password",true};pico_wifi_join join;join.start(now,io);link_status=CYW43_LINK_BADAUTH;
    for(unsigned i=0;i<400&&join.stage!=pico_wifi_join::phase::failed;++i){now+=100;join.tick(now,io);}
    assert(join.attempts==3&&join.stage==pico_wifi_join::phase::failed&&inits==2&&deinits==1);
    assert(radio.diagnostics.count==3);radio.event(0,46,4,15);radio.shutdown();
    for(unsigned fault=0;fault<3;++fault)
    {
        reset();start();dns_result=ERR_OK;socket_fail=fault==0;packet_fail=fault==1;send_result=fault==2?ERR_MEM:ERR_OK;
        link_status=CYW43_LINK_UP;for(unsigned i=0;i<5;++i)tick();assert(boot_time->waiting&&!sockets&&!packets&&!deinits);
    }
    reset();now=UINT32_MAX-100;start();session_failure();assert(inits==1);stop_boot_time();
    reset();start();now+=600000;poll_boot_time();assert(!boot_time&&!radio.initialized);
    reset();start();link_status=CYW43_LINK_UP;tick();pending=lookups[0];stop_boot_time();start();pending.callback("time",&ip,pending.token);assert(!boot_time->dns_ready);stop_boot_time();
    reset();start();now+=599000;boot_time->sessions=1;assert(!boot_time->schedule(now));stop_boot_time();
    reset();start();tick(55000);assert(boot_time->waiting&&!radio.station&&inits==1);stop_boot_time();
    assert(!sockets&&!packets);
    printf("PASS: production Wi-Fi join/radio/NTP adapters: transient auth, three attempts, discovery/DHCP deadlines, four delayed sessions, manual reset/takeover/resume/cancel, stale DNS, NTP validation, allocation failures, timer wrap and cleanup; host state bound %zu bytes (SDK I/O mocked)\n",sizeof(boot_time_client)+sizeof(radio_owner)+sizeof(wifi_join_io)+sizeof(pico_wifi_join)+2*sizeof(uintptr_t));
}
