#include "key_gate.h"
// PicoCalc LCD setup/transfer uses the ClockworkPi-derived UF2 Loader driver.
#include "platform.h"
#include "input.h"
#include "battery.h"
#include "battery_bus.h"
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/i2c.h"
#include "hardware/watchdog.h"
#include "hardware/structs/watchdog.h"
#include "lcdspi.h"
#include <string.h>

static bool dirty[320];
static bool ctrl;
static uint32_t last_poll;
static int held=-1;
static pico_key_gate transition_gate={-1,-1};
static pico_ui_performance ui_performance;
static uint64_t input_started;static bool input_pending;
void pico_ui_input_begin(void){input_started=to_us_since_boot(get_absolute_time());input_pending=true;}
void pico_ui_cursor_complete(void){ui_performance.cursor_us=ui_performance.input_us;ui_performance.cursor_rows=ui_performance.rows;if(ui_performance.cursor_us>ui_performance.max_cursor_us)ui_performance.max_cursor_us=ui_performance.cursor_us;}
void pico_ui_performance_snapshot(pico_ui_performance *out){*out=ui_performance;}
static uint32_t repeat_at;
static pico_battery_state battery;
static bool bios_checked;

void pico_mark_dirty(unsigned row)
{
    if (row < 320) dirty[row] = true;
}

void pico_devices_init(void)
{
    gpio_set_function(6, GPIO_FUNC_I2C);
    gpio_set_function(7, GPIO_FUNC_I2C);
    gpio_pull_up(6); gpio_pull_up(7);
    i2c_init(i2c1, 10000);
    // The keyboard controller boots later than the Pico when USB-powered.
    sleep_ms(600);
    lcd_spi_init();
    lcd_init();
    memset(dirty, 1, sizeof dirty);
}

void pico_display_refresh(const uint8_t *frame)
{
    uint64_t since=to_us_since_boot(get_absolute_time());unsigned transferred=0;
    uint8_t row[320 * 3];
    for (int y = 0; y < 320; ++y)
    {
        if (!dirty[y]) continue;++transferred;
        for (int x = 0; x < 320; ++x)
        {
            // DB48X MONOCHROME_REVERSE: X is mirrored; white bits are 1.
#ifdef CONFIG_COLOR
            pico_rgb_pixel(frame,x,y,&row[x*3]);
#else
            uint8_t v = pico_mono_pixel(frame,x,y);
            row[x*3] = row[x*3+1] = row[x*3+2] = v;
#endif
        }
        define_region_spi(0, y, 319, y, 1);
        hw_send_spi(row, sizeof row);
        spi_finish(Pico_LCD_SPI_MOD);
        lcd_spi_raise_cs();
        dirty[y] = false;
    }
    uint64_t now=to_us_since_boot(get_absolute_time());ui_performance.rows=transferred;ui_performance.refresh_us=(uint32_t)(now-since);ui_performance.input_us=input_pending?(uint32_t)(now-input_started):0;input_pending=false;
}

static int read_register(uint8_t reg,unsigned settle_ms)
{
    uint8_t data[2];
    if (i2c_write_timeout_us(i2c1, 0x1f, &reg, 1, false, 10000) != 1)
        return -1;
    if (settle_ms) sleep_ms(settle_ms);
    if (i2c_read_timeout_us(i2c1, 0x1f, data, 2, false, 10000) != 2)
        return -1;
    return data[0] | ((int)data[1] << 8);
}

int pico_keyboard_event(int *event_state)
{
    uint32_t now=to_ms_since_boot(get_absolute_time());
    if(now-last_poll<10) return -1;
    last_poll=now;int value=read_register(9,0);
    if(value<=0)return -1;
    int state=value&255,code=value>>8;
    if(!pico_key_gate_event(&transition_gate,code,state))return -1;
    if(code==0x7e || code==0xa5) { ctrl=state==1 || state==2;return -1; }
    *event_state=state;
    if(ctrl && code>='A' && code<='Z')return code-'A'+1;
    if(ctrl && code>='a' && code<='z')return code-'a'+1;
    if(ctrl && code==0x85)return 0x86;
    return code;
}
void pico_keyboard_reset(void) { held=-1; }
void pico_keyboard_transition(void){held=-1;pico_key_gate_transition(&transition_gate);}
int pico_keyboard_poll(void)
{
    int state=0,code=pico_keyboard_event(&state);
    uint32_t now=to_ms_since_boot(get_absolute_time());
    if(code<0)
    {
        if(held>=0 && (int32_t)(now-repeat_at)>=0) {repeat_at=now+80;return held;}
        return -1;
    }
    if(state==3) { if(held==code)held=-1;return -1; }
    if(state!=1)return -1;
    held=(!ctrl && ((code>=32 && code<127)||code==8||(code>=0xb4 && code<=0xb7)))?code:-1;
    repeat_at=now+500;return code;
}

