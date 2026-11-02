#pragma once
#include "timezone.h"
#include "resource_file.h"
#include "currency_rates.h"
inline bool pico_timezone_save(const pico_timezone &zone)
{
    if(!zone.valid())return false;uint8_t data[20]{};memcpy(data,"PCTZ",4);data[4]=1;
    for(int i=0;i<4;++i){data[8+i]=uint32_t(zone.minutes)>>(8*i);data[12+i]=zone.rule>>(8*i);}
    uint32_t hash=pico_graph_scene::checksum(data,16);for(int i=0;i<4;++i)data[16+i]=hash>>(8*i);return pico_resource_save("network/TIMEZONE.cfg",data,sizeof data);
}
inline bool pico_timezone_load(pico_timezone &zone)
{
    uint8_t data[20];bool backup=false;return pico_resource_load("network/TIMEZONE.cfg",data,sizeof data,backup,[&](size_t n)
    {
        if(n!=20||memcmp(data,"PCTZ",4)||data[4]!=1||data[5]||data[6]||data[7])return false;uint32_t offset=0,rule=0,hash=0;
        for(int i=0;i<4;++i){offset|=uint32_t(data[8+i])<<(8*i);rule|=uint32_t(data[12+i])<<(8*i);hash|=uint32_t(data[16+i])<<(8*i);}
        pico_timezone candidate;candidate.minutes=int32_t(offset);candidate.rule=rule;if(hash!=pico_graph_scene::checksum(data,16)||!candidate.valid())return false;zone=candidate;return true;
    });
}
