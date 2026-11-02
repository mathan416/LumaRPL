// Single core-0 owner. A ready driver may remain disconnected between boot
// retries; TLS/socket sessions are always torn down before ownership transfers.
struct radio_owner
{
    pico_network_diagnostics diagnostics{};
    bool initialized=false,station=false,foreground=false,live=false;
    uint32_t initialized_heap=0;
    void finish(uint8_t stage)
    {
        if(!live)return;
        auto &a=diagnostics.attempts[diagnostics.count-1];a.stage=stage;
        a.elapsed_ms=uint32_t(to_ms_since_boot(get_absolute_time())-a.started_ms);
        live=false;
    }
    void disconnect()
    {if(initialized&&station){cyw43_arch_disable_sta_mode();station=false;}}
    void shutdown()
    {disconnect();if(initialized){cyw43_arch_deinit();initialized=false;}}
    bool ready()
    {
        if(initialized)return true;
        uint32_t before=network_heap_used();
        if(cyw43_arch_init())return false;
        initialized=true;
        uint32_t after=network_heap_used();
        initialized_heap=after>before?after-before:0;diagnostics.driver_heap_bytes=initialized_heap;
        return true;
    }
    void begin(bool manual,unsigned attempt)
    {
        finish(PICO_NET_FAILED);
        if(diagnostics.count==3)memmove(diagnostics.attempts,diagnostics.attempts+1,2*sizeof(pico_network_attempt));
        else ++diagnostics.count;
        auto &a=diagnostics.attempts[diagnostics.count-1];a={};a.started_ms=to_ms_since_boot(get_absolute_time());
        a.manual=manual;a.attempt=attempt;a.link_status=-99;a.stage=PICO_NET_JOINING;live=true;
    }
    void stage(uint8_t value)
    {if(live)diagnostics.attempts[diagnostics.count-1].stage=value;}
    void event(unsigned interface,unsigned event,unsigned status,unsigned reason)
    {
        if(!live||interface!=CYW43_ITF_STA)return;
        auto &a=diagnostics.attempts[diagnostics.count-1];if(a.stage!=PICO_NET_JOINING)return;
        const bool relevant=event==0||event==3||event==5||event==6||event==7||event==11||event==12||event==23||event==46||event==49;
        const bool bad=(status!=0&&!(event==46&&status==6)&&!(event==3&&status==6))||reason!=0||event==5||event==6||event==11||event==12;
        if(!relevant||!bad||(event==0&&a.event_valid&&a.event!=0))return;
        a.event_valid=true;a.event=event;a.status=status;a.reason=reason;
    }
} radio;
struct wifi_join_io
{
    const char *ssid,*password;
    bool manual;
    pico_network_progress_fn progress=nullptr;
    void *context=nullptr;
    bool request(unsigned attempt)
    {
        // Only explicit foreground operations can perform synchronous full
        // initialization/recovery. Boot retries reuse the already-ready driver.
        if(manual&&attempt==3)radio.shutdown();
        radio.begin(manual,attempt);
        if(progress){char text[72];snprintf(text,sizeof text,"Wi-Fi attempt %u/3 (Esc cancels)",attempt);progress(context,text);}
        if((!manual&&!radio.initialized)||!radio.ready())return false;
        if(!radio.station)
        {
            uint32_t before=network_heap_used();cyw43_arch_enable_sta_mode();radio.station=true;
            uint32_t after=network_heap_used();
            radio.diagnostics.driver_heap_bytes=std::max(radio.diagnostics.driver_heap_bytes,radio.initialized_heap+(after>before?after-before:0));
        }
        return !cyw43_arch_wifi_connect_async(ssid,password,*password?CYW43_AUTH_WPA2_AES_PSK:CYW43_AUTH_OPEN);
    }
    pico_wifi_join::link connection()
    {
        using link=pico_wifi_join::link;
        int value=cyw43_tcpip_link_status(&cyw43_state,CYW43_ITF_STA);
        if(radio.live)radio.diagnostics.attempts[radio.diagnostics.count-1].link_status=value;
        switch(value)
        {
        case CYW43_LINK_UP:return link::up;
        case CYW43_LINK_NONET:return link::missing;
        case CYW43_LINK_FAIL:return link::failed;
        case CYW43_LINK_BADAUTH:return link::bad_auth;
        case CYW43_LINK_NOIP:return link::no_ip;
        default:return link::waiting;
        }
    }
    void reject(){radio.finish(PICO_NET_FAILED);radio.disconnect();}
    void accept(){radio.stage(PICO_NET_TIME);}
};
