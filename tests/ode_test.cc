#include "ode.h"
#include "svg_export.h"
#include <cassert>
#include <string>
int main()
{
    pico_ode_scene scene,loaded;uint8_t encoded[pico_ode_scene::bytes];assert(scene.encode(encoded,sizeof encoded)==sizeof encoded&&loaded.decode(encoded,sizeof encoded));
    for(size_t i=0;i<sizeof encoded;++i){encoded[i]^=1;assert(!loaded.decode(encoded,sizeof encoded));encoded[i]^=1;}
    double samples[300];auto exponential=[](void*,double,double y,double *v){*v=y;return 1;};auto r=pico_ode::solve(exponential,nullptr,0,1,-2,2,samples);
    assert(r.code==1&&!r.partial&&r.evaluations<=8192&&fabs(samples[0]-exp(-2))<1e-6&&fabs(samples[299]-exp(2))<1e-5);
    auto logistic=[](void*,double,double y,double *v){*v=y*(1-y);return 1;};r=pico_ode::solve(logistic,nullptr,0,.1,-2,8,samples);assert(!r.partial&&fabs(samples[299]-1/(1+9*exp(-8)))<1e-5);
    auto holes=[](void*,double x,double,double *v){*v=1;return x>.5?0:1;};r=pico_ode::solve(holes,nullptr,0,0,-1,1,samples);assert(r.partial&&std::isnan(samples[299])&&std::isfinite(samples[0]));
    auto cancel=[](void*,double,double,double*){return -2;};r=pico_ode::solve(cancel,nullptr,0,0,-1,1,samples);assert(r.code==-2&&r.evaluations==1);
    auto stiff=[](void*,double,double y,double *v){*v=-1e6*y;return 1;};r=pico_ode::solve(stiff,nullptr,0,1,0,1,samples);assert(r.partial&&r.evaluations<=8192);
    assert(std::isnan(pico_ode::interpolate(samples,0,1,2)));assert(pico_ode::field_angle(0,2,4)==0);
    std::string output;auto sink=[](void *p,const char *s,size_t n){static_cast<std::string*>(p)->append(s,n);return true;};pico_svg::writer svg{sink,&output};svg.begin("<ODE & \"units\">",0,0xffffff);svg.line(-30,10,400,300,2,0xaabbcc);svg.end_plot();svg.text(5,280,"X<Y",0xffffff);assert(svg.finish());assert(output.find("&lt;ODE &amp; &quot;")!=std::string::npos&&output.find("clip-path=\"url(#plot)\"")!=std::string::npos&&output.find("#aabbcc")!=std::string::npos&&output.find("</svg>")!=std::string::npos);
    auto fail=[](void*,const char*,size_t){return false;};pico_svg::writer bad{fail,nullptr};bad.begin("fail",0,0);assert(!bad.finish());
    puts("PASS: adaptive ODE analytic solutions/backward/domain/cancel/budget/codec and SVG clipping/escaping/sink failure");
}
