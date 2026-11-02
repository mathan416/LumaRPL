#pragma once
#include <stdint.h>
typedef struct {uint32_t start,limit;} pico_sd_deadline;
static inline int pico_sd_expired(pico_sd_deadline deadline,uint32_t now)
{return (uint32_t)(now-deadline.start)>=deadline.limit;}
