#pragma once
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {size_t heap_allocated,heap_committed,heap_committed_peak,stack_high_water;int device;} pico_memory_observation;
#ifdef PICO_UI_HOST
static inline pico_memory_observation pico_memory_observe(void){return (pico_memory_observation){0,0,0,0,0};}
#else
pico_memory_observation pico_memory_observe(void);
void pico_stack_watermark_init(void);
#endif
#ifdef __cplusplus
}
#endif
