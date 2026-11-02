#include <cassert>
#include <cstring>
#include <cstdio>
#include "input.h"
#include "target.h"
const pattern pattern::black=pattern(0,0,0);
const pattern pattern::white=pattern(255,255,255);
const pattern pattern::invert=pattern(~0ULL);
using p8=blitter::pattern<blitter::RGB_8BPP>;
const p8 p8::black(0,0,0),p8::white(255,255,255),p8::invert(~0ULL);
static void expect(const uint8_t *frame,int x,int y,int r,int g,int b)
{ uint8_t pixel[3];pico_rgb_pixel(frame,x,y,pixel);assert(pixel[0]==r && pixel[1]==g && pixel[2]==b); }
template<blitter::mode Mode>void clipped_copies(surface &screen,blitter::surface<Mode> &source)
{
    source.clip(blitter::rect(1,1,source.width()-2,source.height()-2));
    const int positions[]={-4096,-5,0,8,315,4096};
    const int offsets[]={-4096,-3,0,4,29,4096,INT32_MIN,INT32_MAX};
    for(int left:positions)for(int sx:offsets)
    {
        screen.fill(p8(blitter::color<blitter::RGB_8BPP>(pixword(0x55))));
        blitter::rect dest(left,-2,left+source.width()-1,source.height()-3);
        screen.copy(source,dest,blitter::point(sx,-1));
        for(int y=0;y<20;++y)for(int x=0;x<320;++x)
        {
            int64_t xx=int64_t(sx)+(source.horizontal_swap()?int64_t(x)-dest.x1:int64_t(dest.x2)-x);
            int yy=-1+y-dest.y1;uint8_t expected=0x55;
            if(x>=dest.x1&&x<=dest.x2&&y>=dest.y1&&y<=dest.y2&&xx>=1&&xx<=source.width()-2&&yy>=1&&yy<=int(source.height())-2)
            {
                auto c=source.pixel_color(int(xx),yy);unsigned r=c.red(),g=c.green(),b=c.blue();
                if constexpr(Mode==blitter::MONOCHROME){r=255-r;g=255-g;b=255-b;}
                expected=blitter::color<blitter::RGB_8BPP>(r,g,b).value;
            }
            assert(screen.pixel_color(x,y).value==expected);
        }
    }
}
int main()
{
    alignas(8) uint8_t guarded[320*320+16];memset(guarded,0xa5,sizeof guarded);
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
    for(int n=0;n<8;++n) assert(guarded[n]==0xa5 && guarded[102408+n]==0xa5);
    // Untyped settings values must retain their RGB565 serialization meaning.
    pattern legacy(224,240,224);screen.fill(0,0,319,319,legacy.bits);
    blitter::color<blitter::RGB_16BPP> clegacy((pixword)legacy.bits);
    blitter::color<blitter::RGB_8BPP> qlegacy(clegacy.red(),clegacy.green(),clegacy.blue());
    for(int y=0;y<320;++y)for(int x=0;x<320;++x)assert(screen.pixel_color(x,y).value==qlegacy.value);
    // Round-trip existing RGB565 pixmaps through the new screen surface.
    alignas(8) pixword pixdata[32*20/2]={};
    blitter::surface<blitter::RGB_16BPP> pix(pixdata,32,20,32,32);
    for(int y=0;y<20;++y)for(int x=0;x<32;++x)pix.fill(x,y,x,y,pattern(uint8_t(x*8),uint8_t(y*12),uint8_t((x+y)*5)));
    screen.copy(pix,40,50);
    for(int y=0;y<20;++y)for(int x=0;x<32;++x){auto c=pix.pixel_color(x,y);blitter::color<blitter::RGB_8BPP> expected(c.red(),c.green(),c.blue());assert(screen.pixel_color(40+x,50+y).value==expected.value);}
    pix.copy(screen,blitter::rect(0,0,31,19),blitter::point(40,50));
    for(int y=0;y<20;++y)for(int x=0;x<32;++x){auto c=screen.pixel_color(40+x,50+y);blitter::color<blitter::RGB_16BPP> expected(c.red(),c.green(),c.blue());assert(pix.pixel_color(x,y).value==expected.value);}
    // Monochrome font masks expand correctly at unaligned pixel boundaries.
    alignas(8) pixword mono_data[20];for(unsigned i=0;i<20;++i)mono_data[i]=0x35a9f142u+i;
    blitter::surface<blitter::MONOCHROME_REVERSE> mono(mono_data,29,20,32,29);
    screen.copy(mono,43,81);
    for(int y=0;y<20;++y)for(int x=0;x<29;++x)assert(screen.pixel_color(43+x,81+y).value==mono.pixel_color(x,y).red());
    // Unclipped glyph sources include the prefix introduced by aligning a
    // byte bitmap back to a machine word, outside its nominal drawable width.
    blitter::surface<blitter::MONOCHROME> glyph(mono_data,9,3,24,9);
    screen.copy<blitter::CLIP_DST>(glyph,blitter::rect(70,90,78,92),blitter::point(16,0));
    for(int y=0;y<3;++y)for(int x=0;x<9;++x){unsigned bit=24-x+y*24;assert(screen.pixel_color(70+x,90+y).value==((mono_data[bit/32]>>(bit%32))&1?0:255));}
    // Compare the exact upstream RGB565 glyph raster, including left/right
    // clipping, then quantize each reference pixel to the new palette.
    alignas(8) pixword reference_data[320*320/2+1]={};
    blitter::surface<blitter::RGB_16BPP> reference(reference_data,320,320,320,320);
    for(int left=-4;left<330;left+=19)
    {
        reference.fill(pattern::white);screen.fill(pattern::white);
        blitter::rect dest(left,20,left+8,22);blitter::point src(16,0);
        blitter::blit<blitter::CLIP_DST>(reference,glyph,dest,src,blitter::blitop_draw,pattern(35,130,240));
        blitter::blit<blitter::CLIP_DST>(screen,glyph,dest,src,blitter::blitop_draw,p8(pattern(35,130,240)));
        for(int y=20;y<23;++y)for(int x=0;x<320;++x){auto c=reference.pixel_color(x,y);blitter::color<blitter::RGB_8BPP> q(c.red(),c.green(),c.blue());assert(screen.pixel_color(x,y).value==q.value);}
    }
    clipped_copies(screen,pix);clipped_copies(screen,mono);clipped_copies(screen,glyph);
    // Overlapping scroll/copy and XOR remain bit-exact for indexed pixels.
    uint8_t before[320*20];for(int y=0;y<20;++y)for(int x=0;x<320;++x){screen.fill(x,y,x,y,p8(blitter::color<blitter::RGB_8BPP>(pixword(x+y))));before[y*320+x]=screen.pixel_color(x,y).value;}
    screen.copy(screen,blitter::rect(3,2,319,19),blitter::point(0,0));
    for(int y=2;y<20;++y)for(int x=3;x<320;++x)assert(screen.pixel_color(x,y).value==before[(y-2)*320+x-3]);
    screen.invert(3,2,319,19);for(int y=2;y<20;++y)for(int x=3;x<320;++x)assert(screen.pixel_color(x,y).value==uint8_t(~before[(y-2)*320+x-3]));
    screen.invert(3,2,319,19);
    // All 256 palette entries survive the LCD transfer conversion.
    for(unsigned q=0;q<256;++q){blitter::color<blitter::RGB_8BPP> c((pixword)q);screen.fill(q,210,q,210,p8(c));expect(frame,q,210,c.red(),c.green(),c.blue());}
    for(int n=0;n<8;++n)assert(guarded[n]==0xa5&&guarded[102408+n]==0xa5);
    assert(pico_translate_key(20)==PICO_SETUP);
    assert(pico_translate_key(16)==PICO_PROGRAMS);
    assert(pico_translate_key(7)==PICO_GRAPH);
    puts("PASS: 256 palette entries, RGB565 pixmap roundtrip, mono font masks, overlapping scroll, XOR, clipping, orientation, transfer and guards");
}
