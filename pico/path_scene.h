#pragma once
#include "graph_scene.h"
struct pico_path_scene
{
    char x[160]="COS(T)",y[160]="SIN(T)",radius[160]="2*COS(3*T)";
    double first=0,last=6.283185307179586,window[4]={-3,3,-2.1,2.1};
    unsigned polar=0,angle=1;
    static constexpr size_t bytes=548;
    bool valid()const
    {
        return polar<=1&&angle<=3&&pico_graph_scene::formula(x,false)&&pico_graph_scene::formula(y,false)&&pico_graph_scene::formula(radius,false)&&std::isfinite(first)&&std::isfinite(last)&&std::fabs(first)<=1e100&&std::fabs(last)<=1e100&&first<last&&pico_graph_scene::bounds(window);
    }
    static double turn(unsigned angle){return angle==0?360:angle==1?6.283185307179586:angle==2?400:2;}
    static double radians(double t,unsigned angle){return t*(angle==0?0.017453292519943295:angle==1?1:angle==2?0.015707963267948967:3.141592653589793);}
    enum class tangent { unavailable,sloped,vertical };
    static tangent direction(double dx,double dy,double &slope)
    {
        if(!std::isfinite(dx)||!std::isfinite(dy))return tangent::unavailable;
        if(std::fabs(dx)<=1e-10*std::fmax(1.0,std::fabs(dy)))return std::fabs(dy)>1e-12?tangent::vertical:tangent::unavailable;
        slope=dy/dx;return std::isfinite(slope)?tangent::sloped:tangent::unavailable;
    }
    size_t encode(uint8_t *out,size_t capacity)const
    {
        if(capacity<bytes||!valid())return 0;size_t at=0;
        auto word=[&](uint32_t n){for(int i=0;i<4;++i)out[at++]=uint8_t(n>>(i*8));};
        auto real=[&](double n){uint64_t bits;memcpy(&bits,&n,8);word(uint32_t(bits));word(uint32_t(bits>>32));};
        word(0x34315050);word(1);memcpy(out+at,x,160);at+=160;memcpy(out+at,y,160);at+=160;memcpy(out+at,radius,160);at+=160;
        real(first);real(last);for(double n:window)real(n);word(polar);word(angle);word(pico_graph_scene::checksum(out,at));return at;
    }
    bool decode(const uint8_t *data,size_t length)
    {
        if(length!=bytes)return false;size_t at=0;
        auto word=[&](){uint32_t n=0;for(int i=0;i<4;++i)n|=uint32_t(data[at++])<<(i*8);return n;};
        auto real=[&](){uint64_t lo=word(),hi=word(),bits=lo|(hi<<32);double n;memcpy(&n,&bits,8);return n;};
        if(word()!=0x34315050||word()!=1)return false;pico_path_scene next;
        memcpy(next.x,data+at,160);at+=160;memcpy(next.y,data+at,160);at+=160;memcpy(next.radius,data+at,160);at+=160;
        next.first=real();next.last=real();for(double &n:next.window)n=real();next.polar=word();next.angle=word();
        uint32_t crc=pico_graph_scene::checksum(data,at);if(word()!=crc||!next.valid())return false;*this=next;return true;
    }
};
