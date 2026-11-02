#include "path_scene.h"
#include "plot_engine.h"
#include "data_curve.h"
#include <cassert>
#include <cstdio>
struct context {int count=0,fail=0,code=-1;};
int circle(void *opaque,double t,double *x,double *y)
{auto &c=*static_cast<context *>(opaque);if(++c.count==c.fail)return c.code;*x=std::cos(t);*y=std::sin(t);return 1;}
int gap(void *,double t,double *x,double *y){*x=t;*y=1;return t<0?0:1;}
bool stop(void *,int){return true;}
int main()
{
    double slope=0;assert(pico_path_scene::direction(0,1,slope)==pico_path_scene::tangent::vertical);
    assert(pico_path_scene::direction(0,0,slope)==pico_path_scene::tangent::unavailable);
    assert(pico_path_scene::direction(2,4,slope)==pico_path_scene::tangent::sloped&&slope==2);
    assert(pico_path_scene::direction(NAN,1,slope)==pico_path_scene::tangent::unavailable);
    pico_path_scene path;uint8_t bytes[600];assert(path.valid()&&path.encode(bytes,sizeof bytes)==548);
    pico_path_scene copy;copy.polar=1;assert(copy.decode(bytes,548)&&copy.polar==0);
    bytes[77]^=1;assert(!copy.decode(bytes,548)&&copy.polar==0);bytes[77]^=1;assert(!copy.decode(bytes,547));
    path.first=path.last;assert(!path.valid()&&!path.encode(bytes,sizeof bytes));
    assert(std::fabs(pico_path_scene::radians(90,0)-1.5707963267948966)<1e-14);
    assert(std::fabs(pico_path_scene::radians(100,2)-1.5707963267948966)<1e-14);
    assert(std::fabs(pico_path_scene::radians(.5,3)-1.5707963267948966)<1e-14);
    double values[602];values[0]=42;values[601]=99;context c;
    assert(pico_plot_engine::sample_path(0,6.283185307179586,circle,&c,values+1)==1&&c.count==300);
    for(int i=0;i<300;++i)assert(std::fabs(values[1+i]*values[1+i]+values[301+i]*values[301+i]-1)<1e-12);
    assert(values[0]==42&&values[601]==99);
    assert(pico_plot_engine::sample_path(-1,1,gap,nullptr,values+1)==1&&std::isnan(values[1])&&std::isfinite(values[300]));
    c={0,10,-1};assert(pico_plot_engine::sample_path(0,1,circle,&c,values+1)==-1&&c.count==10);
    c={0,10,-2};assert(pico_plot_engine::sample_path(0,1,circle,&c,values+1)==-2);
    c={};assert(pico_plot_engine::sample_path(0,1,circle,&c,values+1,stop)==-2&&!c.count);
    assert(pico_plot_engine::sample_path(1,1,circle,&c,values+1)==-1);
    pico_data_table data;data.demo();pico_graph_scene scene;char formula[160];
    for(unsigned m=0;m<4;++m)assert(pico_data_formula(m,1,2,formula,sizeof formula));
    assert(pico_data_curve_scene(data,0,1,2,scene)&&!strcmp(scene.curve[0],"(1)+(2)*X")&&scene.curve[1][0]==0);
    auto old=scene;assert(!pico_data_curve_scene(data,4,1,2,scene)&&!strcmp(scene.curve[0],old.curve[0]));
    assert(!pico_data_formula(0,INFINITY,2,formula,sizeof formula));
    puts("PASS: parametric/polar codec, angle conversion, native path sampling/guards/gaps/cancellation and fitted-scene export");
}
