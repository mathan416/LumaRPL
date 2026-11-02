#include "psram_bus.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "psram_spi.pio.h"
static PIO bus;
static int machine=-1;
static unsigned program_offset;
int pico_psram_bus_init(void)
{
    if(machine>=0)return 1;
    bus=pio1;machine=pio_claim_unused_sm(bus,false);
    if(machine<0)return 0;
    if(!pio_can_add_program(bus,&picocalc_psram_program)){pio_sm_unclaim(bus,machine);machine=-1;return 0;}
    program_offset=pio_add_program(bus,&picocalc_psram_program);
    pio_sm_config c=picocalc_psram_program_get_default_config(program_offset);
    sm_config_set_out_pins(&c,2,1);sm_config_set_in_pins(&c,3);sm_config_set_sideset_pins(&c,20);
    sm_config_set_out_shift(&c,false,true,32);sm_config_set_in_shift(&c,false,true,32);
    // 60 MHz PIO: 26 instructions/byte; 13 bytes hold CS low < 6 us.
    sm_config_set_clkdiv(&c,(float)clock_get_hz(clk_sys)/60000000.0f);
    // Hold CS high while configuring the state machine and pin directions.
    gpio_init(20);gpio_set_dir(20,GPIO_OUT);gpio_put(20,1);
    gpio_init(21);gpio_set_dir(21,GPIO_OUT);gpio_put(21,0);
    for(unsigned pin=4;pin<=5;++pin){gpio_init(pin);gpio_set_dir(pin,false);gpio_pull_down(pin);}
    gpio_pull_down(3);
    pio_sm_init(bus,machine,program_offset+picocalc_psram_offset_idle,&c);
    pio_sm_set_consecutive_pindirs(bus,machine,2,1,true);pio_sm_set_consecutive_pindirs(bus,machine,3,1,false);pio_sm_set_consecutive_pindirs(bus,machine,20,2,true);
    pio_sm_exec(bus,machine,pio_encode_jmp(program_offset+picocalc_psram_offset_idle)|pio_encode_sideset(2,1));
    pio_gpio_init(bus,2);pio_gpio_init(bus,3);pio_gpio_init(bus,20);pio_gpio_init(bus,21);
    return 1;
}
int pico_psram_bus_transfer(const uint8_t *tx,uint8_t *rx,size_t length)
{
    if(machine<0||!tx||!rx||!length||length>13)return 0;
    pio_sm_set_enabled(bus,machine,false);pio_sm_clear_fifos(bus,machine);pio_sm_restart(bus,machine);
    pio_sm_exec(bus,machine,pio_encode_set(pio_x,length-1)|pio_encode_sideset(2,1));
    pio_sm_exec(bus,machine,pio_encode_jmp(program_offset)|pio_encode_sideset(2,1));
    for(size_t i=0;i<length;i+=4){uint32_t word=0;for(unsigned j=0;j<4&&i+j<length;++j)word|=(uint32_t)tx[i+j]<<(24-j*8);pio_sm_put(bus,machine,word);}
    uint32_t start=time_us_32();pio_sm_set_enabled(bus,machine,true);
    while(pio_sm_get_pc(bus,machine)!=program_offset+picocalc_psram_offset_idle)
    {
        if((uint32_t)(time_us_32()-start)>50)
        {
            pio_sm_set_enabled(bus,machine,false);
            gpio_init(20);gpio_set_dir(20,GPIO_OUT);gpio_put(20,1);gpio_init(21);gpio_set_dir(21,GPIO_OUT);gpio_put(21,0);
            pio_remove_program(bus,&picocalc_psram_program,program_offset);pio_sm_unclaim(bus,machine);machine=-1;return 0;
        }
    }
    pio_sm_set_enabled(bus,machine,false);
    for(size_t i=0;i<length;i+=4)
    {
        if(pio_sm_is_rx_fifo_empty(bus,machine))return 0;
        uint32_t word=pio_sm_get(bus,machine);size_t count=length-i;if(count>4)count=4;
        for(unsigned j=0;j<count;++j)rx[i+j]=word>>((count-1-j)*8);
    }
    // At least 50ns deselection time; the next CPU-side setup also leaves CS high.
    busy_wait_at_least_cycles(16);return 1;
}
