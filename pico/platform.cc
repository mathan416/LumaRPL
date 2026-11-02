// PicoCalc implementation of the DMCP services used by DB48X.
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include <cstdio>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <malloc.h>
#include "memory_observer.h"
#include "dmcp.h"
#include "target.h"
#include "platform.h"
#include "wall_clock.h"
#include "network.h"
#include "network_settings.h"
#include "psram.h"
#include "input.h"
#include "reference_input.h"
#include "runtime.h"
#include "settings.h"
#include "utf8.h"
#include "sysmenu.h"
#include "user_interface.h"

extern const unsigned char PicoHeaderFont_sparse_font_data[];
font_p picocalc_header_font()
{
    return font_p(PicoHeaderFont_sparse_font_data);
}

alignas(8) static uint8_t framebuffer[320 * 320 * BITS_PER_PIXEL / 8];
static FATFS filesystem;
static bool mounted;
static uint32_t deadlines[4];
static bool timers[4];
static int queue[64], head, tail;
#ifdef CONFIG_COLOR
static char statefile[128] = "state/Session-colour.48S";
#else
static char statefile[128] = "state/Session.48S";
#endif
static char auxiliary[AUX_BUF_SIZE];
static disp_stat_t display_state;
extern const line_font_t *dmcp_fonts[];
extern const uint dmcp_fonts_count;
static uint32_t timer_counters[2];
static pico_wall_clock wall_clock;
pico_timezone pico_get_timezone(){return wall_clock.zone;}
void pico_set_timezone(const pico_timezone &zone){if(zone.valid())wall_clock.zone=zone;}
int64_t pico_clock_utc(){return wall_clock.utc(to_us_since_boot(get_absolute_time())/1000000);}
void pico_clock_sync(int64_t utc){wall_clock.sync(utc,to_us_since_boot(get_absolute_time())/1000000);}
sys_sdb_t sdb = {};

static void enqueue(int key)
{
    int next = (head+1)%64;
    if (next != tail) { queue[head] = key; head = next; }
}

extern "C" void pico_poll_background(void)
{
    if(int64_t utc=pico_network_time_poll())pico_clock_sync(utc);
}

static void poll()
{
    pico_poll_background();
    int code = pico_keyboard_poll();
    if (code < 0) return;
    int key = pico_translate_key(code);
    // Dismiss visible errors without executing the underlying fifth soft key.
    if(rt.error()&&key==42)key=33;
    if (key > 0)
    {
        pico_ui_input_begin();
        if (ST(STAT_OFF))
        {
            if (key != KB_ESC) return;
            CLR_ST(STAT_PGM_END); CLR_ST(STAT_SUSPENDED);
        }
        enqueue(key); enqueue(0);
    }
}

