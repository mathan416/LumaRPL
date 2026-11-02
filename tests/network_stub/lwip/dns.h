#pragma once
#include "tcp.h"
err_t dns_gethostbyname(const char *,ip_addr_t *,void (*)(const char *,const ip_addr_t *,void *),void *);
