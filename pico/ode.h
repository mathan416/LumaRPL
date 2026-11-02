#pragma once
#include "graph_scene.h"
struct pico_ode_scene
{
    char formula[160]="Y";
    double x0=0,y0=1,window[4]={-2,2,-1,8};unsigned flags=3,angle=1;
    static constexpr size_t bytes=228;
    bool valid()const{return pico_graph_scene::formula(formula,false)&&std::isfinite(x0)&&std::isfinite(y0)&&std::fabs(x0)<=1e100&&std::fabs(y0)<=1e100&&pico_graph_scene::bounds(window)&&flags>=1&&flags<=3&&angle<=3;}
    size_t encode(uint8_t *out,size_t capacity)const
    {
        if(capacity<bytes||!valid())return 0;size_t p=0;
        auto word=[&](uint32_t n){for(int i=0;i<4;++i)out[p++]=uint8_t(n>>(8*i));};
        auto real=[&](double v){uint64_t bits;memcpy(&bits,&v,8);word(uint32_t(bits));word(uint32_t(bits>>32));};
        word(0x35314544);word(1);memcpy(out+p,formula,160);p+=160;real(x0);real(y0);for(double v:window)real(v);word(flags);word(angle);word(pico_graph_scene::checksum(out,p));return p;
    }
    bool decode(const uint8_t *data,size_t length)
    {
        if(length!=bytes)return false;size_t p=0;
        auto word=[&](){uint32_t n=0;for(int i=0;i<4;++i)n|=uint32_t(data[p++])<<(8*i);return n;};
        auto real=[&](){uint64_t lo=word(),hi=word(),bits=lo|(hi<<32);double v;memcpy(&v,&bits,8);return v;};
        if(word()!=0x35314544||word()!=1)return false;pico_ode_scene next;memcpy(next.formula,data+p,160);p+=160;next.x0=real();next.y0=real();for(double &v:next.window)v=real();next.flags=word();next.angle=word();
        uint32_t crc=pico_graph_scene::checksum(data,p);if(word()!=crc||!next.valid())return false;*this=next;return true;
    }
};
namespace pico_ode {
typedef int (*function)(void *,double,double,double *);
struct result {int code=1,evaluations=0,accepted=0,rejected=0;bool partial=false,budget=false;};
struct solver
{
    function sample;void *context;result info;
    bool value(double x,double y,double &f)
    {
        if(info.evaluations>=8192){info.partial=info.budget=true;return false;}
        if(!std::isfinite(x)||!std::isfinite(y)||std::fabs(x)>1e100||std::fabs(y)>1e100){info.partial=true;return false;}
        ++info.evaluations;int code=sample(context,x,y,&f);
        if(code<0){info.code=code;return false;}
        if(code!=1||!std::isfinite(f)||std::fabs(f)>1e100){info.partial=true;return false;}return true;
    }
    bool step(double x,double y,double h,double &next)
    {
        double a,b,c,d;
        if(!value(x,y,a)||!value(x+h/2,y+h*a/2,b)||!value(x+h/2,y+h*b/2,c)||!value(x+h,y+h*c,d))return false;
        next=y+h*(a+2*b+2*c+d)/6;return std::isfinite(next)&&std::fabs(next)<=1e100;
    }
    bool advance(double &x,double &y,double target,double &suggested)
    {
        for(int tries=0;x!=target&&tries<1024;++tries)
        {
            double h=std::copysign(std::fmin(std::fabs(target-x),suggested),target-x),full,half,next;
            if(!std::isfinite(h)||h==0||x+h==x){info.partial=true;return false;}
            if(!step(x,y,h,full)||!step(x,y,h/2,half)||!step(x+h/2,half,h/2,next)){info.partial=true;return false;}
            double delta=(next-full)/15,tolerance=1e-7*std::fmax(1.0,std::fmax(std::fabs(y),std::fabs(next)));
            if(std::fabs(delta)>tolerance){suggested=std::fabs(h)/2;++info.rejected;continue;}
            y=next+delta;if(!std::isfinite(y)||std::fabs(y)>1e100){info.partial=true;return false;}
            double prior=x;x=std::fabs(target-x)<=std::fabs(h)?target:x+h;
            if(x==prior){info.partial=true;return false;}++info.accepted;
            suggested=std::fabs(h)*(std::fabs(delta)<tolerance/32?2:1);
        }
        if(x!=target){info.partial=true;return false;}return true;
    }
};
inline result solve(function sample,void *context,double x0,double y0,double first,double last,double *values)
{
    result invalid;invalid.code=-1;
    if(!sample||!values||!std::isfinite(first)||!std::isfinite(last)||first>=last||!std::isfinite(x0)||!std::isfinite(y0)||std::fabs(first)>1e100||std::fabs(last)>1e100||std::fabs(x0)>1e100||std::fabs(y0)>1e100)return invalid;
    for(int i=0;i<300;++i)values[i]=NAN;solver engine{sample,context};int split=0;
    while(split<300&&first*(1-double(split)/299)+last*(double(split)/299)<x0)++split;
    for(int direction=0;direction<2;++direction)
    {
        double x=x0,y=y0,suggested=std::fabs(last-first)/299;if(suggested==0)return invalid;
        int begin=direction?split-1:split,end=direction?-1:300,increment=direction?-1:1;
        for(int i=begin;i!=end;i+=increment)
        {
            double t=double(i)/299,target=first*(1-t)+last*t;
            if(!engine.advance(x,y,target,suggested))break;values[i]=y;
        }
        if(engine.info.code<0||engine.info.budget)break;
    }
    return engine.info;
}
inline double interpolate(const double *values,double first,double last,double x)
{
    if(!values||x<first||x>last||!std::isfinite(x)||first>=last)return NAN;
    double position=(x-first)/(last-first)*299;int i=int(position);if(i>=299)return values[299];
    double f=position-i;if(f==0)return values[i];
    return std::isfinite(values[i])&&std::isfinite(values[i+1])?values[i]*(1-f)+values[i+1]*f:NAN;
}
inline double field_angle(double slope,double xspan,double yspan)
{return slope==0?0:std::atan(slope*(xspan/yspan)*(209.0/299));}
}
