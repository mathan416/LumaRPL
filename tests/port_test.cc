#include <cassert>
#include <cstring>
#include <cstdio>
#include "input.h"
#include "target.h"

const pattern pattern::black=pattern(0,0,0);
const pattern pattern::white=pattern(255,255,255);
const pattern pattern::invert=pattern(~0ULL);
int main()
{
    assert(pico_translate_key(0x81)==KB_F1);
    assert(pico_translate_key(0x85)==KB_F5);
    assert(pico_translate_key(0x86)==KB_F6);
    assert(pico_translate_key(10)==KB_ENT);
    assert(pico_translate_key(0xb1)==KB_ESC);
    assert(pico_translate_key(0x91)==KB_ESC);
    assert(pico_translate_key(8)==KB_BKS);
    assert(pico_translate_key('a')==PICO_TEXT+'a');
    assert(pico_translate_key(1)==PICO_CATALOG);
    assert(pico_translate_key(18)==PICO_RPL_SETTINGS);
    assert(pico_translate_key(19)==PICO_SAVE);
    assert(pico_translate_key(23)==PICO_WORKBENCH);
    assert(pico_translate_key(4)==PICO_DATA);
    assert(pico_translate_key(6)==PICO_FORMS);
    assert(pico_translate_key(14)==PICO_NOTEBOOK);
    assert(pico_translate_key(12)==PICO_LOAD);
    assert(pico_translate_key(17)==PICO_EXIT);
    assert(pico_translate_key(0xb4)==PICO_LEFT);
    assert(pico_translate_key(0xb7)==PICO_RIGHT);
    assert(pico_translate_key(2)==PICO_TEXT+0xab);
    assert(pico_translate_key(5)==PICO_TEXT+0xbb);
    assert(pico_translate_key(0xff)==-1);
    alignas(8) uint8_t guarded[12800+16];
    memset(guarded,0xa5,sizeof guarded);
    uint8_t *frame=guarded+8;
    surface screen(reinterpret_cast<pixword *>(frame),320,320,320,320);
    screen.fill(pattern::white);
    for(int y=0;y<320;y++) for(int x=0;x<320;x++)
        assert(pico_mono_pixel(frame,x,y)==255);
    screen.fill(0,0,0,0,pattern::black);
    screen.fill(319,319,319,319,pattern::black);
    assert(pico_mono_pixel(frame,0,0)==0);
    assert(pico_mono_pixel(frame,319,319)==0);
    assert(pico_mono_pixel(frame,319,0)==255);
    assert(pico_mono_pixel(frame,0,319)==255);
    screen.fill(-10,-10,5,5,pattern::black);
    screen.fill(315,315,340,340,pattern::black);
    for(int n=0;n<8;n++) assert(guarded[n]==0xa5 && guarded[12808+n]==0xa5);
    puts("PASS: keyboard mapping, framebuffer orientation, pixels and clipping");
}
