#pragma once
#ifdef __cplusplus
extern "C" {
#endif
void gpio_init(unsigned);void gpio_set_dir(unsigned,int);void gpio_pull_down(unsigned);
void gpio_put(unsigned,int);int gpio_get(unsigned);void busy_wait_at_least_cycles(unsigned);void sleep_us(unsigned);
#ifdef __cplusplus
}
#endif
