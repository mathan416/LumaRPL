#include "advanced_math.h"
#include <cassert>
#include <cstdio>
int main(){
 pico_advanced_math d;assert(d.valid());uint8_t b[d.bytes];assert(d.encode(b)==sizeof b);pico_advanced_math next;assert(next.decode(b,sizeof b));b[33]^=1;assert(!next.decode(b,sizeof b));d.confidence=NAN;assert(!d.valid());
 pico_inference::summary s;for(double v:{1.,2.,3.,4.,5.})assert(s.add(v));assert(s.n==5&&s.mean==3&&s.variance()==2.5);assert(fabs(pico_inference::t_tail(0,4)-.5)<1e-14);assert(fabs(pico_inference::t_quantile(.975,4)-2.7764451051978)<1e-10);assert(fabs(pico_inference::t_tail(2.7764451051978,4)-.025)<1e-12);assert(fabs(pico_inference::t_quantile(.025,4)+2.7764451051978)<1e-10);
 assert(fabs(pico_inference::t_quantile(.975,1)-12.7062047364321)<1e-8);assert(fabs(pico_inference::t_quantile(.975,10)-2.22813885196494)<1e-10);
 auto z=pico_engineering::evaluate(1000,.01,1e-6,1/(2*3.141592653589793*.001));assert(z.valid);assert(fabs(abs(z.lowpass)-sqrt(.5))<1e-12);assert(fabs(arg(z.lowpass)+3.141592653589793/4)<1e-12);assert(fabs(arg(z.highpass)-3.141592653589793/4)<1e-12);assert(!pico_engineering::evaluate(1,1,0,1).valid);
 auto sample=[](void*,double,const double *y,double *f){f[0]=y[1];f[1]=-y[0];return true;};pico_system_ode::solver ode{sample,nullptr};double x=0,y[]={1.,0.},h=.1;assert(ode.advance(x,y,2*3.141592653589793,h));assert(fabs(y[0]-1)<1e-6&&fabs(y[1])<1e-6&&ode.evaluations<=8192);
 pico_system_ode::solver reverse{sample,nullptr};x=0;y[0]=1;y[1]=0;h=.1;assert(reverse.advance(x,y,-1,h));assert(fabs(y[0]-cos(1))<1e-6&&fabs(y[1]-sin(1))<1e-6);
 pico_system_ode::solver cancel{sample,nullptr,[](void*){return true;}};x=0;assert(!cancel.advance(x,y,1,h)&&cancel.status==-2);
 pico_system_ode::solver invalid{[](void*,double,const double*,double*){return false;},nullptr};assert(!invalid.advance(x,y,1,h)&&invalid.status==0);
 pico_system_ode::solver budget{sample,nullptr};budget.evaluations=8192;x=0;assert(!budget.advance(x,y,1,h)&&budget.status==-3);
 puts("PASS: advanced document CRC/validation; sample variance and Student t tails/quantiles; RC gain/phase; adaptive coupled ODE forward/backward accuracy, cancellation and domain failure");
}