extern "C" {
uint8_t *lcd_line_addr(int y) { return framebuffer + std::clamp(y, 0, 319)*320*BITS_PER_PIXEL/8; }
void lcd_clear_buf() { memset(framebuffer, 255, sizeof framebuffer); for(unsigned y=0;y<320;y++) pico_mark_dirty(y); }
void lcd_refresh() { pico_display_refresh(framebuffer); }
void lcd_refresh_dma() { lcd_refresh(); }
void lcd_refresh_wait() { lcd_refresh(); }
void lcd_forced_refresh() { for(unsigned y=0;y<320;y++) pico_mark_dirty(y); lcd_refresh(); }
void lcd_refresh_lines(int y,int count) { for(int n=0;n<count;n++) pico_mark_dirty(y+n); lcd_refresh(); }
void LCD_power_off(int) { pico_backlight(0); }
void LCD_power_on() { pico_backlight(1); }
void lcd_set_buf_cleared(int) {}
int lcd_get_buf_cleared() { return 0; }
void lcd_fill_rect(uint32_t x,uint32_t y,uint32_t w,uint32_t h,int v)
{ Screen.fill(x,y,x+w-1,y+h-1,v ? pattern::black : pattern::white); mark_dirty(x,y,x+w,y+h); }

int key_empty() { poll(); return head == tail; }
int key_push(int k) { enqueue(k); return 1; }
int key_tail() { return key_empty() ? -1 : queue[tail]; }
int key_pop() { if(key_empty()) return -1; int k=queue[tail]; tail=(tail+1)%64; return k; }
void key_pop_all() { head=tail=0;pico_keyboard_transition(); }
int sys_last_key() { return key_tail(); }
int pico_read_key(int *a,int *b) { *a=0; *b=0; return 0; }
void wait_for_key_release(int) { key_pop_all(); }
void wait_for_key_press()
{
    for (;;) { while(key_empty()) sleep_ms(5); int key=key_pop();if(key==13||key==33||key==42)break; }
}
int runner_get_key(int*) { while(key_empty()) sleep_ms(5); return key_pop(); }
void reset_auto_off() {}
int is_menu_auto_off() { return 0; }
int is_auto_off() { return 0; }
uint32_t sys_current_ms() { return to_ms_since_boot(get_absolute_time()); }
uint32_t sys_tick_count() { return sys_current_ms(); }
void sys_delay(uint32_t ms) { sleep_ms(ms); }
void sys_timer_start(int t,uint32_t ms) { if(t>=0&&t<4) {timers[t]=true; deadlines[t]=sys_current_ms()+ms;} }
void sys_timer_disable(int t) { if(t>=0&&t<4) timers[t]=false; }
int sys_timer_active(int t) { return t>=0&&t<4&&timers[t]; }
int sys_timer_timeout(int t) { return sys_timer_active(t) && int32_t(sys_current_ms()-deadlines[t])>=0; }
void sys_critical_start() {}
void sys_critical_end() {}
void sys_sleep()
{
    static uint32_t last_wake;
    sleep_ms(5); poll();
    uint32_t now=sys_current_ms();
    uint32_t period=ST(STAT_CLK_WKUP_SECONDS)?1000:60000;
    if (ST(STAT_CLK_WKUP_ENABLE) && uint32_t(now-last_wake)>=period)
    {
        last_wake=now;
        SET_ST(STAT_CLK_WKUP_FLAG);
    }
}
void sys_reset() { watchdog_reboot(0,0,0); while(true) tight_loop_contents(); }
int sys_free_mem() { return 0; } // Runtime owns a statically bounded arena.
int usb_powered() { return pico_battery_charging(); }
int get_vbat() { int p=pico_battery_percent(); return p < 0 ? 101 : std::clamp(p,0,100); }
uint32_t read_power_voltage() { return get_vbat(); }
int get_lowbat_state() { int p=pico_battery_percent(); return p>0 && p<=2; }
uint8_t get_hw_id() { return 0; }
void start_buzzer_freq(uint32_t frequency)
{
    if(!frequency) return;
    gpio_set_function(26,GPIO_FUNC_PWM);
    unsigned slice=pwm_gpio_to_slice_num(26);
    pwm_set_clkdiv(slice,float(clock_get_hz(clk_sys))/1000000.0f);
    uint32_t wrap=std::clamp<uint32_t>(1000000000u/frequency,2u,65536u)-1;
    pwm_set_wrap(slice,wrap); pwm_set_gpio_level(26,wrap/2); pwm_set_enabled(slice,true);
}
void stop_buzzer() { pwm_set_gpio_level(26,0); }
void rtc_read(tm_t *t,dt_t *d)
{
    int64_t elapsed=to_us_since_boot(get_absolute_time())/1000000;
    time_t value=wall_clock.local(elapsed); // Unset clock: 2026-01-01
    struct tm *v=gmtime(&value);
    *d={uint16_t(v->tm_year+1900),uint8_t(v->tm_mon+1),uint8_t(v->tm_mday)};
    *t={uint8_t(v->tm_hour),uint8_t(v->tm_min),uint8_t(v->tm_sec),uint8_t((sys_current_ms()%1000)/10),uint8_t((v->tm_wday+6)%7)};
}
void rtc_write(tm_t *t,dt_t *d)
{
    pico_network_time_cancel(); // An explicit clock edit takes precedence over boot NTP.
    struct tm v={};
    v.tm_year=d->year-1900;v.tm_mon=d->month-1;v.tm_mday=d->day;
    v.tm_hour=t->hour;v.tm_min=t->min;v.tm_sec=t->sec;
    wall_clock.set_local(mktime(&v),to_us_since_boot(get_absolute_time())/1000000);
}
void rtc_wakeup_delay() {}
int julian_day(dt_t *d) { int a=(14-d->month)/12,y=d->year+4800-a,m=d->month+12*a-3; return d->day+(153*m+2)/5+365*y+y/4-y/100+y/400-32045; }
void julian_to_date(int j,dt_t *d)
{ int a=j+32044,b=(4*a+3)/146097,c=a-(146097*b)/4,e=(4*c+3)/1461,f=c-(1461*e)/4,m=(5*f+2)/153; *d={uint16_t(100*b+e-4800+m/10),uint8_t(m+3-12*(m/10)),uint8_t(f-(153*m+2)/5+1)}; }
const char *get_wday_shortcut(int n) { static const char *v[]={"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};return v[(n%7+7)%7]; }
const char *get_month_shortcut(int n) { static const char *v[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};return v[std::clamp(n,1,12)-1]; }
void print_dmy_date(char *s,int n,dt_t *d,const char *a,int,char sep) { snprintf(s,n,"%04u%c%02u%c%02u%s",d->year,sep,d->month,sep,d->day,a?a:""); }
void print_clk24_time(char *s,int n,tm_t *t,int sec,int) { if(sec)snprintf(s,n,"%02u:%02u:%02u",t->hour,t->min,t->sec);else snprintf(s,n,"%02u:%02u",t->hour,t->min); }
char *get_reset_state_file() { return statefile; }
void set_reset_state_file(const char *s) { if(s != statefile) snprintf(statefile,sizeof statefile,"%s",s); }
void set_reset_magic(uint32_t) {}
int is_reset_state_file() { return *statefile!=0; }
int sys_disk_ok() { return mounted; }
int sys_disk_write_enable(int) { return mounted; }
int sys_is_disk_write_enable() { return mounted; }
void sys_disk_check_valid() {}
int file_exists(const char *s) { FILINFO info; return f_stat(s,&info)==FR_OK; }
int file_size(const char *s) { FILINFO info; return f_stat(s,&info)==FR_OK ? info.fsize : -1; }
int check_create_dir(const char *s) { FRESULT r=f_mkdir(s);return r==FR_OK || r==FR_EXIST; }
char *aux_buf_ptr() { return auxiliary; }
void *write_buf_ptr() { return auxiliary; }
int write_buf_size() { return sizeof auxiliary; }
void sys_clear_write_buf_used() {}
int sys_write_buf_used() { return 0; }
// Optional SwissMicros facilities are unsupported, never flash the Pico.
int create_screenshot(int) { return 2; }
void draw_power_off_image(int) { lcd_refresh(); }
void reset_off_image_cycle() {}
void lcd_switchFont(disp_stat_t *ds,int nr)
{
    if (nr<0) nr=0;
    if (nr>=int(dmcp_fonts_count)) nr=1;
    ds->f=dmcp_fonts[nr];
}
int power_check_screen() { return 0; }
void lcd_writeClr(disp_stat_t*) { lcd_clear_buf(); }
int lcd_for_calc(int) { return 0; }
void msg_box(disp_stat_t*,const char *s,int) { ui.draw_message("PicoCalc",s);lcd_refresh(); }
int handle_menu(const smenu_t*,int,int) { pico_setup_menu();key_pop_all();return 0; }
int file_selection_screen(const char*,const char*,const char*,file_sel_fn_t,int,int,void*) { ui.draw_message("Use native tools","Ctrl+P programs / source editor","Ctrl+T session setup");lcd_refresh();wait_for_key_press();return 1; }
void rtc_check_unset() {}
void run_set_time() {}
void run_set_date() {}
void sys_flashing_init() {}
void sys_flashing_finish() {}
int sys_flash_erase_block(void*,uint32_t) { return -1; }
int sys_flash_write_block(void*,uint8_t*,uint32_t) { return -1; }
const smenu_t MID_SYS_WARN = {};
const smenu_t MID_SYSTEM = {};
const smenu_t MID_DMCP = {};
const smenu_t MID_BASE_SETUP = {};
}

#include "calculator_shortcuts.h"

bool pico_handle_shortcut(int key)
{
    if(pico_reference_input(ui,key,pico_reference_find))return true;
    if(pico_calculator_menu_shortcut(ui,key,pico_open_calculator_menu))return true;
    if (key == PICO_LEFT || key == PICO_RIGHT)
    {
        if (rt.editing())
        {
            uint pos = ui.cursor_position();
            pos = key == PICO_LEFT ? utf8_previous(rt.editor(), pos)
                : utf8_next(rt.editor(), pos, rt.editing());
            ui.cursor_position(pos);
        }
        return true;
    }
    if (key == PICO_SETUP) { pico_setup_menu(); return true; }
    if (key == PICO_PROGRAMS) { pico_program_browser(); return true; }
    if (key == PICO_CONVERSIONS) { pico_conversions_menu(); return true; }
    if (key == PICO_NOTEBOOK) { pico_notebooks_menu(); return true; }
    if (key == PICO_FORMS) { pico_forms_menu(); return true; }
    if (key == PICO_DATA) { pico_data_menu(); return true; }
    if (key == PICO_WORKBENCH) { pico_workbench_menu(); return true; }
    if (key == PICO_GRAPH) { pico_graph_menu(); return true; }
    if(key<PICO_SAVE||key>PICO_EXIT) return false;
    if(!pico_prepare_session_action())return true;
    if(key==PICO_EXIT&&!pico_prepare_exit())return true;
    bool ok;
    if (key == PICO_LOAD)
    {
#ifdef CONFIG_COLOR
        const char *recovery="state/BeforeLoad-colour.48S";
#else
        const char *recovery="state/BeforeLoad.48S";
#endif
        char original[128];
        snprintf(original,sizeof original,"%s",get_reset_state_file());
        ok = file_exists(original) && save_state_file_silent(recovery);
        set_reset_state_file(original);
        if (ok)
        {
            rt.reset(); Settings=settings(); load_saved_keymap();
            ok=load_state_file_silent(original);
            if (!ok)
            {
                rt.reset(); Settings=settings(); load_saved_keymap();
                load_state_file_silent(recovery);
            }
            set_reset_state_file(original);
        }
    }
    else ok=save_system_state_silent();
    if(key==PICO_EXIT && ok) pico_return_to_loader();
    ui.draw_message(ok ? "Session saved/loaded" : "SD operation failed",ok ? "F5 returns to the calculator" : "Check SD card; state retained");
    lcd_refresh();wait_for_key_press();return true;
}

extern "C" size_t pico_heap_committed(void);
extern "C" size_t pico_heap_committed_peak(void);
namespace { volatile uint32_t *stack_mark_end=nullptr; }
extern "C" void pico_stack_watermark_init(void){
    extern char __StackBottom;uintptr_t sp;asm volatile("mov %0, sp":"=r"(sp));
    stack_mark_end=reinterpret_cast<volatile uint32_t *>((sp-128)&~uintptr_t(3));
    for(volatile uint32_t *p=reinterpret_cast<volatile uint32_t *>(&__StackBottom);p<stack_mark_end;++p)*p=0x6d51c0de;
}
extern "C" pico_memory_observation pico_memory_observe(void){
    extern char __StackBottom,__StackTop;volatile uint32_t *p=reinterpret_cast<volatile uint32_t *>(&__StackBottom);
    if(stack_mark_end)while(p<stack_mark_end&&*p==0x6d51c0de)++p;
    return {size_t(mallinfo().uordblks),pico_heap_committed(),pico_heap_committed_peak(),stack_mark_end?size_t(reinterpret_cast<uintptr_t>(&__StackTop)-reinterpret_cast<uintptr_t>(p)):0,1};
}
int main()
{
    extern char __StackBottom;
    asm volatile("msr msplim, %0" :: "r"(&__StackBottom));
    pico_stack_watermark_init();
    pico_devices_init();
    pico_psram_init();
    sdb.pds_t20=sdb.pds_t24=sdb.pds_fReg=&display_state;
    timer2_counter=&timer_counters[0];timer3_counter=&timer_counters[1];
    lcd_clear_buf();
    FRESULT mount_result=FR_NOT_READY;
    for (int attempt=0;attempt<3 && mount_result!=FR_OK;++attempt)
    {
        mount_result=f_mount(&filesystem,"",1);
        if (mount_result!=FR_OK) sleep_ms(150);
    }
    if(mount_result==FR_OK)
    {
        
        #undef f_mkdir
        f_mkdir("/db48x");
#define f_mkdir pico_f_mkdir
        mounted=f_chdir("/db48x")==FR_OK;
        if(mounted) { f_mkdir("state");f_mkdir("config"); }
    }
    if(mounted){pico_network_settings credentials;if(credentials.load())pico_network_time_start(credentials.ssid,credentials.password);}
    program_main();
    return 0;
}

using palette_pattern=blitter::pattern<blitter::RGB_8BPP>;
const palette_pattern palette_pattern::black(0,0,0),palette_pattern::gray10(32,32,32),palette_pattern::gray25(64,64,64),palette_pattern::gray50(128,128,128),palette_pattern::gray75(192,192,192),palette_pattern::gray90(224,224,224),palette_pattern::white(255,255,255),palette_pattern::invert(UINT64_MAX);
