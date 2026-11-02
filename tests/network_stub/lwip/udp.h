#pragma once
#include "tcp.h"
#define PBUF_TRANSPORT 0
#define PBUF_RAM 0
#define ip_addr_cmp(a,b) ((a)->address==(b)->address)
struct udp_pcb;
typedef void (*udp_recv_fn)(void *,udp_pcb *,pbuf *,const ip_addr_t *,uint16_t);
struct udp_pcb {void *context;udp_recv_fn receive;ip_addr_t peer;uint16_t port;};
udp_pcb *udp_new_ip_type(int);void udp_recv(udp_pcb *,udp_recv_fn,void *);void udp_remove(udp_pcb *);err_t udp_connect(udp_pcb *,const ip_addr_t *,uint16_t);err_t udp_send(udp_pcb *,pbuf *);
pbuf *pbuf_alloc(int,uint16_t,int);err_t pbuf_take(pbuf *,const void *,size_t);
