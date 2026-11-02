#pragma once
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include "graph_scene.h"

// Explicit wire layout: independent of host alignment, endianness and FP format.
struct pico_currency_rates
{
    static constexpr unsigned maximum=32;
    static constexpr size_t bytes=48+maximum*40+4;
    struct row { char code[4]{};char date[12]{};char rate[24]{}; };
    char provider[32]="Manual";
    unsigned count=0;
    row rows[maximum]{};
    static bool code_valid(const char *s)
    {return s[0]>='A'&&s[0]<='Z'&&s[1]>='A'&&s[1]<='Z'&&s[2]>='A'&&s[2]<='Z'&&!s[3];}
    static bool date_valid(const char *s)
    {
        if(strnlen(s,12)!=10||s[4]!='-'||s[7]!='-')return false;
        for(int i=0;i<10;++i)if(i!=4&&i!=7&&(s[i]<'0'||s[i]>'9'))return false;
        int y=atoi(s),m=atoi(s+5),d=atoi(s+8);if(y<1900||y>2199||m<1||m>12)return false;
        const int days[]={31,28,31,30,31,30,31,31,30,31,30,31};int limit=days[m-1]+(m==2&&y%4==0&&(y%100!=0||y%400==0));return d>=1&&d<=limit;
    }
    static bool rate_valid(const char *s)
    {
        size_t n=strnlen(s,24);if(!n||n>=24)return false;bool dot=false,digit=false;
        for(size_t i=0;i<n;++i){if(s[i]=='.'&&!dot)dot=true;else if(s[i]>='0'&&s[i]<='9')digit=true;else return false;}
        double value=strtod(s,nullptr);return digit&&std::isfinite(value)&&value>=1e-12&&value<=1e12;
    }
    unsigned index(const char *code,unsigned fallback=0)const
    {for(unsigned i=0;i<count;++i)if(!strcmp(rows[i].code,code))return i;return fallback<count?fallback:0;}
    bool valid()const
    {
        size_t n=strnlen(provider,sizeof provider);if(!n||n==sizeof provider||!count||count>maximum)return false;
        for(size_t i=0;i<n;++i)if(provider[i]<32||provider[i]>126)return false;
        for(unsigned i=0;i<count;++i){const row &r=rows[i];if(!code_valid(r.code)||!date_valid(r.date)||!rate_valid(r.rate))return false;
            if(!strcmp(r.code,"EUR")&&strtod(r.rate,nullptr)!=1)return false;
            for(unsigned j=0;j<i;++j)if(!strcmp(r.code,rows[j].code))return false;}
        return true;
    }
    size_t encode(uint8_t *out,size_t capacity)const
    {
        if(capacity<bytes||!valid())return 0;memset(out,0,bytes);memcpy(out,"PCFX17\0",7);out[8]=1;out[12]=count;memcpy(out+16,provider,32);
        for(unsigned i=0;i<count;++i){size_t at=48+i*40;memcpy(out+at,rows[i].code,4);memcpy(out+at+4,rows[i].date,12);memcpy(out+at+16,rows[i].rate,24);}
        uint32_t hash=pico_graph_scene::checksum(out,bytes-4);for(int i=0;i<4;++i)out[bytes-4+i]=uint8_t(hash>>(8*i));return bytes;
    }
    bool decode(const uint8_t *data,size_t length)
    {
        if(length!=bytes||memcmp(data,"PCFX17\0",8)||data[8]!=1||data[9]||data[10]||data[11]||data[13]||data[14]||data[15])return false;
        uint32_t hash=0;for(int i=0;i<4;++i)hash|=uint32_t(data[bytes-4+i])<<(8*i);if(hash!=pico_graph_scene::checksum(data,bytes-4))return false;
        pico_currency_rates candidate;candidate.count=data[12];memcpy(candidate.provider,data+16,32);if(candidate.count>maximum)return false;
        for(unsigned i=0;i<candidate.count;++i){size_t at=48+i*40;memcpy(candidate.rows[i].code,data+at,4);memcpy(candidate.rows[i].date,data+at+4,12);memcpy(candidate.rows[i].rate,data+at+16,24);}
        if(!candidate.valid())return false;*this=candidate;return true;
    }
};
