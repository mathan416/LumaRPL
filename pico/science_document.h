#pragma once
#include "math_lab.h"
#include "form_document.h"
// Inputs only. Results and working arrays remain transient and bounded.
struct pico_science_document {
    bool dirty=false;
    char quantity[160]="(12_V)/(4_ohm)",target[32]="A";
    char formula[160]="A*B",value[3][64]={"2_m","3_m","0"};
    char sigma[3][64]={"0.1_m","0.2_m","0"},units[3][24]{},output[32]{};
    char xunit[24]{},yunit[24]{};
    char fit[160]="A*EXP(B*X)+C";
    double guesses[3]={2,.5,1},at=1,fs=128,cutoff=10,initial=1;
    unsigned interpolation=0,window=1,demean=1,filter=0,span=5,terms=16;
    char integers[3][32]={"12","18","1000000007"};
    char recurrence[160]="2*U+1";
    static constexpr size_t bytes=2048;
    bool valid()const {
        if(interpolation>1||window>2||demean>1||filter>2||span<3||span>31||!(span&1)||terms<1||terms>128)return false;
        for(double v:{guesses[0],guesses[1],guesses[2],at,fs,cutoff,initial})if(!std::isfinite(v)||fabs(v)>1e100)return false;
        if(fs<=0||cutoff<=0||cutoff>=fs/2)return false;
        for(auto &v:integers)if(!pico_form_document::text(v,sizeof v))return false;
        for(int i=0;i<3;++i)if(!pico_form_document::text(value[i],64)||!pico_form_document::text(sigma[i],64)||!pico_form_document::text(units[i],24,true))return false;
        return pico_graph_scene::formula(quantity,false)&&pico_graph_scene::formula(formula,false)&&pico_graph_scene::formula(fit,false)&&pico_graph_scene::formula(recurrence,false)&&pico_form_document::text(target,32,true)&&pico_form_document::text(output,32,true)&&pico_form_document::text(xunit,24,true)&&pico_form_document::text(yunit,24,true);
    }
    size_t encode(uint8_t *out)const {
        if(!valid())return 0;memset(out,0,bytes);memcpy(out,"LUMASCI",7);out[7]=1;
        out[8]=interpolation;out[9]=window;out[10]=demean;out[11]=filter;out[12]=span;out[13]=terms;
        size_t pos=16;auto text=[&](const void *p,size_t n){memcpy(out+pos,p,n);pos+=n;};
        text(quantity,sizeof quantity);text(target,sizeof target);text(formula,sizeof formula);text(value,sizeof value);text(sigma,sizeof sigma);text(units,sizeof units);text(output,sizeof output);text(xunit,sizeof xunit);text(yunit,sizeof yunit);text(fit,sizeof fit);text(integers,sizeof integers);text(recurrence,sizeof recurrence);
        for(double v:{guesses[0],guesses[1],guesses[2],at,fs,cutoff,initial}){uint64_t b;memcpy(&b,&v,8);for(int i=0;i<8;++i)out[pos++]=b>>(8*i);}
        uint32_t crc=pico_math_lab::checksum(out,bytes-4);for(int i=0;i<4;++i)out[bytes-4+i]=crc>>(8*i);return bytes;
    }
    bool decode(const uint8_t *in,size_t n){
        if(n!=bytes||memcmp(in,"LUMASCI",7)||in[7]!=1||in[14]||in[15])return false;uint32_t crc=0;for(int i=0;i<4;++i)crc|=uint32_t(in[bytes-4+i])<<(8*i);if(crc!=pico_math_lab::checksum(in,bytes-4))return false;
        pico_science_document next;next.interpolation=in[8];next.window=in[9];next.demean=in[10];next.filter=in[11];next.span=in[12];next.terms=in[13];
        size_t pos=16;auto text=[&](void *p,size_t n){memcpy(p,in+pos,n);pos+=n;};
        text(next.quantity,sizeof quantity);text(next.target,sizeof target);text(next.formula,sizeof formula);text(next.value,sizeof value);text(next.sigma,sizeof sigma);text(next.units,sizeof units);text(next.output,sizeof output);text(next.xunit,sizeof xunit);text(next.yunit,sizeof yunit);text(next.fit,sizeof fit);text(next.integers,sizeof integers);text(next.recurrence,sizeof recurrence);
        double *values[]={&next.guesses[0],&next.guesses[1],&next.guesses[2],&next.at,&next.fs,&next.cutoff,&next.initial};for(auto p:values){uint64_t b=0;for(int i=0;i<8;++i)b|=uint64_t(in[pos++])<<(8*i);memcpy(p,&b,8);}
        if(!next.valid())return false;*this=next;return true;
    }
};
