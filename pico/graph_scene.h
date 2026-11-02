#pragma once
#include "wireframe.h"
#include "text_editor.h"
#include <stdint.h>
#include <stddef.h>
#include <cstring>
#include <cmath>
struct pico_graph_scene {
    char curve[3][160]={"X^2","X",""};uint32_t colour[3]={0x3080ef,0xf09030,0xc050d0};
    double window[4]={-10,10,-6,6};char surface[160]="X^2-Y^2";
    double bounds3d[4]={-3,3,-3,3};pico_wireframe::camera camera;uint32_t angle=0,multiple=0;
    static bool formula(const char *s,bool empty=true)
    {
        const char *end=static_cast<const char *>(memchr(s,0,160));if(!end||(!empty&&end==s))return false;
        for(const char *p=s;p<end;++p)if(static_cast<unsigned char>(*p)<32||*p==127||*p=='\'')return false;return pico_text_editor::valid_utf8(s,end-s);
    }
    static bool bounds(const double *v)
    {for(int i=0;i<4;++i)if(!std::isfinite(v[i])||std::fabs(v[i])>1e100)return false;return v[0]<v[1]&&v[2]<v[3];}
    bool valid()const
    {
        for(int i=0;i<3;++i)if(!formula(curve[i])||colour[i]>0xffffff)return false;
        return formula(surface,false)&&bounds(window)&&bounds(bounds3d)&&angle<=3&&multiple<=1&&std::isfinite(camera.yaw)&&std::fabs(camera.yaw)<=10000&&std::isfinite(camera.pitch)&&camera.pitch>=-1.4f&&camera.pitch<=1.4f&&std::isfinite(camera.zoom)&&camera.zoom>=0.4f&&camera.zoom<=2.5f;
    }
    static uint32_t checksum(const uint8_t *data,size_t length)
    {uint32_t hash=2166136261u;for(size_t i=0;i<length;++i)hash=(hash^data[i])*16777619u;return hash;}
    size_t encode(uint8_t *data,size_t capacity)const
    {
        constexpr size_t length=4+4+640+12+64+24+4+4+4;
        if(capacity<length||!valid())return 0;size_t p=0;
        auto word=[&](uint32_t v){for(int i=0;i<4;++i)data[p++]=uint8_t(v>>(i*8));};
        auto real=[&](double v){uint64_t bits;memcpy(&bits,&v,8);word(uint32_t(bits));word(uint32_t(bits>>32));};
        word(0x36534750);word(1);memcpy(data+p,curve,sizeof curve);p+=sizeof curve;memcpy(data+p,surface,sizeof surface);p+=sizeof surface;
        for(auto c:colour)word(c);for(double v:window)real(v);for(double v:bounds3d)real(v);
        real(camera.yaw);real(camera.pitch);real(camera.zoom);word(angle);word(multiple);uint32_t crc=checksum(data,p);word(crc);return p;
    }
    bool decode(const uint8_t *data,size_t length)
    {
        constexpr size_t expected=4+4+640+12+64+24+4+4+4;if(length!=expected)return false;
        size_t p=0;auto word=[&](){uint32_t v=0;for(int i=0;i<4;++i)v|=uint32_t(data[p++])<<(i*8);return v;};
        auto real=[&](){uint64_t lo=word(),hi=word(),bits=lo|(hi<<32);double v;memcpy(&v,&bits,8);return v;};
        if(word()!=0x36534750||word()!=1)return false;pico_graph_scene candidate;
        memcpy(candidate.curve,data+p,sizeof curve);p+=sizeof curve;memcpy(candidate.surface,data+p,sizeof surface);p+=sizeof surface;
        for(auto &c:candidate.colour)c=word();for(auto &v:candidate.window)v=real();for(auto &v:candidate.bounds3d)v=real();
        double yaw=real(),pitch=real(),zoom=real();
        if(!std::isfinite(yaw)||std::fabs(yaw)>10000||!std::isfinite(pitch)||pitch<double(-1.4f)||pitch>double(1.4f)||!std::isfinite(zoom)||zoom<double(0.4f)||zoom>2.5)return false;
        candidate.camera.yaw=float(yaw);candidate.camera.pitch=float(pitch);candidate.camera.zoom=float(zoom);candidate.angle=word();candidate.multiple=word();
        uint32_t actual=checksum(data,p);if(word()!=actual||!candidate.valid())return false;*this=candidate;return true;
    }
};
