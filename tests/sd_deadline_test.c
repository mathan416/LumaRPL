#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
uint64_t fake_us;
static int mode,last_command,pending,stuck;static unsigned rate;
unsigned spi_init(void *p,unsigned baud){(void)p;rate=baud;return baud;}
int spi_write_blocking(void *p,const uint8_t *data,size_t n)
{(void)p;fake_us+=n*20;if(n==6&&(data[0]&0xc0)==0x40){last_command=data[0]&63;pending=1;}return n;}
int spi_read_blocking(void *p,uint8_t filler,uint8_t *data,size_t n)
{
    (void)p;(void)filler;fake_us+=n*20;memset(data,stuck?0:0xff,n);
    if(stuck)return n;
    if(n==4&&last_command==8){memset(data,0,n);data[2]=1;data[3]=0xaa;if(mode==1)stuck=1;return n;}
    if(n==4&&last_command==58){memset(data,0,n);data[0]=0x40;return n;}
    if(pending&&n==1){pending=0;data[0]=mode==3?0xff:last_command==41?(mode==2?0:1):last_command==58?0:1;}
    return n;
}
#include "../pico/sdmmc.c"
int main(void)
{
    for(int test=0;test<5;++test)
    {
        mode=test==4?1:test;stuck=pending=0;last_command=-1;fake_us=test==4?(uint64_t)UINT32_MAX*1000-200000:0;
        uint64_t start=fake_us;int ok=MMC_disk_initialize();uint64_t elapsed=fake_us-start;
        assert(elapsed<=1501000);assert(ok==(mode==2));assert(!initializing);
        if(ok)assert(rate==20000000);else assert(!MMC_disk_ready());
    }
    puts("PASS: actual SD driver idle/busy/missing/successful cards and wrapping 1.5s initialization deadline");
}
