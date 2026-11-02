#include "sd_read.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
struct card {unsigned commands,packets,finished,fail_command,fail_packet;uint32_t address,step;};
static int command(void *opaque,uint32_t address)
{
    struct card *c=opaque;assert(address==c->address+c->commands*c->step);++c->commands;
    return c->commands!=c->fail_command;
}
static int packet(void *opaque,uint8_t *buffer,unsigned size)
{
    struct card *c=opaque;assert(size==512);++c->packets;
    if(c->packets==c->fail_packet)return 0;
    // A card emits a separate token and CRC for each packet. Only payload is copied.
    memset(buffer,(int)c->packets,512);return 1;
}
static void finish(void *opaque){++((struct card*)opaque)->finished;}
int main(void)
{
    uint8_t guarded[3*512+2];memset(guarded,0xa5,sizeof guarded);struct card c={0};c.address=42;c.step=1;
    assert(pico_sd_read_sectors(guarded+1,42,3,1,&c,command,packet,finish));
    assert(c.commands==3&&c.packets==3&&c.finished==3&&guarded[0]==0xa5&&guarded[sizeof guarded-1]==0xa5);
    for(unsigned sector=0;sector<3;++sector)for(unsigned i=0;i<512;++i)assert(guarded[1+sector*512+i]==sector+1);
    c=(struct card){0};c.address=42*512;c.step=512;
    assert(pico_sd_read_sectors(guarded+1,42,3,0,&c,command,packet,finish));
    c=(struct card){0};c.step=1;c.fail_packet=2;
    assert(!pico_sd_read_sectors(guarded+1,0,3,1,&c,command,packet,finish)&&c.finished==2&&c.commands==2);
    c=(struct card){0};c.step=1;c.fail_command=1;
    assert(!pico_sd_read_sectors(guarded+1,0,3,1,&c,command,packet,finish)&&c.finished==1&&!c.packets);
    c=(struct card){0};c.step=1;assert(pico_sd_read_sectors(guarded+1,0,1,1,&c,command,packet,finish));
    assert(!pico_sd_read_sectors(guarded+1,0,0,1,&c,command,packet,finish));
    assert(!pico_sd_read_sectors(guarded+1,UINT32_MAX,2,1,&c,command,packet,finish));
    assert(!pico_sd_read_sectors(guarded+1,UINT32_MAX/512,2,0,&c,command,packet,finish));
    puts("PASS: SD sector packets, payload boundaries, block/byte addressing, failures, deselection and recovery");
}
