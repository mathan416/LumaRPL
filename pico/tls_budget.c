#include "tls_budget.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
// All TLS allocations remain inside the existing newlib 64 KiB bounded heap.
// Include allocation headers in the TLS sub-budget; newlib overhead is additional.
#define TLS_LIMIT (40u*1024u)
typedef union tls_header {max_align_t alignment;size_t bytes;} tls_header;
static size_t tls_used,tls_peak;
void *pico_tls_calloc(size_t count,size_t size)
{
    if(count&&size>SIZE_MAX/count)return NULL;size_t bytes=count*size;
    if(bytes>SIZE_MAX-sizeof(tls_header)||bytes+sizeof(tls_header)>TLS_LIMIT-tls_used)return NULL;
    tls_header *header=(tls_header *)malloc(sizeof(tls_header)+bytes);if(!header)return NULL;
    header->bytes=sizeof(tls_header)+bytes;tls_used+=header->bytes;if(tls_used>tls_peak)tls_peak=tls_used;
    void *result=header+1;memset(result,0,bytes);return result;
}
void pico_tls_free(void *pointer)
{
    if(!pointer)return;tls_header *header=(tls_header *)pointer-1;size_t bytes=header->bytes;
    volatile unsigned char *p=(volatile unsigned char *)(header+1);for(size_t i=sizeof(tls_header);i<bytes;++i)*p++=0;
    tls_used-=bytes;free(header);
}
int pico_tls_budget_reset(void){if(tls_used)return 0;tls_peak=0;return 1;}
size_t pico_tls_used(void){return tls_used;}
size_t pico_tls_peak(void){return tls_peak;}
