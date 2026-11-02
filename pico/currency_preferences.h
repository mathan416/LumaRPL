// Preferred currency codes, independent of rate-table order. GPL-3.0.
#pragma once
#include "currency_rates.h"
#include "resource_file.h"
struct pico_currency_preferences
{
    char from[4]{},to[4]{};
    static constexpr size_t bytes=20;
    static constexpr const char *path="currency/PAIR.cfg";
    bool valid()const{return pico_currency_rates::code_valid(from)&&pico_currency_rates::code_valid(to);}
    bool encode(uint8_t (&data)[bytes])const
    {
        if(!valid())return false;memset(data,0,bytes);memcpy(data,"PCCP",4);data[4]=1;
        memcpy(data+8,from,4);memcpy(data+12,to,4);
        uint32_t hash=pico_graph_scene::checksum(data,16);for(int i=0;i<4;++i)data[16+i]=hash>>(i*8);return true;
    }
    bool decode(const uint8_t *data,size_t length)
    {
        if(length!=bytes||memcmp(data,"PCCP",4)||data[4]!=1||data[5]||data[6]||data[7])return false;
        uint32_t hash=0;for(int i=0;i<4;++i)hash|=uint32_t(data[16+i])<<(i*8);
        if(hash!=pico_graph_scene::checksum(data,16))return false;
        pico_currency_preferences next;memcpy(next.from,data+8,4);memcpy(next.to,data+12,4);
        if(!next.valid())return false;*this=next;return true;
    }
    bool save()const{uint8_t data[bytes];return encode(data)&&pico_resource_save(path,data,bytes);}
    bool load()
    {
        uint8_t data[bytes];bool backup=false;
        return pico_resource_load(path,data,bytes,backup,[&](size_t n){return decode(data,n);});
    }
};
