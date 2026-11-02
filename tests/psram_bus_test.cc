#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstring>
extern "C" {
#include "psram_bus.h"
#include "hardware/pio.h"
#include "psram_spi.pio.h"
static constexpr unsigned idle_pc=4+picocalc_psram_offset_idle;
#include "pico/stdlib.h"
static bool sm_available=true,program_available=true,claimed=false,stall=false,empty=false,cs_high=false;
static unsigned pc=0,length=0,tx_count=0,rx_count=0,rx_pos=0,ticks=0,removed=0;
static uint32_t tx[4],rx[4];
int pio_claim_unused_sm(PIO,bool panic){assert(!panic);if(!sm_available)return -1;claimed=true;return 0;}
void pio_sm_unclaim(PIO,uint){assert(claimed);claimed=false;}
bool pio_can_add_program(PIO,const pio_program *){return program_available;}
uint pio_add_program(PIO,const pio_program *p){assert(p->length==8);return 4;}
void pio_remove_program(PIO,const pio_program *,uint offset){assert(offset==4);++removed;}
pio_sm_config pio_get_default_sm_config(){return {};}
void sm_config_set_wrap(pio_sm_config *,uint a,uint b){assert(a==4&&b==idle_pc);}
void sm_config_set_sideset(pio_sm_config *,uint n,bool optional,bool direction){assert(n==2&&!optional&&!direction);}
void sm_config_set_out_pins(pio_sm_config *,uint pin,uint n){assert(pin==2&&n==1);}
void sm_config_set_in_pins(pio_sm_config *,uint pin){assert(pin==3);}
void sm_config_set_sideset_pins(pio_sm_config *,uint pin){assert(pin==20);}
void sm_config_set_out_shift(pio_sm_config *,bool right,bool automatic,uint n){assert(!right&&automatic&&n==32);}
void sm_config_set_in_shift(pio_sm_config *,bool right,bool automatic,uint n){assert(!right&&automatic&&n==32);}
void sm_config_set_clkdiv(pio_sm_config *c,float value){c->divider=value;}
unsigned clock_get_hz(unsigned){return 150000000;}
void pio_gpio_init(PIO,uint pin){assert(pin==2||pin==3||pin==20||pin==21);}
void pio_sm_set_consecutive_pindirs(PIO,uint,uint pin,uint n,bool output){assert((pin==2&&n==1&&output)||(pin==3&&n==1&&!output)||(pin==20&&n==2&&output));}
void pio_sm_init(PIO,uint,uint at,const pio_sm_config *c){assert(at==idle_pc&&std::fabs(c->divider-2.5f)<1e-6);pc=at;}
void pio_sm_exec(PIO,uint,uint code){assert((code&0x1800)==0x0800);if((code>>13)==7)length=(code&31)+1;else pc=code&31;}
void pio_sm_clear_fifos(PIO,uint){tx_count=rx_count=rx_pos=0;memset(rx,0,sizeof rx);}
void pio_sm_restart(PIO,uint){}
void pio_sm_put(PIO,uint,uint32_t word){assert(tx_count<4);tx[tx_count++]=word;}
void pio_sm_set_enabled(PIO,uint,bool enabled)
{
    if(!enabled||stall)return;assert(pc==4&&tx_count==(length+3)/4);
    if(!empty)for(unsigned i=0;i<length;++i){unsigned count=length-i/4*4;if(count>4)count=4;unsigned value=((tx[i/4]>>(24-(i%4)*8))&255)^0xa5;rx[i/4]|=value<<((count-1-i%4)*8);}
    rx_count=empty?0:(length+3)/4;pc=idle_pc;
}
uint pio_sm_get_pc(PIO,uint){return pc;}bool pio_sm_is_rx_fifo_empty(PIO,uint){return rx_pos==rx_count;}
uint32_t pio_sm_get(PIO,uint){assert(rx_pos<rx_count);return rx[rx_pos++];}
void gpio_init(unsigned){}void gpio_set_dir(unsigned,bool){}void gpio_put(unsigned pin,bool value){if(pin==20)cs_high=value;}void gpio_pull_down(unsigned){}void busy_wait_at_least_cycles(unsigned){}uint32_t time_us_32(){return ticks++;}
}
int main()
{
    sm_available=false;assert(!pico_psram_bus_init()&&!claimed);sm_available=true;program_available=false;assert(!pico_psram_bus_init()&&!claimed);program_available=true;assert(pico_psram_bus_init()&&claimed);
    uint8_t a[14],b[14];for(unsigned i=0;i<14;++i)a[i]=i*71+5;
    for(unsigned n=1;n<=13;++n){memset(b,0,sizeof b);assert(pico_psram_bus_transfer(a,b,n));for(unsigned i=0;i<n;++i)assert(b[i]==uint8_t(a[i]^0xa5));}
    assert(!pico_psram_bus_transfer(a,b,0)&&!pico_psram_bus_transfer(a,b,14)&&!pico_psram_bus_transfer(nullptr,b,1)&&!pico_psram_bus_transfer(a,nullptr,1));
    empty=true;assert(!pico_psram_bus_transfer(a,b,5));empty=false;assert(pico_psram_bus_transfer(a,b,5));
    stall=true;unsigned before=ticks;assert(!pico_psram_bus_transfer(a,b,13));assert(ticks-before<60&&!claimed&&cs_high&&removed==1);assert(!pico_psram_bus_transfer(a,b,1));stall=false;assert(pico_psram_bus_init()&&pico_psram_bus_transfer(a,b,13));
    puts("PASS: actual PIO bus C code, non-panicking claims, configuration, packed/partial FIFO words, missing RX, bounded timeout, CS deassertion and resource recovery");
}
