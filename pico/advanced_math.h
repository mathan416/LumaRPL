#pragma once
#include "math_lab.h"

// Pointer-free inputs; inactive documents use the existing checked PSRAM cache.
struct pico_advanced_math {
 bool dirty=false;unsigned degree[2]={2,1},ode_mode=0;
 char coefficients[2][9][24]={{"1","0","-1"},{"1","-1"}};
 char slope[2][160]={"Z","-Y"};
 double x0=0,y0=1,z0=0,last=6.283185307179586;
 double resistance=1000,inductance=.01,capacitance=.000001,frequency=1000,first_frequency=10,last_frequency=100000;
 double confidence=.95,null_mean=0,null_proportion=.5,successes=5,trials=10;
 unsigned column=0;
 char units[6][20]{}; // ODE X,Y,Z; polynomial X, output A, output B
 static constexpr size_t bytes=1024;
 bool valid()const {
  if(degree[0]<1||degree[0]>8||degree[1]<1||degree[1]>8||ode_mode>1||column>2)return false;
  for(int k=0;k<2;++k)for(unsigned i=0;i<=degree[k];++i){auto &s=coefficients[k][i];size_t n=strnlen(s,24);if(!n||n>=24||!pico_text_editor::valid_utf8(s,n)||strchr(s,'\''))return false;}
  for(auto &u:units)if(strnlen(u,20)>=20||!pico_text_editor::valid_utf8(u,strlen(u)))return false;
  for(auto &s:slope)if(!pico_graph_scene::formula(s,false))return false;
  for(double v:{x0,y0,z0,last,resistance,inductance,capacitance,frequency,first_frequency,last_frequency,confidence,null_mean,null_proportion,successes,trials})if(!std::isfinite(v)||fabs(v)>1e100)return false;
  return x0!=last&&fabs(x0)<=1e6&&fabs(last)<=1e6&&resistance>=0&&inductance>=0&&capacitance>0&&frequency>0&&first_frequency>0&&first_frequency<last_frequency&&confidence>0&&confidence<1&&null_proportion>0&&null_proportion<1&&trials>=1&&trials<=1e9&&trials==floor(trials)&&successes>=0&&successes<=trials&&successes==floor(successes);
 }
 size_t encode(uint8_t *out)const {
  if(!valid())return 0;memset(out,0,bytes);memcpy(out,"LUMAADV",7);out[7]=2;out[8]=degree[0];out[9]=degree[1];out[10]=ode_mode;out[11]=column;
  size_t at=16;memcpy(out+at,coefficients,sizeof coefficients);at+=sizeof coefficients;memcpy(out+at,slope,sizeof slope);at+=sizeof slope;
  for(double v:{x0,y0,z0,last,resistance,inductance,capacitance,frequency,first_frequency,last_frequency,confidence,null_mean,null_proportion,successes,trials}){uint64_t b;memcpy(&b,&v,8);for(int i=0;i<8;++i)out[at++]=b>>(8*i);}
  memcpy(out+at,units,sizeof units);at+=sizeof units;
  uint32_t c=pico_math_lab::checksum(out,bytes-4);for(int i=0;i<4;++i)out[bytes-4+i]=c>>(8*i);return bytes;
 }
 bool decode(const uint8_t *in,size_t n){
  if(n!=bytes||memcmp(in,"LUMAADV",7)||(in[7]!=1&&in[7]!=2))return false;uint32_t c=0;for(int i=0;i<4;++i)c|=uint32_t(in[bytes-4+i])<<(8*i);if(c!=pico_math_lab::checksum(in,bytes-4))return false;
  pico_advanced_math next;next.degree[0]=in[8];next.degree[1]=in[9];next.ode_mode=in[10];next.column=in[11];size_t at=16;memcpy(next.coefficients,in+at,sizeof coefficients);at+=sizeof coefficients;memcpy(next.slope,in+at,sizeof slope);at+=sizeof slope;
  double *values[]={&next.x0,&next.y0,&next.z0,&next.last,&next.resistance,&next.inductance,&next.capacitance,&next.frequency,&next.first_frequency,&next.last_frequency,&next.confidence,&next.null_mean,&next.null_proportion,&next.successes,&next.trials};
  for(auto p:values){uint64_t b=0;for(int i=0;i<8;++i)b|=uint64_t(in[at++])<<(8*i);memcpy(p,&b,8);}if(in[7]==2){memcpy(next.units,in+at,sizeof units);at+=sizeof units;}if(!next.valid())return false;*this=next;return true;
 }
};
namespace pico_inference {
struct summary {unsigned n=0;double mean=0,m2=0;bool add(double v){if(!std::isfinite(v))return false;double d=v-mean;mean+=d/++n;m2+=d*(v-mean);return std::isfinite(m2);}double variance()const{return n>1?m2/(n-1):NAN;}};
// Regularized incomplete beta: bounded continued fraction for Student t tails.
inline double beta_fraction(double a,double b,double x){double qab=a+b,qap=a+1,qam=a-1,c=1,d=1-qab*x/qap;const double tiny=1e-300;if(fabs(d)<tiny)d=tiny;d=1/d;double h=d;for(int m=1;m<=256;++m){double aa=m*(b-m)*x/((qam+2*m)*(a+2*m));d=1+aa*d;if(fabs(d)<tiny)d=tiny;c=1+aa/c;if(fabs(c)<tiny)c=tiny;d=1/d;h*=d*c;aa=-(a+m)*(qab+m)*x/((a+2*m)*(qap+2*m));d=1+aa*d;if(fabs(d)<tiny)d=tiny;c=1+aa/c;if(fabs(c)<tiny)c=tiny;d=1/d;double delta=d*c;h*=delta;if(fabs(delta-1)<2e-14)return h;}return NAN;}
inline double beta(double a,double b,double x){if(x<=0)return 0;if(x>=1)return 1;double f=exp(lgamma(a+b)-lgamma(a)-lgamma(b)+a*log(x)+b*log1p(-x));return x<(a+1)/(a+b+2)?f*beta_fraction(a,b,x)/a:1-f*beta_fraction(b,a,1-x)/b;}
inline double t_tail(double t,double df){if(!(df>0)||!std::isfinite(t))return NAN;double p=.5*beta(df/2,.5,df/(df+t*t));return t>=0?p:1-p;}
inline double t_quantile(double p,double df){if(p<=0||p>=1||df<=0)return NAN;if(p==.5)return 0;if(p<.5)return -t_quantile(1-p,df);double lo=0,hi=1;while(t_tail(hi,df)>1-p&&hi<1e12)hi*=2;for(int i=0;i<100;++i){double mid=(lo+hi)/2;if(t_tail(mid,df)>1-p)lo=mid;else hi=mid;}return (lo+hi)/2;}
}
namespace pico_engineering {
struct complex {
 double re=0,im=0;complex()=default;complex(double r,double i=0):re(r),im(i){}double real()const{return re;}double imag()const{return im;}
 friend complex operator+(complex a,complex b){return {a.re+b.re,a.im+b.im};}
 friend complex operator/(complex a,complex b){if(fabs(b.re)>=fabs(b.im)){double r=b.im/b.re,d=b.re+b.im*r;return {(a.re+a.im*r)/d,(a.im-a.re*r)/d};}double r=b.re/b.im,d=b.im+b.re*r;return {(a.re*r+a.im)/d,(a.im*r-a.re)/d};}
 };
 inline double abs(complex z){return hypot(z.re,z.im);}inline double arg(complex z){return atan2(z.im,z.re);}
struct result {complex series,parallel,lowpass,highpass;bool valid=false;};
inline result evaluate(double r,double l,double c,double f){result z;if(!std::isfinite(r)||!std::isfinite(l)||!std::isfinite(c)||!std::isfinite(f)||r<0||l<0||c<=0||f<=0)return z;double w=6.283185307179586*f;z.series={r,w*l-1/(w*c)};if(r>0&&l>0)z.parallel=1./(complex(1/r,0)+1./complex(0,w*l)+complex(0,w*c));else z.parallel={0,0};z.lowpass=1./complex(1,w*r*c);z.highpass=complex(0,w*r*c)/complex(1,w*r*c);z.valid=std::isfinite(abs(z.series))&&std::isfinite(abs(z.parallel))&&std::isfinite(abs(z.lowpass))&&std::isfinite(abs(z.highpass));return z;}
}
namespace pico_system_ode {
using sample=bool(*)(void *,double,const double *,double *);using poll=bool(*)(void *);
struct solver {
 sample f;void *context;poll cancel=nullptr;void *cancel_context=nullptr;unsigned evaluations=0;int status=1;
 bool value(double x,const double *y,double *out){if(cancel&&cancel(cancel_context)){status=-2;return false;}if(evaluations>=8192){status=-3;return false;}++evaluations;if(!f(context,x,y,out)){status=0;return false;}for(int i=0;i<2;++i)if(!std::isfinite(out[i])||fabs(out[i])>1e100){status=0;return false;}return true;}
 bool step(double x,const double *y,double h,double *out){double k[4][2],v[2];if(!value(x,y,k[0]))return false;for(int j=1;j<4;++j){for(int i=0;i<2;++i)v[i]=y[i]+h*(j==3?1:.5)*k[j-1][i];if(!value(x+h*(j==3?1:.5),v,k[j]))return false;}for(int i=0;i<2;++i){out[i]=y[i]+h*(k[0][i]+2*k[1][i]+2*k[2][i]+k[3][i])/6;if(!std::isfinite(out[i])||fabs(out[i])>1e100){status=0;return false;}}return true;}
 bool advance(double &x,double *y,double target,double &h){for(int tries=0;x!=target&&tries<1024;++tries){double step_size=copysign(fmin(fabs(target-x),h),target-x),whole[2],half[2],next[2];if(step_size==0||x+step_size==x){status=-3;return false;}if(!step(x,y,step_size,whole)||!step(x,y,step_size/2,half)||!step(x+step_size/2,half,step_size/2,next))return false;double ratio=0;for(int i=0;i<2;++i)ratio=fmax(ratio,fabs(next[i]-whole[i])/(15*1e-7*fmax(1.,fmax(fabs(y[i]),fabs(next[i])))));if(ratio>1){h=fabs(step_size)/2;continue;}for(int i=0;i<2;++i)y[i]=next[i]+(next[i]-whole[i])/15;x=fabs(target-x)<=fabs(step_size)?target:x+step_size;if(ratio<.03)h=fabs(step_size)*2;}if(x!=target){status=-3;return false;}return true;}
};
}
