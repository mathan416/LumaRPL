#pragma once
#include <stdint.h>
/* Controller register 0x0b: echoed address, then 0..100 plus charging bit 7.
 * A failed read is never a full battery. No PMU configuration is changed. */
typedef struct {
    int percent,charging,last_reply,write_result,read_result,bios_version;
    uint32_t attempted_at,valid_at;
    int attempted,valid;
} pico_battery_state;
static inline int pico_battery_due(const pico_battery_state *s,uint32_t now)
{
    return !s->attempted || (s->valid&&(uint32_t)(now-s->valid_at)>=30000u) || (uint32_t)(now-s->attempted_at)>=(s->valid?10000u:s->last_reply<0?5000u:1000u);
}
static inline int pico_battery_reply_valid(int word)
{return word>=0&&(word&255)==0x0b&&((word>>8)&127)<=100;}
static inline void pico_battery_update(pico_battery_state *s,uint32_t now,int word)
{
    s->attempted=1;s->attempted_at=now;s->last_reply=word;
    if(pico_battery_reply_valid(word))
    {
        s->percent=(word>>8)&127;s->charging=(word>>15)&1;
        s->valid_at=now;s->valid=1;
    }
    else if(s->valid&&(uint32_t)(now-s->valid_at)>=30000u)
    {s->valid=0;s->charging=0;}
}
static inline int pico_battery_value(const pico_battery_state *s)
{return s->valid?s->percent:-1;}
static inline unsigned pico_battery_icon_fill(unsigned percent,unsigned width)
{return percent<=100?(percent*width+50)/100:0;}

#ifdef __cplusplus
extern "C" {
#endif
void pico_battery_snapshot(pico_battery_state *out);
#ifdef __cplusplus
}
#endif
