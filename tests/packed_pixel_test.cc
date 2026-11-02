#include <cstdlib>

#include <cassert>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include "input.h"
#include "target.h"
const pattern pattern::black=pattern(0,0,0),pattern::white=pattern(255,255,255),pattern::invert=pattern(~0ULL);
using p8=blitter::pattern<blitter::RGB_8BPP>;
const p8 p8::black(0,0,0),p8::white(255,255,255),p8::invert(~0ULL);
using p0=blitter::pattern<blitter::MONOCHROME>;const p0 p0::black(0,0,0),p0::white(255,255,255),p0::invert(~0ULL);
using p1=blitter::pattern<blitter::MONOCHROME_REVERSE>;const p1 p1::black(0,0,0),p1::white(255,255,255),p1::invert(~0ULL);
using p4=blitter::pattern<blitter::GRAY_4BPP>;const p4 p4::black(0,0,0),p4::white(255,255,255),p4::invert(~0ULL);
template<blitter::mode Mode>void packed()
{
 constexpr unsigned bits=blitter::color<Mode>::BPP;
 for(unsigned offset=0;offset<4;++offset)for(unsigned width:{1,3,7,13,31})for(unsigned height:{1,3}){
  size_t size=(width*height*bits+7)/8;auto *allocation=new uint8_t[offset+size];auto *data=allocation+offset;for(size_t i=0;i<size;++i)data[i]=uint8_t(0xa5+37*i);
  blitter::surface<Mode> source(reinterpret_cast<blitter::pixword *>(data),width,height,width,width);
  alignas(8) blitter::pixword pixels[64];surface screen(pixels,16,16,16,16);
  for(int left:{-3,0,7,14}){
   screen.fill(p8::white);screen.copy(source,blitter::rect(left,2,left+width-1,2+height-1),blitter::point(0,0));
   for(int y=0;y<16;++y)for(int x=0;x<16;++x){unsigned expected=255;
    if(x>=left&&x<left+int(width)&&y>=2&&y<2+int(height)){
     size_t at=((width-1-(x-left))+(y-2)*width)*bits;unsigned raw=0;for(unsigned bit=0;bit<bits;++bit)raw|=((data[(at+bit)/8]>>((at+bit)%8))&1)<<bit;
     blitter::color<Mode> colour(raw);expected=blitter::color<blitter::RGB_8BPP>(colour.red(),colour.green(),colour.blue()).value;if constexpr(Mode==blitter::MONOCHROME)expected^=255;
    }assert(screen.pixel_color(x,y).value==expected);
   }
  }delete[] allocation;
 }
}
// Compare byte-packed source/destination operations against the existing
// word-aligned raster path, without using unaligned pixel_color reads.
template<blitter::mode Src,blitter::mode Dst>void raster_equivalence()
{
 if constexpr(Src!=Dst && Src!=blitter::MONOCHROME && Src!=blitter::MONOCHROME_REVERSE && Src!=blitter::RGB_8BPP && Dst!=blitter::RGB_8BPP)return;
 constexpr unsigned sb=blitter::color<Src>::BPP,db=blitter::color<Dst>::BPP,w=9,h=3;
 constexpr unsigned ss=((w*sb+31)/32)*32/sb,ds=((w*db+31)/32)*32/db;
 auto read=[](const uint8_t *p,size_t at,unsigned bits){unsigned value=0;for(unsigned b=0;b<bits;++b)value|=((p[(at+b)/8]>>((at+b)%8))&1)<<b;return value;};
 auto write=[](uint8_t *p,size_t at,unsigned bits,unsigned value){for(unsigned b=0;b<bits;++b){unsigned mask=1u<<((at+b)%8);p[(at+b)/8]=(p[(at+b)/8]&~mask)|(((value>>b)&1)?mask:0);}};
 for(unsigned source_offset=0;source_offset<4;++source_offset)for(unsigned dest_offset=0;dest_offset<4;++dest_offset){
  auto *sa=new uint8_t[source_offset+(w*h*sb+7)/8];auto *da=new uint8_t[dest_offset+(w*h*db+7)/8];auto *sp=sa+source_offset,*dp=da+dest_offset;
  blitter::pixword ref_src[(ss*h*sb+31)/32+1]{},ref_dst[(ds*h*db+31)/32+1]{};
  for(unsigned y=0;y<h;++y)for(unsigned x=0;x<w;++x){unsigned value=(x+13*y)*7919;write(sp,(x+y*w)*sb,sb,value);write(reinterpret_cast<uint8_t *>(ref_src),(x+y*ss)*sb,sb,value);}
  blitter::surface<Src> source(reinterpret_cast<blitter::pixword *>(sp),w,h,w,w),reference_source(ref_src,w,h,ss,w);
  blitter::surface<Dst> dest(reinterpret_cast<blitter::pixword *>(dp),w,h,w,w),reference(ref_dst,w,h,ds,w);
  for(auto op:{blitter::blitop_source,blitter::blitop_draw,blitter::blitop_xor}){
   memset(dp,0xa5,(w*h*db+7)/8);memset(ref_dst,0xa5,sizeof ref_dst);
   for(unsigned y=0;y<h;++y)for(unsigned x=0;x<w;++x){unsigned value=(x+7*y)*1237;write(dp,(x+y*w)*db,db,value);write(reinterpret_cast<uint8_t *>(ref_dst),(x+y*ds)*db,db,value);}
   blitter::pattern<Dst> ink(37,160,210);blitter::rect area(0,0,w-1,h-1);blitter::point point(0,0);
   blitter::blit<blitter::CLIP_ALL>(dest,source,area,point,op,ink);blitter::blit<blitter::CLIP_ALL>(reference,reference_source,area,point,op,ink);
   for(unsigned y=0;y<h;++y)for(unsigned x=0;x<w;++x)if(read(dp,(x+y*w)*db,db)!=read(reinterpret_cast<uint8_t *>(ref_dst),(x+y*ds)*db,db)){fprintf(stderr,"Raster mismatch src=%u dst=%u offsets=%u,%u op=%u pixel=%u,%u actual=%u ref=%u\n",Src,Dst,source_offset,dest_offset,op==blitter::blitop_source?0:op==blitter::blitop_draw?1:2,x,y,read(dp,(x+y*w)*db,db),read(reinterpret_cast<uint8_t *>(ref_dst),(x+y*ds)*db,db));std::abort();}
  }delete[] sa;delete[] da;
 }
}
template<blitter::mode S>void destinations(){raster_equivalence<S,blitter::MONOCHROME>();raster_equivalence<S,blitter::MONOCHROME_REVERSE>();raster_equivalence<S,blitter::GRAY_4BPP>();raster_equivalence<S,blitter::RGB_8BPP>();raster_equivalence<S,blitter::RGB_16BPP>();}
template<blitter::mode Mode>void aligned_ends()
{
 constexpr unsigned bits=blitter::color<Mode>::BPP,width=32/bits;
 for(unsigned words:{1,2,3}){
  auto *a=new blitter::pixword[words],*b=new blitter::pixword[words];for(unsigned i=0;i<words;++i){a[i]=0xaabbccdd+i;b[i]=0;}
  blitter::surface<Mode> source(a,width*words,1),dest(b,width*words,1);
  blitter::blit<blitter::COPY>(dest,source,blitter::rect(0,0,width*words-1,0),blitter::point(0,0),blitter::blitop_source,blitter::pattern<Mode>(0,0,0));
  assert(!memcmp(a,b,words*4));
  if(words==1){unsigned before=a[0];blitter::blit<blitter::clipping(blitter::COPY|blitter::OVERLAP)>(source,source,blitter::rect(1,0,width-1,0),blitter::point(0,0),blitter::blitop_source,blitter::pattern<Mode>(0,0,0));unsigned expected;if(Mode==blitter::RGB_16BPP||Mode==blitter::MONOCHROME_REVERSE)expected=(before>>bits)|(before&(~0u<<(32-bits)));else expected=(before<<bits)|(before&((1u<<bits)-1));if(a[0]!=expected){fprintf(stderr,"Overlap mode %u: %08x != %08x\n",Mode,a[0],expected);std::abort();}}
  delete[] a;delete[] b;
 }
}
int main(){aligned_ends<blitter::MONOCHROME>();aligned_ends<blitter::MONOCHROME_REVERSE>();aligned_ends<blitter::GRAY_4BPP>();aligned_ends<blitter::RGB_16BPP>();destinations<blitter::MONOCHROME>();destinations<blitter::MONOCHROME_REVERSE>();destinations<blitter::GRAY_4BPP>();destinations<blitter::RGB_8BPP>();destinations<blitter::RGB_16BPP>();packed<blitter::MONOCHROME>();packed<blitter::MONOCHROME_REVERSE>();packed<blitter::GRAY_4BPP>();packed<blitter::RGB_8BPP>();packed<blitter::RGB_16BPP>();puts("PASS: exact-length packed sources at offsets 0-3, all five pixel modes, partial bytes/rows and left/right clipping: byte-exact pixels without alignment or tail overreads");}
