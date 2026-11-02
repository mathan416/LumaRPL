#include "wall_clock.h"
#include "network_time.h"
#include <cassert>
#include <cstdio>
int main()
{
    pico_wall_clock clock;assert(clock.zone.minutes==-300&&clock.zone.rule==pico_timezone::north_america&&!clock.synchronized);assert(clock.local(10)==1767225610LL);
    int64_t utc;assert(pico_network_utc("2026-10-31","12:00",utc));assert(clock.sync(utc,12345));assert(clock.utc(12345)==utc&&clock.local(12345)==utc-14400);assert(clock.utc(15945)==utc+3600);
    clock.zone.minutes=0;clock.zone.rule=pico_timezone::fixed;assert(clock.local(15945)==utc+3600);clock.zone=pico_timezone();
    assert(pico_network_utc("2026-03-08","06:59",utc)&&clock.sync(utc+59,0));int64_t before=clock.local(0);assert(clock.local(1)==before+3601);
    assert(pico_network_utc("2026-11-01","05:59",utc)&&clock.sync(utc+59,0));before=clock.local(0);assert(clock.local(1)==before-3599);
    assert(!clock.sync(0,0)&&clock.synchronized&&clock.local(0)==before);
    assert(pico_network_utc("2026-10-31","12:00",utc));clock.set_local(utc,50);assert(!clock.synchronized&&clock.local(50)==utc&&clock.utc(50)==utc+14400);
    puts("PASS: clock sync, monotonic advancement, immediate timezone changes, Toronto DST transitions, invalid sync and manual local RTC writes");
}
