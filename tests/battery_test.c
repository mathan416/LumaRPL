#include "battery.h"
#include "battery_bus.h"
#include <assert.h>
#include <stdio.h>
typedef struct {int write_result,read_result,phase;unsigned pauses;uint8_t low,high,command;} fake_bus;
static int write_command(void *context,uint8_t reg,uint32_t timeout)
{fake_bus *bus=context;assert(reg==(bus->command?bus->command:0x0b)&&timeout==500000&&bus->phase==0);bus->phase=1;return bus->write_result;}
static void pause_bus(void *context,unsigned ms)
{fake_bus *bus=context;assert(bus->phase==1&&ms==16);bus->phase=2;++bus->pauses;}
static int read_reply(void *context,uint8_t *reply,uint32_t timeout)
{fake_bus *bus=context;assert(bus->phase==2&&timeout==500000);bus->phase=3;reply[0]=bus->low;reply[1]=bus->high;return bus->read_result;}
int main(void)
{
    fake_bus fake={1,2,0,0,0x0b,0xb2};pico_battery_bus bus={&fake,write_command,read_reply,pause_bus};
    pico_battery_transaction transaction=pico_battery_read_bus(bus);
    assert(transaction.word==0xb20b&&transaction.write_result==1&&transaction.read_result==2&&fake.pauses==1);
    for(int code=-2;code<=0;++code)
    {
        fake=(fake_bus){code,2,0,0,0x0b,50};transaction=pico_battery_read_bus(bus);
        assert(transaction.word==-1&&transaction.write_result==code&&transaction.read_result==0&&fake.phase==1&&!fake.pauses);
        fake=(fake_bus){1,code,0,0,0x0b,50};transaction=pico_battery_read_bus(bus);
        assert(transaction.word==-1&&transaction.write_result==1&&transaction.read_result==code&&fake.phase==3);
    }
    fake=(fake_bus){1,1,0,0,0x0b,50};transaction=pico_battery_read_bus(bus);assert(transaction.word==-1&&transaction.read_result==1);
    fake=(fake_bus){1,2,0,0,0,0x16,1};transaction=pico_controller_read_bus(bus,1);
    assert(pico_keyboard_bios(transaction.word)==0x16&&!pico_battery_bios_unsupported(0x16));
    assert(pico_battery_bios_unsupported(pico_keyboard_bios(0x1100)));
    assert(!pico_battery_bios_unsupported(pico_keyboard_bios(0x1200)));
    assert(pico_keyboard_bios(0x1601)==-1&&pico_keyboard_bios(-1)==-1&&pico_keyboard_bios(0x1a00)==-1);
    assert(!pico_battery_bios_unsupported(-1));
    pico_battery_state s={0};assert(pico_battery_value(&s)==-1&&pico_battery_due(&s,0));
    pico_battery_update(&s,0,-1);assert(!pico_battery_due(&s,4999)&&pico_battery_due(&s,5000));
    pico_battery_update(&s,1000,0x8b);assert(!s.valid); /* wrong echoed register */
    pico_battery_update(&s,2000,0xff0b);assert(!s.valid); /* invalid percentage */
    pico_battery_update(&s,3000,0xb20b);assert(pico_battery_value(&s)==50&&s.charging);
    assert(!pico_battery_due(&s,12999)&&pico_battery_due(&s,13000));
    pico_battery_update(&s,13000,-1);assert(pico_battery_value(&s)==50&&s.charging);
    pico_battery_update(&s,32999,-1);assert(pico_battery_due(&s,33000));
    pico_battery_update(&s,33000,-1);assert(!s.valid&&!s.charging&&pico_battery_value(&s)==-1);
    pico_battery_update(&s,34000,0x640b);assert(pico_battery_value(&s)==100&&!s.charging);
    for(int i=0;i<=100;++i){pico_battery_update(&s,35000+i,(i<<8)|0x0b);assert(pico_battery_value(&s)==i);}
    pico_battery_update(&s,0xfffffff0u,0x320b);assert(!pico_battery_due(&s,0x10));assert(pico_battery_due(&s,0x3000));
    assert(pico_battery_icon_fill(101,20)==0&&pico_battery_icon_fill(50,20)==10&&pico_battery_icon_fill(100,20)==20);
    puts("PASS: reference battery transport sequencing, 500 ms timeouts, write/read errors and short reads; battery register validation, charging, startup retry, transient failures, expiry, recovery, clock wrap and truthful icon fill");
}
