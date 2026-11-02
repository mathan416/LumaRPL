#pragma once
#include <stddef.h>
#include <stdint.h>
typedef int err_t;
#define ERR_OK 0
#define ERR_MEM -1
#define ERR_INPROGRESS -5
#define IPADDR_TYPE_V4 0
#define TCP_WRITE_FLAG_COPY 1
struct ip_addr_t {uint32_t address;};
struct pbuf {uint16_t tot_len;unsigned char *data;bool owned=false;};
struct tcp_pcb;
typedef err_t (*recv_fn)(void *,tcp_pcb *,pbuf *,err_t);typedef void (*error_fn)(void *,err_t);
struct tcp_pcb {void *context;recv_fn receive;error_fn error;int fd;};
tcp_pcb *tcp_new_ip_type(int);void tcp_arg(tcp_pcb *,void *);void tcp_recv(tcp_pcb *,recv_fn);void tcp_err(tcp_pcb *,error_fn);void tcp_abort(tcp_pcb *);void tcp_recved(tcp_pcb *,size_t);size_t tcp_sndbuf(tcp_pcb *);err_t tcp_write(tcp_pcb *,const void *,size_t,int);err_t tcp_output(tcp_pcb *);err_t tcp_connect(tcp_pcb *,const ip_addr_t *,unsigned,err_t (*)(void *,tcp_pcb *,err_t));
size_t pbuf_copy_partial(pbuf *,void *,size_t,size_t);void pbuf_free(pbuf *);
