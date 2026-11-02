#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#ifndef PICOCALC_HEAP_BYTES
#define PICOCALC_HEAP_BYTES 98304
#endif
// Keep newlib allocation within the dedicated 96 KiB heap, below the stack.
static ptrdiff_t used,peak;
size_t pico_heap_committed(void){return used;}
size_t pico_heap_committed_peak(void){return peak;}
void *_sbrk(int increment)
{
    extern char end;

    if ((increment > 0 && increment > PICOCALC_HEAP_BYTES-used) ||
        (increment < 0 && increment < -used))
    {
        errno=ENOMEM;
        return (void *)-1;
    }
    char *previous=&end+used;
    used+=increment;if(used>peak)peak=used;
    return previous;
}
