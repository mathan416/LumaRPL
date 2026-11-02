#include "plot_engine.h"
#include <cassert>
#include <cstdio>
#include <cfloat>
#include <initializer_list>
struct source {int calls=0,stop=0,error=0;};
int sample(void *opaque,int curve,double x,double y,double *z)
{
    auto &s=*static_cast<source *>(opaque);++s.calls;
    if(s.stop&&s.calls==s.stop)return s.error;
    *z=curve==0?x*x-y*y:curve==1?x:NAN;return 1;
}
bool cancel(void *opaque,int){return ++*static_cast<int *>(opaque)==17;}
int main()
{
    pico_plot_request r{};double window[]={-3,3,-2,4},bounds[]={-3,3,-3,3};
    for(int i=0;i<4;++i){r.window[i]=window[i];r.bounds3d[i]=bounds[i];}
    r.active[0]=r.active[1]=r.active[2]=1;
    struct {double before=123,values[900],after=456;} output;source s;
    assert(pico_plot_engine::sample_curves(r,sample,&s,output.values)==1&&s.calls==900);
    assert(output.before==123&&output.after==456);
    for(int i=0;i<300;++i)
    {
        double x=pico_plot_engine::coordinate(-3,3,i,299);
        assert(output.values[i]==x*x&&output.values[300+i]==x&&std::isnan(output.values[600+i]));
    }
    for(int error:{-1,-2}){s={0,12,error};assert(pico_plot_engine::sample_curves(r,sample,&s,output.values)==error&&s.calls==12);}
    int polls=0;s={};assert(pico_plot_engine::sample_curves(r,sample,&s,output.values,cancel,&polls)==-2&&s.calls==16);
    double lo=0,hi=0;int count=0;s={};
    assert(pico_plot_engine::sample_surface(r,sample,&s,output.values,lo,hi,count)==1&&count==289&&s.calls==289&&lo==-9&&hi==9);
    for(int j=0;j<17;++j)for(int i=0;i<17;++i)
    {double x=-3+i*0.375,y=-3+j*0.375;assert(std::fabs(output.values[j*17+i]-(x*x-y*y)/9)<1e-6);}
    s={0,20,-1};assert(pico_plot_engine::sample_surface(r,sample,&s,output.values,lo,hi,count)==-1);
    r.window[1]=r.window[0];s={};assert(pico_plot_engine::sample_curves(r,sample,&s,output.values)==-1&&!s.calls);
    r.bounds3d[0]=NAN;assert(pico_plot_engine::sample_surface(r,sample,&s,output.values,lo,hi,count)==-1&&!s.calls);
    assert(pico_plot_engine::coordinate(-1e100,1e100,150,300)==0);
    puts("PASS: shared native 2D/3D sampling, all curves, domain gaps, bounds, interruption, callback errors and buffer guards");
}
