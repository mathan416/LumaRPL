#include <cassert>
#include <cstdio>
#include <cstring>
#include "wireframe.h"
#include "target.h"
const pattern pattern::black=pattern(0,0,0);
const pattern pattern::white=pattern(255,255,255);
const pattern pattern::invert=pattern(~0ULL);
#include "rgb332_patterns.h"
int main()
{
    using namespace pico_wireframe;
    assert(normalize(-10,-10,10)==-1 && normalize(10,-10,10)==1);
    assert(normalize(7,7,7)==0 && normalize(0,-1e308,1e308)==0);
    assert(normalize(1e-308,0,2e-308)==0);
    alignas(8) unsigned char data[320*320*BITS_PER_PIXEL/8+16];memset(data,0xa5,sizeof data);
    surface screen(reinterpret_cast<pixword *>(data+8),320,320,320,320);
    camera c;
    for(int turn=0;turn<100;++turn)
    {
        c.rotate(0.2f,turn%2?0.3f:-0.3f);c.magnify(turn<50?1.15f:1/1.15f);
        assert(c.zoom>=0.4f && c.zoom<=2.5f);
        for(int y=0;y<17;++y) for(int x=0;x<16;++x)
        {
            float u=-1+x/8.f,v=-1+y/8.f;
            auto a=c.project(u,v,(u*u-v*v));
            auto b=c.project(u+.125f,v,((u+.125f)*(u+.125f)-v*v));
            assert(std::isfinite(a.x) && std::isfinite(a.y));
            screen.line(int(a.x),int(a.y),int(b.x),int(b.y),1,pattern(30,170,230));
        }
    }
    for(int i=0;i<8;++i) assert(data[i]==0xa5 && data[320*320*BITS_PER_PIXEL/8+8+i]==0xa5);
    puts("PASS: 3D projection, constant/extreme/subnormal normalization, bounded rotation/zoom and actual blitter clipping");
}