static int battery_write(void *context,uint8_t reg,uint32_t timeout)
{(void)context;return i2c_write_timeout_us(i2c1,0x1f,&reg,1,false,timeout);}
static int battery_read(void *context,uint8_t *reply,uint32_t timeout)
{(void)context;return i2c_read_timeout_us(i2c1,0x1f,reply,2,false,timeout);}
static void battery_pause(void *context,unsigned ms)
{(void)context;sleep_ms(ms);}

int pico_battery_percent(void)
{
    pico_battery_bus bus={0,battery_write,battery_read,battery_pause};
    if(!bios_checked)
    {
        pico_battery_transaction version=pico_controller_read_bus(bus,0x01);
        battery.bios_version=pico_keyboard_bios(version.word);bios_checked=true;
    }
    // The official developer documents REG_ID_BAT starting at BIOS 1.2.
    if(pico_battery_bios_unsupported(battery.bios_version))
    {battery.last_reply=-1;battery.write_result=0;battery.read_result=0;battery.valid=0;battery.charging=0;return -1;}
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (pico_battery_due(&battery,now))
    {
        pico_battery_transaction result=pico_battery_read_bus(bus);
        battery.write_result=result.write_result;battery.read_result=result.read_result;
        pico_battery_update(&battery,to_ms_since_boot(get_absolute_time()),result.word);
    }
    return pico_battery_value(&battery);
}

void pico_battery_snapshot(pico_battery_state *out)
{bios_checked=false;battery.attempted=0;pico_battery_percent();*out=battery;}

void pico_backlight(int on)
{
    uint8_t msg[] = { 0x85, on ? 200 : 0 };
    i2c_write_timeout_us(i2c1, 0x1f, msg, sizeof msg, false, 10000);
}

void pico_return_to_loader(void)
{
    // UF2 Loader v2.4.1 stage3 command ABI: BOOT_SD=1, magic 0xe98cc638.
    watchdog_hw->scratch[1] = 1;
    watchdog_hw->scratch[2] = 0;
    watchdog_hw->scratch[0] = 0xe98cc638;
    watchdog_reboot(0, 0, 0);
    while (true) tight_loop_contents();
}

int pico_battery_charging(void) { pico_battery_percent(); return battery.valid?battery.charging:0; }

// Query the installed RP2350 partition table without erasing or writing flash.
#include "boot/picobin.h"
int pico_partition_fits(void)
{
    uint32_t work[1024],info[3];
    extern char __flash_binary_end;
    if (rom_load_partition_table((uint8_t *)work,sizeof work,false) != BOOTROM_OK) return 1;
    uint32_t flags=PT_INFO_PARTITION_LOCATION_AND_FLAGS | PT_INFO_SINGLE_PARTITION;
    if (rom_get_partition_table_info(info,3,flags)<0) return 1;
    unsigned first=(info[1]&PICOBIN_PARTITION_LOCATION_FIRST_SECTOR_BITS)>>PICOBIN_PARTITION_LOCATION_FIRST_SECTOR_LSB;
    unsigned last=(info[1]&PICOBIN_PARTITION_LOCATION_LAST_SECTOR_BITS)>>PICOBIN_PARTITION_LOCATION_LAST_SECTOR_LSB;
    if (!first && !last) return 1; // Standalone BOOTSEL build, no loader table.
    uint32_t bytes=(last+1-first)*4096u;
    return (uintptr_t)&__flash_binary_end-0x10000000u <= bytes;
}
