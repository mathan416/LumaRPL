#pragma once
#include "bulk_storage.h"
struct pico_sample_cache
{
    uint32_t handle=0,checksum=0;bool valid=false;
    static constexpr size_t bytes=900*sizeof(double);
    bool store(const double *samples)
    {
        uint32_t next=pico_bulk_checksum(samples,bytes);
        if(!handle)handle=pico_bulk_allocate(bytes);if(!handle)return false;
        valid=false;if(!pico_psram_write(handle,samples,bytes))return false;
        checksum=next;valid=true;return true;
    }
    bool restore(double *samples){if(!valid||!handle||!pico_psram_read(handle,samples,bytes)||pico_bulk_checksum(samples,bytes)!=checksum){valid=false;return false;}return true;}
    ~pico_sample_cache(){if(handle)pico_bulk_free(handle);}
    pico_sample_cache()=default;
    pico_sample_cache(const pico_sample_cache &)=delete;
};
