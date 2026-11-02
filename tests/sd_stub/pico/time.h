#pragma once
#include <stdint.h>
extern uint64_t fake_us;
static inline uint64_t get_absolute_time(void){return fake_us;}
static inline uint32_t to_ms_since_boot(uint64_t t){return (uint32_t)(t/1000);}
static inline void sleep_us(uint32_t n){fake_us+=n;}
