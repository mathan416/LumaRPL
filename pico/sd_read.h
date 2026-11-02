#pragma once
#include <stdint.h>
#include <stddef.h>
// CMD17 gives each sector its own token/data/CRC transaction. CMD18 requires
// a token/CRC per block and a stop sequence; never treat it as one long packet.
typedef int (*pico_sd_read_command)(void *,uint32_t);
typedef int (*pico_sd_read_packet)(void *,uint8_t *,unsigned);
typedef void (*pico_sd_read_finish)(void *);
static inline int pico_sd_read_sectors(uint8_t *buffer,uint32_t sector,unsigned count,int block_addressed,
                                      void *context,pico_sd_read_command command,pico_sd_read_packet packet,pico_sd_read_finish finish)
{
    if(!buffer||!count||!command||!packet||!finish)return 0;
    uint64_t last=(uint64_t)sector+count-1;
    if(last>UINT32_MAX||(!block_addressed&&last>UINT32_MAX/512u))return 0;
    uint32_t address=block_addressed?sector:sector*512u,step=block_addressed?1u:512u;
    for(unsigned n=0;n<count;++n)
    {
        int ok=command(context,address)&&packet(context,buffer,512);
        finish(context);
        if(!ok)return 0;
        address+=step;buffer+=512;
    }
    return 1;
}
