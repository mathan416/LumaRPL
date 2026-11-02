#pragma once
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
// Explicit serial storage, never a CPU-addressable malloc arena.
int pico_psram_init(void);
int pico_psram_available(void);
int pico_psram_read(uint32_t offset,void *data,size_t length);
int pico_psram_write(uint32_t offset,const void *data,size_t length);
uint32_t pico_psram_capacity(void);
#ifdef __cplusplus
}
#endif
