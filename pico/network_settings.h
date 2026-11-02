#pragma once
#include "resource_file.h"
#include "currency_rates.h"
struct pico_network_settings
{
    char ssid[33]{};char password[64]{};bool open=false;
    bool valid()const
    {
        size_t n=strnlen(ssid,sizeof ssid),p=strnlen(password,sizeof password);if(!n||n==sizeof ssid||p==sizeof password||(open?p!=0:p<8))return false;
        for(size_t i=0;i<n;++i)if(static_cast<unsigned char>(ssid[i])<32||ssid[i]==127)return false;
        for(size_t i=0;i<p;++i)if(password[i]<32||password[i]>126)return false;return true;
    }
    static constexpr size_t bytes=116;
    bool save()const
    {
        if(!valid())return false;uint8_t data[bytes]{};memcpy(data,"PCWIFI17",8);data[8]=1;data[9]=open;memcpy(data+12,ssid,33);memcpy(data+45,password,64);uint32_t hash=pico_graph_scene::checksum(data,112);for(int i=0;i<4;++i)data[112+i]=hash>>(i*8);return pico_resource_save("network/WIFI.cfg",data,sizeof data);
    }
    bool load()
    {
        uint8_t data[bytes];bool backup=false;return pico_resource_load("network/WIFI.cfg",data,sizeof data,backup,[&](size_t n)
        {
            if(n!=bytes||memcmp(data,"PCWIFI17",8)||data[8]!=1||data[9]>1||data[10]||data[11])return false;uint32_t hash=0;for(int i=0;i<4;++i)hash|=uint32_t(data[112+i])<<(i*8);if(hash!=pico_graph_scene::checksum(data,112))return false;
            pico_network_settings candidate;candidate.open=data[9]!=0;memcpy(candidate.ssid,data+12,33);memcpy(candidate.password,data+45,64);if(!candidate.valid())return false;*this=candidate;return true;
        });
    }
    void clear(){open=false;volatile char *s=ssid,*p=password;for(size_t i=0;i<sizeof ssid;++i)s[i]=0;for(size_t i=0;i<sizeof password;++i)p[i]=0;}
    ~pico_network_settings(){clear();}
};
