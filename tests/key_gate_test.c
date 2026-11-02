#include "key_gate.h"
#include <assert.h>
#include <stdio.h>
int main(void)
{
    const int keys[]={'1','A','R',13,0x81,0x85,0xb5};
    for(unsigned i=0;i<sizeof keys/sizeof *keys;++i)
    {
        pico_key_gate g={-1,-1};int key=keys[i];
        assert(pico_key_gate_event(&g,key,1));
        pico_key_gate_transition(&g);
        assert(!pico_key_gate_event(&g,key,2));
        assert(!pico_key_gate_event(&g,key,1));
        assert(pico_key_gate_event(&g,'x',1));
        assert(!pico_key_gate_event(&g,key,3));
        assert(pico_key_gate_event(&g,key,1));
    }
    puts("PASS: screen transitions reject held digits, Ctrl shortcut letters, Enter, F1/F5 and arrows until release; other keys remain available");
}
