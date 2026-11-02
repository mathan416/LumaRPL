#pragma once
#include <stddef.h>
#include <stdint.h>
int pico_psram_bus_init(void);
// Full-duplex SPI. 1..13 bytes, preloaded TX and bounded completion wait.
int pico_psram_bus_transfer(const uint8_t *tx,uint8_t *rx,size_t length);
