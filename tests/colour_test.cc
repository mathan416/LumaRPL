#include <cassert>
#include <cstring>
#include <cstdio>
#include "input.h"
#include "target.h"
const pattern pattern::black=pattern(0,0,0);
const pattern pattern::white=pattern(255,255,255);
const pattern pattern::invert=pattern(~0ULL);
static void expect(const uint8_t *frame,int x,int y,int r,int g,int b)
{ uint8_t pixel[3];pico_rgb_pixel(frame,x,y,pixel);assert(pixel[0]==r && pixel[1]==g && pixel[2]==b); }
int main()
{
    alignas(8) uint8_t guarded[320*320*2+16];memset(guarded,0xa5,sizeof guarded);
    uint8_t *frame=guarded+8;surface screen(reinterpret_cast<pixword *>(frame),320,320,320,320);
    screen.fill(pattern::white);
    for(int y=0;y<320;++y) for(int x=0;x<320;++x) expect(frame,x,y,255,255,255);
    screen.fill(0,0,0,0,pattern(255,0,0));
    screen.fill(319,0,319,0,pattern(0,255,0));
    screen.fill(0,319,0,319,pattern(0,0,255));
    screen.fill(319,319,319,319,pattern::black);
    expect(frame,0,0,255,0,0);expect(frame,319,0,0,255,0);
    expect(frame,0,319,0,0,255);expect(frame,319,319,0,0,0);
    screen.fill(-4,-4,4,4,pattern(255,0,0));screen.fill(315,315,340,340,pattern(0,0,255));
    for(int n=0;n<8;++n) assert(guarded[n]==0xa5 && guarded[204808+n]==0xa5);
    assert(pico_translate_key(20)==PICO_SETUP);
    assert(pico_translate_key(16)==PICO_PROGRAMS);
    assert(pico_translate_key(7)==PICO_GRAPH);
    puts("PASS: RGB565 primary colours, all corners, orientation, clipping and new shortcuts");
}
