#pragma once
#include <stdint.h>
#include <stddef.h>
struct cyw43_t {int unused;};extern cyw43_t cyw43_state;
#define CYW43_AUTH_WPA2_MIXED_PSK 1
#define CYW43_AUTH_WPA2_AES_PSK 0x00400004
#define CYW43_AUTH_OPEN 0
#define CYW43_ITF_STA 0
#define CYW43_LINK_DOWN 0
#define CYW43_LINK_JOIN 1
#define CYW43_LINK_NOIP 2
#define CYW43_LINK_UP 3
#define CYW43_LINK_FAIL -1
#define CYW43_LINK_NONET -2
#define CYW43_LINK_BADAUTH -3
uint64_t get_absolute_time();uint32_t to_ms_since_boot(uint64_t);void sleep_ms(unsigned);
int cyw43_arch_init();void cyw43_arch_deinit();void cyw43_arch_enable_sta_mode();int cyw43_arch_wifi_connect_async(const char *,const char *,uint32_t);int cyw43_tcpip_link_status(cyw43_t *,int);void cyw43_arch_poll();
