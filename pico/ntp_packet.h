#pragma once
#include <stddef.h>
#include <stdint.h>
#include <cstring>
// Small SNTP client codec. NTP is not authenticated: the unpredictable originate
// cookie rejects unrelated replies, not an attacker who can observe the request.
struct pico_ntp_packet
{
    static constexpr size_t bytes=48;
    static constexpr int64_t minimum=1577836800LL,maximum=2145916799LL;
    static uint32_t word(const uint8_t *p){return uint32_t(p[0])<<24|uint32_t(p[1])<<16|uint32_t(p[2])<<8|p[3];}
    static void word(uint8_t *p,uint32_t value){for(int i=3;i>=0;--i){p[i]=uint8_t(value);value>>=8;}}
    static void request(uint8_t *out,uint64_t cookie)
    {memset(out,0,bytes);out[0]=0x23;word(out+40,uint32_t(cookie>>32));word(out+44,uint32_t(cookie));}
    static bool unix_seconds(uint32_t seconds,int64_t &unix_time)
    {
        // Resolve the 2036 NTP era rollover without trusting an unset RTC.
        int64_t value=int64_t(seconds)-2208988800LL;if(value<minimum)value+=(1LL<<32);
        if(value<minimum||value>maximum)return false;unix_time=value;return true;
    }
    static bool reply(const uint8_t *packet,size_t length,const uint8_t *sent,uint32_t roundtrip_ms,int64_t &utc)
    {
        if(length<bytes||length>512||roundtrip_ms>4000||(packet[0]>>6)==3||(packet[0]&7)!=4||((packet[0]>>3)&7)<3||((packet[0]>>3)&7)>4||!packet[1]||packet[1]>15||memcmp(packet+24,sent+40,8))return false;
        if(!(word(packet+32)|word(packet+36))||!(word(packet+40)|word(packet+44)))return false;
        int64_t receive,transmit;if(!unix_seconds(word(packet+32),receive)||!unix_seconds(word(packet+40),transmit)||receive>transmit||transmit-receive>4)return false;
        if(receive==transmit&&word(packet+36)>word(packet+44))return false;
        uint64_t fraction_ms=(uint64_t(word(packet+44))*1000)>>32;
        int64_t value=transmit+int64_t((fraction_ms+roundtrip_ms/2+500)/1000);
        if(value<minimum||value>maximum)return false;utc=value;return true;
    }
};
