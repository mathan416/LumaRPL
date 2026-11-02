#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void *pico_tls_calloc(size_t,size_t);
void pico_tls_free(void *);
int pico_tls_budget_reset(void);
size_t pico_tls_used(void);
size_t pico_tls_peak(void);
#ifdef __cplusplus
}
#endif
