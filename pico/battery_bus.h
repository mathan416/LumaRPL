#pragma once
#include <stdint.h>
/* Battery transactions use the UF2 Loader reference's 500 ms timeout and
 * 16 ms settle pause. Callbacks return SDK byte counts/error codes verbatim. */
typedef struct {
    int word,write_result,read_result;
} pico_battery_transaction;
typedef struct {
    void *context;
    int (*write)(void *,uint8_t command,uint32_t timeout_us);
    int (*read)(void *,uint8_t *reply,uint32_t timeout_us);
    void (*pause)(void *,unsigned milliseconds);
} pico_battery_bus;
static inline pico_battery_transaction pico_controller_read_bus(pico_battery_bus bus,uint8_t command)
{
    pico_battery_transaction result={-1,0,0};uint8_t reply[2]={0};
    result.write_result=bus.write(bus.context,command,500000);
    if(result.write_result!=1)return result;
    bus.pause(bus.context,16);
    result.read_result=bus.read(bus.context,reply,500000);
    if(result.read_result==2)result.word=reply[0]|((int)reply[1]<<8);
    return result;
}

static inline pico_battery_transaction pico_battery_read_bus(pico_battery_bus bus)
{return pico_controller_read_bus(bus,0x0b);}
/* Official REG_ID_VER reply is [0, BIOSVERSION], version encoded as BCD. */
static inline int pico_keyboard_bios(int word)
{
    if(word<0||(word&255)!=0)return -1;
    int version=(word>>8)&255;
    return version>=0x10&&version<=0x99&&(version&15)<=9?version:-1;
}
static inline int pico_battery_bios_unsupported(int version)
{return version>=0x10&&version<0x12;}
