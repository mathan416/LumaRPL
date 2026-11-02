#include "science_document.h"
#include "science_numeric.h"
#include <cassert>
#include <cstdio>
static bool model(void*,double x,const double *p,double &v){v=p[0]*exp(p[1]*x)+p[2];return true;}
int main(){
 pico_science_document doc;assert(doc.valid());uint8_t bytes[doc.bytes];assert(doc.encode(bytes)==sizeof bytes);pico_science_document read;assert(read.decode(bytes,sizeof bytes));bytes[33]^=1;assert(!read.decode(bytes,sizeof bytes));doc.fs=NAN;assert(!doc.valid());
 pico_data_table data;data.rows=8;for(unsigned i=0;i<data.rows;++i){data.points[i][0]=i;data.points[i][1]=2*i+1;}double y;assert(pico_science::interpolate(data,2.5,0,y)&&fabs(y-6)<1e-12);assert(pico_science::interpolate(data,2.5,1,y)&&fabs(y-6)<1e-12);assert(!pico_science::interpolate(data,-1,1,y));data.points[3][0]=2;assert(!pico_science::ordered(data));data.points[3][0]=3;
 for(unsigned i=0;i<data.rows;++i)data.points[i][1]=i<3?0:i<5?1:2;for(double x=0;x<7;x+=.01){assert(pico_science::interpolate(data,x,1,y));assert(y>=-1e-12&&y<=2+1e-12);}
 data.rows=12;for(unsigned i=0;i<data.rows;++i){data.points[i][0]=i*.2;data.points[i][1]=2*exp(.5*i*.2)+1;}double guess[]={1,.2,.3};auto fit=pico_science::fit(data,guess,model,nullptr);assert(fit.status==1&&fit.evaluations<=8192);assert(fabs(fit.p[0]-2)<1e-5&&fabs(fit.p[1]-.5)<1e-5&&fabs(fit.p[2]-1)<1e-5);auto stopped=pico_science::fit(data,guess,model,nullptr,[](void*){return true;});assert(stopped.status==-1);for(unsigned i=0;i<data.rows;++i)data.points[i][0]=0;assert(pico_science::fit(data,guess,model,nullptr).status==-2);
 data.rows=64;for(unsigned i=0;i<64;++i){data.points[i][0]=double(i)/64;data.points[i][1]=1+2*sin(6.283185307179586*8*i/64);}pico_science::spectrum fft;assert(fft.compute(data,64,0,false));assert(fabs(fft.amplitude(0)-1)<1e-12&&fabs(fft.amplitude(8)-2)<1e-12&&fft.frequency(8)==8&&fabs(fft.phase(8)+90)<1e-10);assert(fft.compute(data,64,1,true)&&fabs(fft.amplitude(8)-2)<.002);for(unsigned i=0;i<64;++i)data.points[i][1]=3*cos(3.141592653589793*i);assert(fft.compute(data,64,0,false)&&fabs(fft.amplitude(32)-3)<1e-12);data.rows=63;assert(!fft.compute(data,64,0,false));data.rows=64;assert(!fft.compute(data,64,0,false,[](void*){return true;}));
 for(unsigned i=0;i<64;++i)data.points[i][1]=1;pico_data_table out;assert(pico_science::filtered(data,out,64,4,0,5));for(unsigned i=0;i<64;++i)assert(out.points[i][1]==1);assert(pico_science::filtered(data,out,64,4,2,5));for(unsigned i=0;i<64;++i)assert(out.points[i][1]==0);assert(!pico_science::filtered(data,out,64,32,1,5));
 assert(pico_science::gcd(48,18)==6);uint64_t value;assert(pico_science::modpow(2,40,1000000007,value)&&value==511620083);assert(!pico_science::modpow(2,2,0,value));
 puts("PASS: science document CRC/ranges; linear/PCHIP accuracy, ordering and bounds; nonlinear fit values/singular/cancellation; FFT DC/bin/phase/Nyquist/window scaling and cancellation; filters and modular arithmetic");
}
