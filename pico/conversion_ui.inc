// Modal, bounded conversion screens. No persistent framebuffer or heap arena.
namespace {
pico_currency_rates currency_active_rates;bool currency_initialized=false;
pico_currency_preferences currency_preferences;
bool currency_preferences_loaded=false,currency_preferences_dirty=false;
void currency_preferences_flush()
{
    if(!currency_preferences_dirty||!sys_disk_ok())return;
    pico_f_mkdir("currency");if(currency_preferences.save())currency_preferences_dirty=false;
}
void currency_preferences_open()
{
    if(!currency_preferences_loaded&&sys_disk_ok())
    {currency_preferences.load();currency_preferences_loaded=true;}
    currency_preferences_flush();
}
void currency_preferences_remember(const pico_currency_preferences &next)
{
    if(!next.valid())return;
    if(memcmp(&next,&currency_preferences,sizeof next))
    {currency_preferences=next;currency_preferences_loaded=true;currency_preferences_dirty=true;}
    currency_preferences_flush();
}
void currency_preferences_indices(const pico_currency_rates &rates,unsigned &from,unsigned &to)
{
    if(!currency_preferences.valid())return;
    from=rates.index(currency_preferences.from,0);to=rates.index(currency_preferences.to,rates.count>1?1:0);
}
bool conversion_pick_unit(char *unit,size_t capacity)
{
    const char *names[12];for(unsigned i=0;i<11;++i)names[i]=conversion_categories[i].name;names[11]="Custom / compound unit";
    int category=screen_menu("conversion_ui.conversion_pick_unit.choose1","Unit category",names,12);if(category<0)return false;
    if(category==11){char next[32];snprintf(next,sizeof next,"%s",unit);if(!prompt("Unit, e.g. kg*m/s^2",next,sizeof next))return false;cleaner temporary;unit_g check=pico_form_unit(next);if(!check){engine_error();return false;}snprintf(unit,capacity,"%s",next);return true;}
    const auto &group=conversion_categories[category];const char *labels[8];for(int i=0;i<group.count;++i)labels[i]=group.units[i].label;
    int selected=screen_menu("conversion_ui.conversion_pick_unit.choose2",group.name,labels,group.count);if(selected<0)return false;snprintf(unit,capacity,"%s",group.units[selected].symbol);return true;
}
void conversion_result(object_r result,const char *title,const char *context)
{
    screen_result("conversion_ui.conversion_result.value_result1",result,title,context);
}
void unit_conversion_menu()
{
    char amount[64]="1",from[32]="m",to[32]="ft";
    for(;;)
    {
        char value[88],source[64],target[64];snprintf(value,sizeof value,"Value: %s",amount);snprintf(source,sizeof source,"From: %s",from);snprintf(target,sizeof target,"To: %s",to);
        const char *items[]={"Convert",value,source,target,"Swap units","Help","Back"};int action=screen_menu("conversion_ui.unit_conversion_menu.choose1","Unit conversion",items,7);if(action<0||action==6)return;
        if(action==0){cleaner temporary;object_g result=pico_convert_units(amount,from,to);if(!result){engine_error();continue;}char context[160];snprintf(context,sizeof context,"%s %s -> %s",amount,from,to);conversion_result(result,"Unit conversion",context);}
        if(action==1)prompt("Value / expression (no units)",amount,sizeof amount);
        if(action==2)conversion_pick_unit(from,sizeof from);if(action==3)conversion_pick_unit(to,sizeof to);
        if(action==4){char swap[32];memcpy(swap,from,32);memcpy(from,to,32);memcpy(to,swap,32);}
        if(action==5){const char *help=pico_ui_help::tool_text("conversion_ui.unit_conversion_menu.view_text1");screen_text("conversion_ui.unit_conversion_menu.view_text1","Unit conversion help",help,strlen(help));}
    }
}
bool currency_load(pico_currency_rates &rates,bool &backup)
{
    return pico_currency_load("currency/CURRENT.rates",rates,backup);
}
bool currency_save(const pico_currency_rates &rates)
{
    if(!sys_disk_ok())return false;pico_f_mkdir("currency");return pico_currency_save("currency/CURRENT.rates",rates);
}
int currency_pick(const pico_currency_rates &rates,const char *title)
{
    if(!rates.count){notice("Currency","Import a rate table or add manual rates first");return -1;}
    const char *items[pico_currency_rates::maximum];for(unsigned i=0;i<rates.count;++i)items[i]=rates.rows[i].code;return screen_menu("conversion_ui.currency_pick.choose1",title,items,rates.count);
}
void wifi_status()
{
    for(;;)
    {
        pico_network_status status=pico_network_get_status();screen_page("conversion_ui.wifi_status.page1","Radio / connection status",pico_ui::Actions{{"Refresh",nullptr,nullptr,"Details","Back"}});
        ui_line(8,55,304,16,status.radio_on?"Radio: On":"Radio: Off",pico_ui::TextRole::status,pico_ui::text_colour());
        ui_line(8,83,304,16,status.connected?"Connection: Connected":"Connection: Disconnected",pico_ui::TextRole::status,pico_ui::text_colour());flush_display();
        int key=read_key_raw();if(key==27||key==0xb1||key==0x85)return;
        if(key==0x84)
        {
            pico_network_diagnostics d{};pico_network_get_diagnostics(&d);char text[700];
            int n=snprintf(text,sizeof text,"Clock retry: %s\nNext retry: %u s\nSDK init heap delta: %u bytes\n\n",d.clock_pending?"Pending":"None",unsigned((d.next_retry_ms+999)/1000),unsigned(d.driver_heap_bytes));
            if(!d.count)snprintf(text+n,sizeof text-size_t(n),"No connection attempts recorded this boot.");
            static const char *const stages[]={"Joining","Time sync","HTTPS","Success","Failed","Cancelled"};
            for(unsigned i=0;i<d.count&&size_t(n)<sizeof text;++i)
            {
                const auto &a=d.attempts[i];int used=snprintf(text+n,sizeof text-size_t(n),"%s #%u/3: %s | %u ms\nLink: %d\nEvent/status/reason: %s%u / %u / %u\n\n",a.manual?"Manual":"Startup",unsigned(a.attempt),a.stage<6?stages[a.stage]:"Unknown",unsigned(a.elapsed_ms),int(a.link_status),a.event_valid?"":"unavailable ",unsigned(a.event),unsigned(a.status),unsigned(a.reason));
                if(used<0)break;n+=used;
            }
            screen_text("conversion_ui.wifi_status.details","Recent network attempts",text,strlen(text));
        }
    }
}
void timezone_menu()
{
    const char *names[15];for(unsigned i=0;i<13;++i)names[i]=pico_timezones[i].name;names[13]="Custom fixed UTC offset";names[14]="Back";
    int choice=screen_menu("conversion_ui.timezone_menu.choose1","Clock timezone",names,15);if(choice<0||choice==14)return;pico_timezone zone=pico_get_timezone();
    if(choice<13){zone.minutes=pico_timezones[choice].minutes;zone.rule=pico_timezones[choice].rule;}
    else{char text[8]="+00:00";if(!prompt("Fixed UTC offset (+HH:MM)",text,sizeof text))return;int32_t minutes;if(!pico_timezone::fixed_offset(text,minutes)){notice("Timezone","Use +/-HH:MM, within +/-14:00");return;}zone.minutes=minutes;zone.rule=pico_timezone::fixed;}
    pico_set_timezone(zone);bool saved=false;if(sys_disk_ok()){pico_f_mkdir("network");saved=pico_timezone_save(zone);}notice(saved?"Timezone saved":"Timezone set for this boot",saved?"Used for local clock display after sync":"No saved change; check SD to retain timezone");
}
struct currency_update_options {int64_t utc=0;uint64_t manual_at=0;char rate_date[12]{};int64_t current_utc()const{return utc?utc+int64_t((to_us_since_boot(get_absolute_time())-manual_at)/1000000):0;}};
void currency_options(currency_update_options &options)
{
    for(;;)
    {
        char clock[72],date[64],zone_name[64],zone[84];pico_timezone_name(pico_get_timezone(),zone_name,sizeof zone_name);
        snprintf(clock,sizeof clock,"Clock: %s",options.utc?"Manual UTC override":"Automatic NTP");snprintf(date,sizeof date,"Rates date: %s",*options.rate_date?options.rate_date:"Latest");snprintf(zone,sizeof zone,"Timezone: %s",zone_name);
        const char *items[]={clock,date,zone,"Help","Back"};int action=screen_menu("conversion_ui.currency_options.choose1","Exchange-rate update options",items,5);if(action<0||action==4)return;
        if(action==0)
        {
            static const char *const sources[]={"Automatic NTP (default)","Manual current UTC date/time","Back"};int selected=screen_menu("conversion_ui.currency_options.choose2","Clock source",sources,3);if(selected==0)options.utc=0;
            if(selected==1)
            {
                time_t value=options.utc?time_t(options.current_utc()):time_t(pico_clock_utc());tm current=*gmtime(&value);char date[12],time[6];snprintf(date,sizeof date,"%04d-%02d-%02d",current.tm_year+1900,current.tm_mon+1,current.tm_mday);snprintf(time,sizeof time,"%02d:%02d",current.tm_hour,current.tm_min);
                int64_t utc;if(!prompt("Actual UTC date YYYY-MM-DD",date,sizeof date)||!prompt("Actual UTC time HH:MM",time,sizeof time))continue;
                if(!pico_network_utc(date,time,utc)){notice("Invalid UTC time","Use current UTC, within 2020-2037");continue;}options.utc=utc;options.manual_at=to_us_since_boot(get_absolute_time());
            }
        }
        if(action==1)
        {
            static const char *const dates[]={"Latest available rates","Historical date","Back"};int selected=screen_menu("conversion_ui.currency_options.choose3","Rate observation date",dates,3);if(selected==0)options.rate_date[0]=0;
            if(selected==1){char date[12];snprintf(date,sizeof date,"%s",*options.rate_date?options.rate_date:"2024-01-02");if(!prompt("Historical rates YYYY-MM-DD",date,sizeof date))continue;if(!pico_currency_rates::date_valid(date)){notice("Invalid date","Use YYYY-MM-DD");continue;}snprintf(options.rate_date,sizeof options.rate_date,"%s",date);}
        }
        if(action==2)timezone_menu();
        if(action==3){const char *help=pico_ui_help::tool_text("conversion_ui.currency_options.view_text1");screen_text("conversion_ui.currency_options.view_text1","Time and rate-date help",help,strlen(help));}
    }
}
void wifi_setup(pico_network_settings &settings)
{
    for(;;)
    {
        char ssid[56];snprintf(ssid,sizeof ssid,"Network: %s",*settings.ssid?settings.ssid:"not configured");
        const char *items[]={ssid,"Set Wi-Fi password (masked)","Use open network (no password)","Save credentials on SD","Forget saved credentials","Radio / connection status","Help","Clock timezone","Back"};int action=screen_menu("conversion_ui.wifi_setup.choose1","Wi-Fi setup",items,9);if(action<0||action==8)return;
        if(action==0){char next[33];snprintf(next,sizeof next,"%s",settings.ssid);if(prompt("Wi-Fi SSID (2.4 GHz)",next,sizeof next))snprintf(settings.ssid,sizeof settings.ssid,"%s",next);}
        if(action==1)
        {
            pico_network_settings next=settings;
            if(prompt("WPA2-AES password (8-63 ASCII)",next.password,sizeof next.password,false,false,nullptr,true))
            {if(strlen(next.password)<8)notice("Wi-Fi password","Use 8-63 characters");else{snprintf(settings.password,sizeof settings.password,"%s",next.password);settings.open=false;}}
        }
        if(action==2&&confirm("Use this network without a password?")){memset(settings.password,0,sizeof settings.password);settings.open=true;}
        if(action==3)
        {
            if(!settings.valid()){notice("Wi-Fi setup","Enter a network name and valid password");continue;}
            if(!sys_disk_ok()){notice("Wi-Fi setup","SD card unavailable; session setup still works");continue;}
            if(!confirm("Credentials will be readable on SD. Save?"))continue;pico_f_mkdir("network");bool saved=settings.save();if(saved)pico_network_credentials_saved(settings.ssid,settings.password);notice(saved?"Wi-Fi saved":"Save failed","Settings are in /db48x/network/WIFI.cfg");
        }
        if(action==4&&confirm("Forget Wi-Fi credentials and backups?"))
        {
            pico_network_time_cancel();bool ok=true;for(const char *path:{"network/WIFI.cfg","network/WIFI.cfg.bak","network/WIFI.cfg.rollback","network/WIFI.cfg.tmp"}){FRESULT code=pico_f_unlink(path);if(code!=FR_OK&&code!=FR_NO_FILE&&code!=FR_NO_PATH)ok=false;}settings.clear();notice("Wi-Fi forgotten",ok?"Session and SD credentials cleared":"Session cleared; SD removal failed");
        }
        if(action==7)timezone_menu();
        if(action==5)wifi_status();
        if(action==6){const char *help=pico_ui_help::tool_text("conversion_ui.wifi_setup.view_text1");screen_text("conversion_ui.wifi_setup.view_text1","Wi-Fi help",help,strlen(help));}
    }
}
int network_cancel(void *){int key=pico_keyboard_poll();return key==27||key==0xb1||key==0x85||key==0xd0||key==0x91;}
void network_progress(void *,const char *text){screen_page("conversion_ui.network_progress.page1","Currency HTTPS update",pico_ui::Actions{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});ui_line(8,55,304,16,text,pico_ui::TextRole::status,pico_ui::text_colour());ui_line(8,87,304,16,"Old rates kept until validation",pico_ui::TextRole::status,pico_ui::text_colour());flush_display();}
bool currency_refresh(pico_network_settings &settings,pico_currency_rates &rates,const currency_update_options &options)
{
    if(!settings.valid()){wifi_setup(settings);if(!settings.valid()){notice("Wi-Fi setup","Configure a network before refreshing");return false;}}
    char *body=new(std::nothrow) char[2048];if(!body){notice("Network memory","Not enough free heap; old rates retained");return false;}
    pico_network_result result;key_pop_all();pico_keyboard_reset();bool ok=pico_network_rates_dated(settings.ssid,settings.password,options.current_utc(),options.rate_date,body,2048,&result,network_cancel,network_progress,nullptr);key_pop_all();pico_keyboard_reset();
    if(result.utc_time)pico_clock_sync(result.utc_time);
    if(ok){pico_currency_rates next;ok=pico_currency_csv(body,strlen(body),next,options.rate_date);if(ok)rates=next;else snprintf(result.message,sizeof result.message,"Invalid/incomplete currency CSV; old rates retained");}
    delete[] body;char radio[100];
    if(result.radio_event_valid)snprintf(radio,sizeof radio,"Radio event: %u\nEvent status: %u | Reason: %u",unsigned(result.radio_event),unsigned(result.radio_status),unsigned(result.radio_reason));
    else snprintf(radio,sizeof radio,"Radio event: unavailable");
    char detail[560];snprintf(detail,sizeof detail,"%s\n\n%s\nTime: %s\nWi-Fi status: %d\nJoin time: %u ms | Requests: %u\nTLS error: %d\nTLS peak: %u bytes\nSession: %u bytes",result.message,radio,result.ntp_status==1?"NTP synchronized":options.utc?"Manual UTC override":"NTP not completed",result.link_status,unsigned(result.join_ms),result.join_attempts,result.tls_error,unsigned(result.tls_peak),unsigned(result.session_bytes));
    const char *cache=ok?(sys_disk_ok()?(currency_save(rates)?"Saved to SD; ready to convert now.":"Save failed; rates available for this session. Previous SD cache kept."):"SD unavailable; rates available for this session."):"Previous rates kept. Check Wi-Fi settings or try Refresh again.";
    char summary[480];snprintf(summary,sizeof summary,"%s\n\n%s\n\n%s",ok?"Validated reference rates are ready.":result.message,cache,result.ntp_status==1?"Clock synchronized by NTP.":options.utc?"Using manual UTC.":"Clock was not synchronized.");
    for(;;){int action=screen_text("conversion_ui.currency_refresh.view_text1",ok?"Rates refreshed":"Rates not updated",summary,strlen(summary),pico_ui::TextRole::prose,pico_ui::Actions{{nullptr,nullptr,nullptr,"Details","Back"}});if(action!=4)break;screen_text("conversion_ui.currency_refresh.details","Connection details",detail,strlen(detail));}return ok;
}
void currency_menu(pico_network_settings &settings)
{
    currency_update_options options;
    pico_currency_rates &rates=currency_active_rates;bool backup=false;
    bool loaded=currency_initialized|| (sys_disk_ok()&&currency_load(rates,backup));unsigned from=0,to=0;char amount[64]="100";
    if(loaded&&backup)notice("Currency","Recovered previous valid rate cache");
    if(sys_disk_ok()&&!loaded)
    {
        uint8_t data[pico_currency_rates::bytes];bool recovered=false;
        if(pico_resource_load("currency/REFERENCE.rates",data,sizeof data,recovered,[&](size_t n){return rates.decode(data,n);}))
            notice("Reference cache loaded","Check observation dates before converting");
    }
    currency_initialized=true;
    if(rates.count>1)to=1;
    currency_preferences_open();currency_preferences_indices(rates,from,to);
    for(;;)
    {
        char value[88],source[40],target[40];snprintf(value,sizeof value,"Amount: %s",amount);snprintf(source,sizeof source,"From: %s",currency_preferences.valid()?currency_preferences.from:rates.count?rates.rows[from].code:"--");snprintf(target,sizeof target,"To: %s",currency_preferences.valid()?currency_preferences.to:rates.count?rates.rows[to].code:"--");
        const char *items[]={"Convert",value,source,target,"Swap currencies","Refresh rates over Wi-Fi","Manage rates / settings","Rate details / help"};
        int selected=screen_menu("conversion_ui.currency_menu.choose1","Currency conversion",items,8,currency_preferences_dirty?"Pair kept for session; SD save unavailable":nullptr);if(selected<0)return;
        static constexpr int actions[]={0,1,2,3,4,9,-1,8};int action=actions[selected];
        if(selected==6)
        {
            static const char *const tools[]={"Import SD rate table","Add / edit manual EUR rate","Save current cache","Wi-Fi setup","Update options / date"};
            int tool=screen_menu("conversion_ui.currency_tools","Manage rates / settings",tools,5);if(tool<0)continue;
            static constexpr int tool_actions[]={5,6,7,10,11};action=tool_actions[tool];
        }
        if(action==0)
        {
            if(!rates.count){notice("Currency","No rates available; import or enter rates");continue;}
            if(currency_preferences.valid()&&(strcmp(rates.rows[from].code,currency_preferences.from)||strcmp(rates.rows[to].code,currency_preferences.to)))
            {notice("Saved currency unavailable","Refresh/import its rate or select another currency");continue;}
            cleaner temporary;object_g result=pico_convert_currency(amount,rates,from,to);if(!result){engine_error();continue;}
            char context[256],title[48],dates[64];
            snprintf(title,sizeof title,"%s to %s",rates.rows[from].code,rates.rows[to].code);
            if(!strcmp(rates.rows[from].date,rates.rows[to].date))snprintf(dates,sizeof dates,"Rate date: %s",rates.rows[from].date);
            else snprintf(dates,sizeof dates,"%s: %s | %s: %s",rates.rows[from].code,rates.rows[from].date,rates.rows[to].code,rates.rows[to].date);
            snprintf(context,sizeof context,"%s %s to %s\n%s\nSource: %s\nCached reference rates; fees excluded.",amount,rates.rows[from].code,rates.rows[to].code,dates,rates.provider);
            conversion_result(result,title,context);
        }
        if(action==1)prompt("Amount / expression",amount,sizeof amount);
        if(action==2||action==3)
        {
            int selected=currency_pick(rates,action==2?"From currency":"To currency");
            if(selected>=0)
            {
                pico_currency_preferences next=currency_preferences;
                if(!next.valid()){strcpy(next.from,rates.rows[from].code);strcpy(next.to,rates.rows[to].code);}
                strcpy(action==2?next.from:next.to,rates.rows[selected].code);
                currency_preferences_remember(next);currency_preferences_indices(rates,from,to);
            }
        }
        if(action==4&&(rates.count||currency_preferences.valid()))
        {
            pico_currency_preferences next;
            strcpy(next.from,currency_preferences.valid()?currency_preferences.to:rates.rows[to].code);
            strcpy(next.to,currency_preferences.valid()?currency_preferences.from:rates.rows[from].code);
            currency_preferences_remember(next);currency_preferences_indices(rates,from,to);
        }
        if(action==5)
        {
            if(!sys_disk_ok()){notice("Import","SD card unavailable");continue;}char path[320];if(!document_file("currency",".rates",path,sizeof path))continue;
            pico_currency_rates next;uint8_t data[pico_currency_rates::bytes];bool recovered=false;
            if(!pico_resource_load(path,data,sizeof data,recovered,[&](size_t n){return next.decode(data,n);})){notice("Invalid rates","Current table retained");continue;}
            rates=next;from=0;to=rates.count>1?1:0;currency_preferences_indices(rates,from,to);notice(recovered?"Backup rates imported":"Rates imported","Choose Save current cache to keep after restart");
        }
        if(action==6)
        {
            pico_currency_rates::row row;strcpy(row.code,"USD");strcpy(row.rate,"1");
            if(!prompt("Three-letter currency code",row.code,sizeof row.code))continue;for(int i=0;i<3;++i)if(row.code[i]>='a'&&row.code[i]<='z')row.code[i]-=32;
            if(!pico_currency_rates::code_valid(row.code)){notice("Currency","Use three ASCII letters, e.g. CAD");continue;}
            unsigned index=0;while(index<rates.count&&strcmp(row.code,rates.rows[index].code))++index;if(index<rates.count)row=rates.rows[index];
            if(!prompt("Units per 1 EUR (EUR must be 1)",row.rate,sizeof row.rate)||!prompt("Rate date YYYY-MM-DD",row.date,sizeof row.date))continue;
            if(!pico_currency_rates::rate_valid(row.rate)||!pico_currency_rates::date_valid(row.date)||(!strcmp(row.code,"EUR")&&strtod(row.rate,nullptr)!=1)){notice("Invalid rate","Use a positive decimal and a valid date");continue;}
            if(index==rates.maximum){notice("Currency","Rate table full (32 currencies)");continue;}
            rates.rows[index]=row;if(index==rates.count)++rates.count;strcpy(rates.provider,"Manual / mixed; verify dates");currency_preferences_indices(rates,from,to);
        }
        if(action==7){bool ok=currency_save(rates);notice(ok?"Rates saved":"Save unavailable",ok?"Cache saved under /db48x/currency":"Need SD card and a valid rate table");}
        if(action==9){char from_code[4]={},to_code[4]={};if(rates.count){strcpy(from_code,rates.rows[from].code);strcpy(to_code,rates.rows[to].code);}
            if(currency_refresh(settings,rates,options)){from=rates.index(from_code,0);to=rates.index(to_code,rates.count>1?1:0);currency_preferences_indices(rates,from,to);pico_ui::focus("conversion_ui.currency_menu.choose1").selected=0;}}
        if(action==10)wifi_setup(settings);
        if(action==11)currency_options(options);
        if(action==8)
        {
            char help[1000];snprintf(help,sizeof help,"Provider: %s\nCurrencies: %u\n\nRates are units per 1 EUR. Cross conversion = amount * target rate / source rate. Each currency keeps its observation date. Dates can differ; all results are labelled cached and freshness is not assumed. No bank fees or spreads are included.\n\nImport a .rates file from /db48x/currency, then Save current cache to make it the startup table. A corrupt cache can recover its previous valid backup.\n\nUse Refresh over Wi-Fi / HTTPS, the computer updater, or manual dated rates. No SD is needed for manual conversion in this session.\n\nConversions leave the stack unchanged unless you press F1 Copy to stack.",rates.provider,rates.count);screen_text("conversion_ui.currency_menu.view_text1","Cached currency rates",help,strlen(help));
        }
    }
}
}
void pico_conversions_menu()
{
    if(rt.editing()){notice("Conversions","Finish with Enter or cancel with Esc before opening conversions");return;}
    save<bool> direct_screen(user_display_enable,false);pico_network_settings settings;if(sys_disk_ok())settings.load();
    for(;;){static const char *const items[]={"Unit conversion","Cached currency rates","Wi-Fi setup","Back"};int action=screen_menu("conversion_ui.pico_conversions_menu.choose1","Conversions",items,4);if(action<0||action==3)break;if(action==0)unit_conversion_menu();if(action==1)currency_menu(settings);if(action==2)wifi_setup(settings);}key_pop_all();
}

void pico_wifi_menu()
{
    save<bool> direct_screen(user_display_enable,false);pico_network_settings settings;if(sys_disk_ok())settings.load();wifi_setup(settings);key_pop_all();
}
