#pragma once
#include <stdint.h>
#include <stddef.h>
#define spi0 ((void*)1)
unsigned spi_init(void *,unsigned);
int spi_write_blocking(void *,const uint8_t *,size_t);
int spi_read_blocking(void *,uint8_t,uint8_t *,size_t);
