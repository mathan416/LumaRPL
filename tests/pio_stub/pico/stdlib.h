#pragma once
#include <stdbool.h>
#include <stdint.h>
#define GPIO_OUT true
void gpio_init(unsigned);void gpio_set_dir(unsigned,bool);void gpio_put(unsigned,bool);void gpio_pull_down(unsigned);void busy_wait_at_least_cycles(unsigned);uint32_t time_us_32(void);
