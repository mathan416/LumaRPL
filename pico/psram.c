// Explicit serial storage: GP2/3 data, GP20 CS and GP21 SCK, GPL-3.0.
#include "psram.h"
#include "psram_bus.h"
#include "pico/stdlib.h"
#include <string.h>
#define PSRAM_BYTES (8u*1024u*1024u)
// Respect both 32-byte wrap and default 1KiB boundaries, including unaligned I/O.
static int ready;
static int bounds(uint32_t offset,size_t length){return offset<=PSRAM_BYTES&&length<=PSRAM_BYTES-offset;}
static void command(uint8_t *tx,uint8_t code,uint32_t offset)
{tx[0]=code;tx[1]=offset>>16;tx[2]=offset>>8;tx[3]=offset;}
int pico_psram_read(uint32_t offset,void *data,size_t length)
{
    if(!ready||!bounds(offset,length)||(!data&&length))return 0;uint8_t *out=data;
    while(length)
    {
        size_t n=length>8?8:length;if(n>32-(offset&31))n=32-(offset&31);
        uint8_t tx[13]={},rx[13];command(tx,0x0b,offset);
        if(!pico_psram_bus_transfer(tx,rx,n+5)){ready=0;return 0;}
        memcpy(out,rx+5,n);out+=n;offset+=n;length-=n;
    }
    return 1;
}
int pico_psram_write(uint32_t offset,const void *data,size_t length)
{
    if(!ready||!bounds(offset,length)||(!data&&length))return 0;const uint8_t *in=data;
    while(length)
    {
        size_t n=length>8?8:length;if(n>32-(offset&31))n=32-(offset&31);
        uint8_t tx[12],rx[12];command(tx,0x02,offset);memcpy(tx+4,in,n);
        if(!pico_psram_bus_transfer(tx,rx,n+4)){ready=0;return 0;}in+=n;offset+=n;length-=n;
    }
    return 1;
}
int pico_psram_init(void)
{
    ready=0;if(!pico_psram_bus_init())return 0;
    uint8_t tx[12]={0x66},rx[12];if(!pico_psram_bus_transfer(tx,rx,1))return 0;sleep_us(50);
    tx[0]=0x99;if(!pico_psram_bus_transfer(tx,rx,1))return 0;sleep_us(150);
    memset(tx,0,sizeof tx);tx[0]=0x9f;if(!pico_psram_bus_transfer(tx,rx,12))return 0;
    unsigned variation=0;for(unsigned i=4;i<12;++i)variation|=rx[i]^rx[4];if(!variation&&(rx[4]==0||rx[4]==255))return 0;
    ready=1;uint8_t expected[256],actual[256];const uint32_t addresses[]={0,4096,65536,131072,262144,524288,1048576,2097152,4194304,PSRAM_BYTES-256};
    for(unsigned bank=0;bank<sizeof addresses/sizeof *addresses;++bank){for(unsigned i=0;i<256;++i)expected[i]=(uint8_t)(i*73+bank*51+19);if(!pico_psram_write(addresses[bank],expected,256)){ready=0;return 0;}}
    for(unsigned bank=0;bank<sizeof addresses/sizeof *addresses;++bank){for(unsigned i=0;i<256;++i)expected[i]=(uint8_t)(i*73+bank*51+19);if(!pico_psram_read(addresses[bank],actual,256)||memcmp(expected,actual,256)){ready=0;return 0;}}
    return 1;
}
int pico_psram_available(void){return ready;}
uint32_t pico_psram_capacity(void){return ready?PSRAM_BYTES:0;}
