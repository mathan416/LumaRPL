#include "timezone.h"
#include "network_time.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>
int main()
{
    pico_timezone toronto;int64_t epoch;
    assert(pico_network_utc("2026-03-08","06:59",epoch)&&toronto.offset(epoch)==-300);assert(toronto.offset(epoch+60)==-240);
    assert(pico_network_utc("2026-11-01","05:59",epoch)&&toronto.offset(epoch)==-240);assert(toronto.offset(epoch+60)==-300);
    assert(pico_network_utc("2026-10-31","12:00",epoch)&&toronto.local(epoch)==epoch-14400);
    int32_t minutes;assert(pico_timezone::fixed_offset("+05:45",minutes)&&minutes==345);assert(pico_timezone::fixed_offset("-03:30",minutes)&&minutes==-210);assert(!pico_timezone::fixed_offset("+14:01",minutes));assert(!pico_timezone::fixed_offset("+01:60",minutes));
    for(const auto &entry:pico_timezones){pico_timezone zone;zone.minutes=entry.minutes;zone.rule=entry.rule;assert(zone.valid());}
#ifdef TIMEZONE_VECTORS
#include "timezone-vectors.inc"
#endif
    puts("PASS: timezone offsets, Toronto DST transition boundaries and fixed fractional offsets");
}
