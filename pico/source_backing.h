#pragma once
#include "bulk_storage.h"
struct pico_source_backing
{
    pico_source_backing()=default;
    pico_source_backing(const pico_source_backing &)=delete;
    uint32_t handle=0,checksum=0;size_t length=0;
    bool store(const char *text)
    {
        size_t n=strlen(text)+1;if(n>16384)return false;
        if(!handle)handle=pico_bulk_allocate(16384);if(!handle)return false;
        if(!pico_psram_write(handle,text,n))return false;
        length=n;checksum=pico_bulk_checksum(text,n);return true;
    }
    bool restore(char *text){return handle&&length&&pico_psram_read(handle,text,length)&&pico_bulk_checksum(text,length)==checksum&&text[length-1]==0;}
    ~pico_source_backing(){if(handle)pico_bulk_free(handle);}
};
