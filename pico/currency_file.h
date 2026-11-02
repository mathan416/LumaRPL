#pragma once
#include "currency_rates.h"
#include "resource_file.h"
inline bool pico_currency_save(const char *path,const pico_currency_rates &rates)
{
    uint8_t data[pico_currency_rates::bytes];size_t n=rates.encode(data,sizeof data);return n&&pico_resource_save(path,data,n);
}
inline bool pico_currency_load(const char *path,pico_currency_rates &rates,bool &backup)
{
    uint8_t data[pico_currency_rates::bytes];return pico_resource_load(path,data,sizeof data,backup,[&](size_t n){return rates.decode(data,n);});
}
