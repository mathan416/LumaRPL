#pragma once
#include "graph_scene.h"
// Bounded, portable storage; doubles and integers are explicitly little endian.
struct pico_workbench
{
    char single[160]="COS(X)=X";
    char equations[3][160]={"X+Y=3","X-Y=1","Z=2"};
    double guess=0.5,guesses[3]={1,1,1},interval[2]={-3,3};
    struct parameter {double value,minimum,maximum,step;};
    parameter parameters[3]={{1,-5,5,0.1},{1,0.1,5,0.1},{0,-3.141592653589793,3.141592653589793,0.1}};
    unsigned count=2;
    pico_graph_scene scene;
    pico_workbench(){strcpy(scene.curve[0],"A*SIN(B*X+C)");scene.curve[1][0]=scene.curve[2][0]=0;scene.angle=1;scene.multiple=1;}
    static bool number(double n){return std::isfinite(n)&&std::fabs(n)<=1e100;}
    static bool valid_parameter(const parameter &p)
    {return number(p.value)&&number(p.minimum)&&number(p.maximum)&&number(p.step)&&p.minimum<p.maximum&&p.value>=p.minimum&&p.value<=p.maximum&&p.step>0;}
    bool valid()const
    {
        if(count<2||count>3||!pico_graph_scene::formula(single,false)||!number(guess)||!number(interval[0])||!number(interval[1])||interval[0]>=interval[1]||!scene.valid())return false;
        for(int i=0;i<3;++i)if(!pico_graph_scene::formula(equations[i],false)||!number(guesses[i])||!valid_parameter(parameters[i]))return false;
        return true;
    }
    void adjust(int index,int direction)
    {
        auto &p=parameters[index];double next=p.value+direction*p.step;
        p.value=std::fmax(p.minimum,std::fmin(p.maximum,next));
    }
    static constexpr size_t bytes=1564;
    size_t encode(uint8_t *out,size_t capacity)const
    {
        if(capacity<bytes||!valid())return 0;size_t at=0;
        auto word=[&](uint32_t n){for(int i=0;i<4;++i)out[at++]=uint8_t(n>>(8*i));};
        auto real=[&](double n){uint64_t b;memcpy(&b,&n,8);word(uint32_t(b));word(uint32_t(b>>32));};
        word(0x30314257);word(1);memcpy(out+at,single,160);at+=160;memcpy(out+at,equations,480);at+=480;
        real(guess);for(double n:guesses)real(n);for(double n:interval)real(n);
        for(const auto &p:parameters){real(p.value);real(p.minimum);real(p.maximum);real(p.step);}
        word(count);word(760);if(scene.encode(out+at,capacity-at)!=760)return 0;at+=760;
        word(pico_graph_scene::checksum(out,at));return at;
    }
    bool decode(const uint8_t *data,size_t length,const char **reason=nullptr)
    {
        auto fail=[&](const char *text){if(reason)*reason=text;return false;};
        if(reason)*reason="Valid";if(length!=bytes)return fail("Wrong file length");size_t at=0;
        auto word=[&](){uint32_t n=0;for(int i=0;i<4;++i)n|=uint32_t(data[at++])<<(8*i);return n;};
        auto real=[&](){uint64_t lo=word(),hi=word(),bits=lo|(hi<<32);double n;memcpy(&n,&bits,8);return n;};
        if(word()!=0x30314257||word()!=1)return fail("Wrong magic/version");pico_workbench next;
        memcpy(next.single,data+at,160);at+=160;memcpy(next.equations,data+at,480);at+=480;
        next.guess=real();for(double &n:next.guesses)n=real();for(double &n:next.interval)n=real();
        for(auto &p:next.parameters){p.value=real();p.minimum=real();p.maximum=real();p.step=real();}
        next.count=word();if(word()!=760||!next.scene.decode(data+at,760))return fail("Graph scene rejected");at+=760;
        uint32_t crc=pico_graph_scene::checksum(data,at);if(word()!=crc)return fail("Checksum mismatch");if(!next.valid())return fail("Invalid fields/parameters");*this=next;return true;
    }
};
