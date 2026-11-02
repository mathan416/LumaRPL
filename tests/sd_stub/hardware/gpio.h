#pragma once
#define GPIO_FUNC_SPI 1
#define GPIO_OUT 1
static inline void gpio_set_function(int a,int b){(void)a;(void)b;}
static inline void gpio_set_pulls(int a,int b,int c){(void)a;(void)b;(void)c;}
static inline void gpio_init(int a){(void)a;}
static inline void gpio_put(int a,int b){(void)a;(void)b;}
static inline void gpio_set_dir(int a,int b){(void)a;(void)b;}
