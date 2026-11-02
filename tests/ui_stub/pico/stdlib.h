#pragma once
#include <stdint.h>
static inline uint64_t get_absolute_time(){static uint64_t ticks=1000000;return ticks+=1000;}
static inline uint64_t to_us_since_boot(uint64_t t){return t;}
static inline uint32_t to_ms_since_boot(uint64_t t){return t/1000;}
static inline void sleep_ms(unsigned){}
