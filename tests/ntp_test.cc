#include "ntp_packet.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>
static void response(uint8_t *packet,const uint8_t *request,int64_t utc)
{
    memset(packet,0,48);packet[0]=0x24;packet[1]=2;memcpy(packet+24,request+40,8);pico_ntp_packet::word(packet+32,uint32_t(utc+2208988800LL));pico_ntp_packet::word(packet+40,uint32_t(utc+2208988800LL));pico_ntp_packet::word(packet+36,4096);pico_ntp_packet::word(packet+44,4096);
}
int main()
{
    uint8_t request[48],packet[48];pico_ntp_packet::request(request,0xa8123456789abcdeULL);assert(request[0]==0x23);int64_t out=0;
    for(int64_t utc:{1793448000LL,2085978495LL,2085978496LL,2114380800LL}){response(packet,request,utc);assert(pico_ntp_packet::reply(packet,48,request,10,out)&&out==utc);}
    response(packet,request,1793448000LL);uint8_t valid[48];memcpy(valid,packet,48);
    for(int byte:{24,25,26,27,28,29,30,31}){memcpy(packet,valid,48);packet[byte]^=1;assert(!pico_ntp_packet::reply(packet,48,request,10,out));}
    memcpy(packet,valid,48);packet[0]|=0xc0;assert(!pico_ntp_packet::reply(packet,48,request,10,out));memcpy(packet,valid,48);packet[1]=0;assert(!pico_ntp_packet::reply(packet,48,request,10,out));packet[1]=16;assert(!pico_ntp_packet::reply(packet,48,request,10,out));
    memcpy(packet,valid,48);packet[0]=0x23;assert(!pico_ntp_packet::reply(packet,48,request,10,out));assert(!pico_ntp_packet::reply(valid,47,request,10,out));assert(!pico_ntp_packet::reply(valid,48,request,4001,out));
    response(packet,request,1577836799LL);assert(!pico_ntp_packet::reply(packet,48,request,10,out));response(packet,request,2145916800LL);assert(!pico_ntp_packet::reply(packet,48,request,10,out));
    memcpy(packet,valid,48);memset(packet+40,0,8);assert(!pico_ntp_packet::reply(packet,48,request,10,out));
    memcpy(packet,valid,48);pico_ntp_packet::word(packet+32,pico_ntp_packet::word(packet+40)+1);assert(!pico_ntp_packet::reply(packet,48,request,10,out));
    puts("PASS: SNTP request/reply cookie, mode/version/leap/stratum, bounds, malformed packets and 2036 era rollover");
}
