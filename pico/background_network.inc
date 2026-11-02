// Included by network.cc: same core-0 poll owner as the on-demand client.
struct boot_time_client;
boot_time_client *boot_time=nullptr;
uintptr_t boot_dns_generation=0;
struct boot_time_client
{
    pico_background_time flow;
    char ssid[33]{},password[64]{};
    bool dns_ready=false,dns_error=false,ready=false,waiting=false,paused=false;
    uint8_t sessions=0;uint32_t lifetime=0,due_at=0;
    ip_addr_t address{};
    udp_pcb *udp=nullptr;
    uintptr_t token=0;
    uint8_t ntp_request[48]{};
    uint32_t sent=0,last_poll=0;
    int64_t received_utc=0;
    explicit boot_time_client(uint32_t now):flow(now),lifetime(now){}
    void clear_credentials()
    {
        volatile char *s=ssid,*p=password;
        for(size_t i=0;i<sizeof ssid;++i)s[i]=0;
        for(size_t i=0;i<sizeof password;++i)p[i]=0;
    }
    bool request(unsigned attempt){wifi_join_io io{ssid,password,false};return io.request(attempt);}
    pico_wifi_join::link connection(){wifi_join_io io{ssid,password,false};return io.connection();}
    void reject(){wifi_join_io io{ssid,password,false};io.reject();}
    void accept(){wifi_join_io io{ssid,password,false};io.accept();}
    void begin(uint32_t now){waiting=false;++sessions;flow.start(now,*this);}
    bool schedule(uint32_t now)
    {
        close_request();radio.finish(PICO_NET_FAILED);radio.disconnect();
        if(sessions>=4||uint32_t(now-lifetime)>=600000)return false;
        static constexpr uint32_t delays[]={30000,120000,300000};
        if(delays[sessions-1]>=600000u-uint32_t(now-lifetime))return false;
        due_at=now+delays[sessions-1];waiting=true;return true;
    }
    static void dns_reply(const char *,const ip_addr_t *value,void *context)
    {
        // A delayed lookup must never touch a freed/replacement client.
        if(!boot_time||boot_time->token!=reinterpret_cast<uintptr_t>(context))return;
        if(value){boot_time->address=*value;boot_time->dns_ready=true;}
        else boot_time->dns_error=true;
    }
    void resolve(unsigned server)
    {
        dns_ready=dns_error=false;token=++boot_dns_generation;if(!token)token=++boot_dns_generation;
        const char *name=server?"time.google.com":"time.cloudflare.com";
        err_t error=dns_gethostbyname(name,&address,dns_reply,reinterpret_cast<void *>(token));
        dns_ready=error==ERR_OK;dns_error=error!=ERR_OK&&error!=ERR_INPROGRESS;
    }
    bool resolved()const{return dns_ready;}
    bool dns_failed()const{return dns_error;}
    static void reply(void *context,udp_pcb *,pbuf *packet,const ip_addr_t *source,uint16_t port)
    {
        auto &s=*static_cast<boot_time_client *>(context);
        uint32_t now=to_ms_since_boot(get_absolute_time());
        if(packet&&port==123&&ip_addr_cmp(source,&s.address)&&packet->tot_len>=48&&packet->tot_len<=512&&!s.ready)
        {
            uint8_t data[48];pbuf_copy_partial(packet,data,48,0);
            s.ready=pico_ntp_packet::reply(data,packet->tot_len,s.ntp_request,uint32_t(now-s.sent),s.received_utc);
        }
        if(packet)pbuf_free(packet);
    }
    bool send()
    {
        token=0; // DNS complete; invalidate duplicate callbacks.
        udp=udp_new_ip_type(IPADDR_TYPE_V4);if(!udp)return false;
        pico_ntp_packet::request(ntp_request,get_rand_64()|1);ready=false;
        udp_recv(udp,reply,this);
        if(udp_connect(udp,&address,123)!=ERR_OK)return false;
        pbuf *packet=pbuf_alloc(PBUF_TRANSPORT,48,PBUF_RAM);if(!packet)return false;
        err_t result=pbuf_take(packet,ntp_request,48);sent=to_ms_since_boot(get_absolute_time());
        if(result==ERR_OK)result=udp_send(udp,packet);
        pbuf_free(packet);return result==ERR_OK;
    }
    bool result(int64_t &utc)const{if(ready)utc=received_utc;return ready;}
    void close_request()
    {
        token=0;
        if(udp){udp_recv(udp,nullptr,nullptr);udp_remove(udp);udp=nullptr;}
    }
    ~boot_time_client(){close_request();clear_credentials();}
};
static_assert(sizeof(boot_time_client)+sizeof(radio_owner)+sizeof(wifi_join_io)+sizeof(pico_wifi_join)+2*sizeof(uintptr_t)+sizeof(pico_compute_gate)<=512,"Keep application retry/diagnostic state within 512 bytes");
void stop_boot_time()
{
    auto *previous=boot_time;boot_time=nullptr;delete previous;
    if(!radio.foreground)radio.shutdown();
}

void start_boot_time(const char *ssid,const char *password)
{
    if(boot_time||!ssid||!*ssid||!password||strlen(ssid)>32||strlen(password)>63)return;
    auto *state=new(std::nothrow) boot_time_client(to_ms_since_boot(get_absolute_time()));
    if(!state)return;
    snprintf(state->ssid,sizeof state->ssid,"%s",ssid);snprintf(state->password,sizeof state->password,"%s",password);
    // Driver initialization is done once during initial boot, before the
    // calculator accepts input. All network waiting is cooperative thereafter.
    boot_time=state;
    if(!radio.ready()){radio.begin(false,1);radio.finish(PICO_NET_FAILED);stop_boot_time();return;}
    state->begin(to_ms_since_boot(get_absolute_time()));
}
int64_t poll_boot_time(void)
{
    if(!boot_time||boot_time->paused||radio.foreground)return 0;
    uint32_t now=to_ms_since_boot(get_absolute_time());
    if(uint32_t(now-boot_time->last_poll)<5)return 0;
    boot_time->last_poll=now;
    if(uint32_t(now-boot_time->lifetime)>=600000){radio.finish(PICO_NET_FAILED);stop_boot_time();return 0;}
    cyw43_arch_poll();
    if(boot_time->waiting)
    {
        if(int32_t(now-boot_time->due_at)>=0)boot_time->begin(now);
        return 0;
    }
    boot_time->flow.tick(now,*boot_time);
    if(boot_time->flow.stage!=pico_background_time::phase::done)return 0;
    int64_t utc=boot_time->flow.utc;
    if(utc){radio.finish(PICO_NET_SUCCESS);stop_boot_time();}
    else if(!boot_time->schedule(now))stop_boot_time();
    return utc;
}

void pause_boot_time(const char *ssid,const char *password)
{
    radio.foreground=true;
    if(boot_time)
    {
        if(strcmp(ssid,boot_time->ssid)||strcmp(password,boot_time->password))stop_boot_time();
        else {boot_time->close_request();boot_time->paused=true;}
    }
    radio.finish(PICO_NET_CANCELLED);radio.disconnect();
}
void finish_foreground_time(bool synchronized,bool cancelled)
{
    radio.foreground=false;
    if(synchronized||cancelled){stop_boot_time();return;}
    if(boot_time)
    {
        uint32_t now=to_ms_since_boot(get_absolute_time());boot_time->paused=false;
        if(!radio.initialized||uint32_t(now-boot_time->lifetime)>=600000||(!boot_time->waiting&&!boot_time->schedule(now)))stop_boot_time();
        else radio.disconnect();
    }
    else radio.shutdown();
}
