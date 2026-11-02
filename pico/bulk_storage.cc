#include "bulk_storage.h"
// 127 owned 64KiB slots. The first 64KiB is reserved for chip diagnostics.
static uint32_t allocated[4];
uint32_t pico_bulk_allocate(size_t size)
{
    if(!pico_psram_available()||!size||size>65536)return 0;
    for(unsigned i=0;i<127;++i)if(!(allocated[i/32]&(1u<<(i%32)))){allocated[i/32]|=1u<<(i%32);return (i+1)*65536;}return 0;
}
void pico_bulk_free(uint32_t handle)
{
    if(handle&&!(handle%65536)&&handle<=127*65536){unsigned i=handle/65536-1;allocated[i/32]&=~(1u<<(i%32));}
}
size_t pico_bulk_used(){unsigned count=0;for(auto bits:allocated)for(;bits;bits&=bits-1)++count;return count*65536;}
