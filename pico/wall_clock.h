#pragma once
#include "timezone.h"
// Monotonic UTC anchor plus a local display timezone; never apply a historical
// exchange-rate date to the clock. A manual local RTC write keeps legacy behavior.
struct pico_wall_clock
{
    int64_t local_offset=1767225600LL,utc_offset=0;
    pico_timezone zone;
    bool synchronized=false;
    int64_t utc(int64_t elapsed)const
    {
        if(synchronized)return utc_offset+elapsed;
        int64_t local=local_offset+elapsed;return local-int64_t(zone.offset(local-int64_t(zone.minutes)*60))*60;
    }
    int64_t local(int64_t elapsed)const{return synchronized?zone.local(utc_offset+elapsed):local_offset+elapsed;}
    bool sync(int64_t value,int64_t elapsed)
    {if(value<1577836800LL||value>2145916799LL)return false;utc_offset=value-elapsed;synchronized=true;return true;}
    void set_local(int64_t value,int64_t elapsed){local_offset=value-elapsed;synchronized=false;}
};
