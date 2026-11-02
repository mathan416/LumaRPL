#pragma once
#include <stdbool.h>
#include <stdint.h>
typedef unsigned uint;typedef void *PIO;
#define pio1 ((PIO)1)
struct pio_program{const uint16_t *instructions;uint8_t length;int8_t origin;uint8_t pio_version;};
typedef struct{float divider;} pio_sm_config;
enum{pio_x=1};
int pio_claim_unused_sm(PIO,bool);void pio_sm_unclaim(PIO,uint);bool pio_can_add_program(PIO,const struct pio_program *);uint pio_add_program(PIO,const struct pio_program *);void pio_remove_program(PIO,const struct pio_program *,uint);
pio_sm_config pio_get_default_sm_config(void);void sm_config_set_wrap(pio_sm_config *,uint,uint);void sm_config_set_sideset(pio_sm_config *,uint,bool,bool);void sm_config_set_out_pins(pio_sm_config *,uint,uint);void sm_config_set_in_pins(pio_sm_config *,uint);void sm_config_set_sideset_pins(pio_sm_config *,uint);void sm_config_set_out_shift(pio_sm_config *,bool,bool,uint);void sm_config_set_in_shift(pio_sm_config *,bool,bool,uint);void sm_config_set_clkdiv(pio_sm_config *,float);
void pio_gpio_init(PIO,uint);void pio_sm_set_consecutive_pindirs(PIO,uint,uint,uint,bool);void pio_sm_init(PIO,uint,uint,const pio_sm_config *);void pio_sm_exec(PIO,uint,uint);void pio_sm_set_enabled(PIO,uint,bool);void pio_sm_clear_fifos(PIO,uint);void pio_sm_restart(PIO,uint);void pio_sm_put(PIO,uint,uint32_t);uint pio_sm_get_pc(PIO,uint);bool pio_sm_is_rx_fifo_empty(PIO,uint);uint32_t pio_sm_get(PIO,uint);
static inline uint pio_encode_set(uint dest,uint value){return 0xe000|(dest<<5)|value;}static inline uint pio_encode_jmp(uint address){return address;}static inline uint pio_encode_sideset(uint count,uint value){return value<<(13-count);}
