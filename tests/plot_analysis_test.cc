#include "plot_analysis.h"
#include "formula_tools.h"
#include "plot_explore.h"
#include <cassert>
#include <cstdio>
#include <cstring>
using namespace pico_plot_analysis;
struct fixture {int mode=0,calls=0,stop=0,code=-2;};
int sample(void *opaque,int curve,double x,double,double *y)
{
    auto &f=*static_cast<fixture *>(opaque);if(++f.calls==f.stop)return f.code;
    switch(f.mode)
    {
    case 0:*y=curve==0?x*x-4:x;break;
    case 1:*y=1/(x-.1234567);break;
    case 2:*y=(x-.12345)*(x-.12345);break;
    case 3:*y=(x-.12345)*(x-.12345)+1e-12;break;
    case 4:*y=-x*x+4;break;
    case 5:*y=x*x;break;
    case 6:*y=x;break;
    case 7:*y=std::sin(x);break;
    case 8:*y=std::pow(std::sin(4*3.141592653589793*x),2);break;
    case 9:*y=std::fabs(x);break;
    case 10:*y=0;break;
    case 11:*y=std::sqrt(x)-1;break;
    case 12:*y=std::sin(40*x);break;
    case 13:*y=x*x-1e-20;break;
    case 14:*y=x<.12345?-1:1;break;
    default:return 0;
    }
    return std::isfinite(*y)?1:0;
}
result check(operation op,fixture &f,double a=-3,double b=3,double at=0,int other=1)
{return run(op,sample,&f,0,other,a,b,at);}
int main()
{
    pico_plot_explore::selection bounds;
    assert(!bounds.ready());bounds.mark(2);assert(bounds.count==1);bounds.mark(-2);assert(bounds.ready()&&bounds.first==2&&bounds.last==-2);
    bounds.mark(0);assert(bounds.count==1);bounds.mark(0);assert(!bounds.ready());bounds.clear();assert(!bounds.count);
    double line;assert(pico_plot_explore::tangent(3,2,4,4,line)&&line==8);
    assert(!pico_plot_explore::tangent(1e308,0,0,1e308,line));
    fixture between;auto area=check(operation::between,between,0,1);
    assert(area.state==status::ok&&std::fabs(area.value-25.0/6)<1e-8);
    between={};area=check(operation::between,between,1,0);assert(area.state==status::ok&&std::fabs(area.value-25.0/6)<1e-8);
    between={};area=check(operation::between,between,-3,3);assert(area.state==status::ok&&area.value>0);
    between={};area=check(operation::between,between,0,1,0,0);assert(area.state==status::invalid);
    between={0,0,2,-1};area=check(operation::between,between,0,1);assert(area.state==status::callback_error);
    between={0,0,2,-2};area=check(operation::between,between,0,1);assert(area.state==status::cancelled);
    fixture limited;auto exhausted=run(operation::roots,sample,&limited,0,1,-3,3,0,nullptr,nullptr,2);
    assert(exhausted.state==status::budget&&exhausted.evaluations==2);
    fixture f;auto r=check(operation::roots,f);assert(r.state==status::ok&&r.count==2&&std::fabs(r.points[0].x+2)<1e-8&&std::fabs(r.points[1].x-2)<1e-8);
    f={};r=check(operation::intersections,f);assert(r.state==status::ok&&r.count==2);for(int i=0;i<2;++i)assert(std::fabs(r.points[i].x*r.points[i].x-4-r.points[i].x)<1e-8);
    f={1};r=check(operation::roots,f);assert(r.state==status::no_result); // Pole must not be reported as a root.
    f={14};r=check(operation::roots,f);assert(r.state==status::no_result);
    f={2};r=check(operation::roots,f);assert(r.state==status::ok&&r.count==1&&r.points[0].x==.12345);
    f={3};r=check(operation::roots,f);assert(r.state==status::no_result); // Tiny positive minimum is not a zero.
    f={13};r=check(operation::roots,f,-1e-8,1e-8);assert(r.state==status::ok&&r.count==2&&std::fabs(r.points[0].x+1e-10)<1e-18&&std::fabs(r.points[1].x-1e-10)<1e-18);
    f={10};r=check(operation::roots,f);assert(r.state==status::not_converged&&!r.count);
    f={5};r=check(operation::minimum,f,-2,3);assert(r.state==status::ok&&std::fabs(r.points[0].x)<1e-8&&r.value<1e-16);
    f={5};r=check(operation::maximum,f,-2,3);assert(r.state==status::ok&&r.points[0].x==3&&r.value==9);
    f={4};r=check(operation::maximum,f);assert(r.state==status::ok&&std::fabs(r.points[0].x)<1e-7&&std::fabs(r.value-4)<1e-12);
    f={5};r=check(operation::integral,f,0,1);assert(r.state==status::ok&&std::fabs(r.value-1.0/3)<1e-10);
    f={5};r=check(operation::integral,f,1,0);assert(r.state==status::ok&&std::fabs(r.value+1.0/3)<1e-10);
    f={6};r=check(operation::area,f,-.3,.7);assert(r.state==status::ok&&std::fabs(r.value-.29)<1e-7);
    f={7};r=check(operation::integral,f,0,3.141592653589793);assert(r.state==status::ok&&std::fabs(r.value-2)<1e-8);
    f={8};r=check(operation::integral,f,0,1);assert(r.state==status::ok&&std::fabs(r.value-.5)<1e-8);
    f={1};r=check(operation::integral,f,-1,1);assert(r.state!=status::ok);
    f={5};r=check(operation::derivative,f,-3,3,2);assert(r.state==status::ok&&std::fabs(r.value-4)<1e-8);
    f={9};r=check(operation::derivative,f);assert(r.state!=status::ok); // Cusp: centred difference alone would incorrectly say zero.
    f={9};r=check(operation::derivative,f,-1,1,1e-7);assert(r.state==status::ok&&std::fabs(r.value-1)<1e-8);
    f={11};r=check(operation::roots,f,-2,2);assert(r.state==status::ok&&r.count==1&&r.points[0].x==1);
    f={12};r=check(operation::roots,f,-10,10);assert(r.state==status::ok&&r.truncated&&r.count==16&&r.evaluations<=8192);
    f={0,0,10,-2};r=check(operation::roots,f);assert(r.state==status::cancelled);
    f={0,0,10,-1};r=check(operation::roots,f);assert(r.state==status::callback_error);
    f={};r=check(operation::roots,f,1,1);assert(r.state==status::invalid&&!f.calls);
    r=check(operation::integral,f,1,1);assert(r.state==status::ok&&r.value==0);
    char buffer[160]="X^2";pico_text_editor editor{buffer,sizeof buffer,3,3};
    assert(pico_formula_insert(editor,"SIN(",")",true)&&!strcmp(buffer,"SIN()")&&editor.cursor==4);
    assert(editor.insert("X",1)&&!strcmp(buffer,"SIN(X)"));
    editor.cursor=0;assert(pico_formula_insert(editor,"∂(",",X)",false)&&editor.cursor==4);
    char fraction[160]="(X)/()";pico_text_editor slots{fraction,sizeof fraction,6,2};pico_formula_next_slot(slots);assert(slots.cursor==5);
    char label[3];pico_formula_label(label,sizeof label,"πX");assert(!strcmp(label,"π"));pico_formula_label(label,2,"πX");assert(!*label);
    char small[8]="123456";pico_text_editor full{small,sizeof small,6,3};assert(!pico_formula_insert(full,"SQRT(",")",false)&&!strcmp(small,"123456")&&full.cursor==3);
    puts("PASS: native roots/intersections, pole rejection, tangent roots, bounded extrema, adaptive integrals/areas, slopes/cusps, errors/cancellation and formula templates");
}
