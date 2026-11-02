#include <initializer_list>

#include "psram.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdint>
static uint8_t memory[8*1024*1024];
static bool present=true,selected=false,mosi=false,clocked=false;static unsigned byte=0,bit=0,command=0,address=0,input=0,reply=0;static unsigned capacity=sizeof memory,transactions=0,wrap=1024;
extern "C" {
void gpio_init(unsigned){}void gpio_set_dir(unsigned,int){}void gpio_pull_down(unsigned){}void busy_wait_at_least_cycles(unsigned){}void sleep_us(unsigned){}
void gpio_put(unsigned pin,int value)
{
    if(pin==2)mosi=value;
    if(pin==20){selected=!value;if(selected){byte=bit=command=address=input=0;++transactions;}}
    if(pin!=21)return;bool rising=value&&!clocked;clocked=value;if(!rising||!selected)return;
    unsigned output=0;
    if(present&&command==0x9f&&byte>=4)output=(byte==4?0x0d:byte==5?0x5d:0x26);
    if(present&&command==0x0b&&byte>=5)output=memory[address%capacity];
    reply=(output>>(7-bit))&1;input=(input<<1)|mosi;
    if(++bit==8)
    {
        if(byte==0)command=input;
        else if(byte<=3)address=(address<<8)|input;
        else if(command==0x02){if(present)memory[address%capacity]=input;address=(address&~(wrap-1))|((address+1)&(wrap-1));}
        if(command==0x0b&&byte>=5)address=(address&~(wrap-1))|((address+1)&(wrap-1));
        input=bit=0;++byte;
    }
}
int gpio_get(unsigned){return reply;}
int pico_psram_bus_init(){return 1;}
int pico_psram_bus_transfer(const uint8_t *tx,uint8_t *rx,size_t n)
{
    assert(n>=1&&n<=13);gpio_put(20,0);
    for(size_t i=0;i<n;++i){unsigned value=0;for(unsigned b=0;b<8;++b){gpio_put(2,(tx[i]>>(7-b))&1);gpio_put(21,1);value=(value<<1)|gpio_get(3);gpio_put(21,0);}rx[i]=value;}
    gpio_put(20,1);return 1;
}
}
int main()
{
    present=false;assert(!pico_psram_init()&&!pico_psram_available()&&!pico_psram_capacity());
    uint8_t a[900],b[900];for(unsigned i=0;i<sizeof a;++i)a[i]=i*71+3;
    assert(!pico_psram_read(0,b,1)&&!pico_psram_write(0,a,1));
    present=true;for(unsigned size:{1024u*1024u,2u*1024u*1024u,4u*1024u*1024u}){capacity=size;assert(!pico_psram_init());}
    capacity=sizeof memory;assert(pico_psram_init()&&pico_psram_capacity()==sizeof memory);
    for(unsigned boundary:{32u,1024u}){wrap=boundary;
    for(unsigned offset: {0u,991u,1023u,65519u,unsigned(sizeof memory-sizeof a)}){assert(pico_psram_write(offset,a,sizeof a));memset(b,0,sizeof b);assert(pico_psram_read(offset,b,sizeof b));assert(!memcmp(a,b,sizeof a));}
    }
    assert(!pico_psram_write(sizeof memory-1,a,2));assert(!pico_psram_read(UINT32_MAX,b,1));assert(!pico_psram_write(0,nullptr,1));assert(!pico_psram_read(0,nullptr,1));assert(pico_psram_read(sizeof memory,nullptr,0));
    printf("PASS: actual SPI driver, absent/aliased chips, 8MiB probe, burst boundaries, end-of-chip and bounds (%u transactions)\n",transactions);
}
