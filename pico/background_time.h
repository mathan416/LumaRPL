#pragma once
#include "wifi_join.h"
// One cooperative clock session, bounded independently from delayed scheduling.
struct pico_background_time
{
    enum class phase:uint8_t { joining, resolving, reply, done };
    phase stage=phase::joining;
    pico_wifi_join association;
    uint32_t started=0,since=0;
    unsigned server=0;
    int64_t utc=0;
    explicit pico_background_time(uint32_t now=0):started(now),since(now){}
    template<class IO> void start(uint32_t now,IO &io)
    {*this=pico_background_time(now);association.start(now,io);}
    template<class IO> void tick(uint32_t now,IO &io)
    {
        if(stage==phase::done)return;
        if(uint32_t(now-started)>=55000){stage=phase::done;return;}
        if(stage==phase::joining)
        {
            association.tick(now,io);
            if(association.stage==pico_wifi_join::phase::failed){stage=phase::done;return;}
            if(association.stage==pico_wifi_join::phase::connected)
            {stage=phase::resolving;since=now;io.resolve(server);}
            return;
        }
        if(stage==phase::resolving)
        {
            if(io.resolved())
            {if(io.send()){stage=phase::reply;since=now;return;}}
            else if(!io.dns_failed()&&uint32_t(now-since)<4000)return;
        }
        else if(stage==phase::reply)
        {
            if(io.result(utc)){stage=phase::done;return;}
            if(uint32_t(now-since)<4000)return;
        }
        io.close_request();
        if(++server==2){stage=phase::done;return;}
        stage=phase::resolving;since=now;io.resolve(server);
    }
};
