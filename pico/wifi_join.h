#pragma once
#include <stdint.h>
// Shared cooperative association/DHCP policy. SDK operations are supplied by IO;
// no sleep, polling loop or credential storage is hidden in this controller.
struct pico_wifi_join
{
    enum class phase:uint8_t { joining, backoff, connected, failed };
    enum class link:int8_t { waiting, up, missing, failed, bad_auth, no_ip };
    enum class failure:uint8_t { none, handshake, discovery, association, dhcp, request, timeout };
    phase stage=phase::joining;
    link observed=link::waiting;
    failure reason=failure::none;
    uint8_t attempts=0;
    uint32_t started=0,attempt_at=0,changed_at=0,backoff_at=0;
    template<class IO> void reject(uint32_t now,IO &io,failure why)
    {
        reason=why;io.reject();
        if(attempts>=3||uint32_t(now-started)>=35000)stage=phase::failed;
        else {stage=phase::backoff;backoff_at=now;}
    }
    template<class IO> void request(uint32_t now,IO &io)
    {
        ++attempts;stage=phase::joining;observed=link::waiting;
        attempt_at=changed_at=now;
        if(!io.request(attempts))reject(now,io,failure::request);
    }
    template<class IO> void start(uint32_t now,IO &io)
    { *this=pico_wifi_join();started=now;request(now,io); }
    template<class IO> void tick(uint32_t now,IO &io)
    {
        if(stage==phase::connected||stage==phase::failed)return;
        if(uint32_t(now-started)>=35000)
        {
            if(stage==phase::joining)io.reject();
            if(observed==link::no_ip)reason=failure::dhcp;
            else if(reason==failure::none)reason=failure::timeout;
            stage=phase::failed;return;
        }
        if(stage==phase::backoff)
        {
            // 250 ms station-settling interval, then 1 s / 3 s backoff.
            if(uint32_t(now-backoff_at)>=(attempts==1?1250u:3250u))request(now,io);
            return;
        }
        auto status=io.connection();
        if(status==link::up){stage=phase::connected;reason=failure::none;io.accept();return;}
        if(status!=observed){observed=status;changed_at=now;}
        if(uint32_t(now-attempt_at)<1000)return;
        if(status==link::missing&&uint32_t(now-changed_at)>=5000)reject(now,io,failure::discovery);
        else if((status==link::failed||status==link::bad_auth)&&uint32_t(now-changed_at)>=1000)
            reject(now,io,status==link::bad_auth?failure::handshake:failure::association);
    }
};
