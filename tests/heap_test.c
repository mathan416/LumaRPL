#include <assert.h>
#include <errno.h>
#include <stdio.h>
char end[98304] = {0};
void *_sbrk(int);
int main(void)
{
    assert(_sbrk(0)==end);
    assert(_sbrk(-1)==(void *)-1 && errno==ENOMEM);
    assert(_sbrk(98304)==end);
    assert(_sbrk(1)==(void *)-1 && errno==ENOMEM);
    assert(_sbrk(-98304)==end+98304);
    assert(_sbrk(49152)==end);
    assert(_sbrk(49152)==end+49152);
    assert(_sbrk(4)==(void *)-1);
    puts("PASS: bounded 96 KiB heap, exhaustion and underflow");
}
