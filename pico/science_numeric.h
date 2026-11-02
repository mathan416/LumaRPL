#pragma once
#include "data_table.h"
#include <cstdint>
#include <algorithm>
namespace pico_science {
using poll=bool(*)(void*);
inline bool cancelled(poll p,void *c){return p&&p(c);}
inline bool ordered(const pico_data_table &d){if(!d.valid()||d.rows<2)return false;for(unsigned i=1;i<d.rows;++i)if(!(d.points[i][0]>d.points[i-1][0]))return false;return true;}
inline double endpoint(double h0,double h1,double d0,double d1){double s=((2*h0+h1)*d0-h0*d1)/(h0+h1);if(s*d0<=0)return 0;if(d0*d1<0&&fabs(s)>3*fabs(d0))return 3*d0;return s;}
inline double slope(const pico_data_table &d,unsigned i){unsigned n=d.rows;auto h=[&](unsigned j){return d.points[j+1][0]-d.points[j][0];};auto delta=[&](unsigned j){return (d.points[j+1][1]-d.points[j][1])/h(j);};if(n==2)return delta(0);if(i==0)return endpoint(h(0),h(1),delta(0),delta(1));if(i==n-1)return endpoint(h(n-2),h(n-3),delta(n-2),delta(n-3));double a=delta(i-1),b=delta(i);if(a*b<=0)return 0;double w1=2*h(i)+h(i-1),w2=h(i)+2*h(i-1);return (w1+w2)/(w1/a+w2/b);}
inline bool interpolate(const pico_data_table &d,double x,unsigned method,double &y){if(method>1||!ordered(d)||!std::isfinite(x)||x<d.points[0][0]||x>d.points[d.rows-1][0])return false;unsigned i=0;while(i+2<d.rows&&d.points[i+1][0]<x)++i;double h=d.points[i+1][0]-d.points[i][0],t=(x-d.points[i][0])/h,a=d.points[i][1],b=d.points[i+1][1];y=method?((2*t-3)*t*t+1)*a+((t-2)*t+1)*t*h*slope(d,i)+(-2*t+3)*t*t*b+(t-1)*t*t*h*slope(d,i+1):a+t*(b-a);return std::isfinite(y);}
struct fit_result {double p[3]{},rss=NAN;unsigned evaluations=0,iterations=0;int status=0;}; // 1 converged, 0 domain, -1 cancelled, -2 singular/limit
using model=bool(*)(void*,double,const double*,double&);
inline bool solve3(double a[3][3],double b[3],double x[3]){for(int k=0;k<3;++k){int best=k;for(int r=k+1;r<3;++r)if(fabs(a[r][k])>fabs(a[best][k]))best=r;if(!std::isfinite(a[best][k])||fabs(a[best][k])<1e-24)return false;if(best!=k){for(int j=k;j<3;++j){double t=a[k][j];a[k][j]=a[best][j];a[best][j]=t;}double t=b[k];b[k]=b[best];b[best]=t;}for(int r=k+1;r<3;++r){double f=a[r][k]/a[k][k];for(int j=k;j<3;++j)a[r][j]-=f*a[k][j];b[r]-=f*b[k];}}for(int k=2;k>=0;--k){double v=b[k];for(int j=k+1;j<3;++j)v-=a[k][j]*x[j];x[k]=v/a[k][k];if(!std::isfinite(x[k]))return false;}return true;}
inline fit_result fit(const pico_data_table &d,const double *initial,model evaluate,void *context,poll stop=nullptr,void *cancel=nullptr){fit_result r;for(int i=0;i<3;++i)r.p[i]=initial[i];if(!d.valid()||d.rows<4)return r;double scale=1;for(unsigned i=0;i<d.rows;++i)scale=fmax(scale,fabs(d.points[i][1]));double lambda=1e-3;
 auto sample=[&](double x,const double *p,double &v){if(cancelled(stop,cancel)){r.status=-1;return false;}if(r.evaluations>=8192){r.status=-2;return false;}++r.evaluations;return evaluate(context,x,p,v)&&std::isfinite(v);};
 for(unsigned iter=0;iter<100;++iter){r.iterations=iter+1;double normal[3][3]{},gradient[3]{},rss=0;
  for(unsigned row=0;row<d.rows;++row){double f,j[3];if(!sample(d.points[row][0],r.p,f))return r;double residual=(d.points[row][1]-f)/scale;rss+=residual*residual;
   for(int k=0;k<3;++k){double p[3]={r.p[0],r.p[1],r.p[2]},h=6e-6*fmax(1.,fabs(p[k])),a,b;p[k]+=h;if(!sample(d.points[row][0],p,a))return r;p[k]-=2*h;if(!sample(d.points[row][0],p,b))return r;j[k]=(a-b)/(2*h*scale);if(!std::isfinite(j[k]))return r;}
   for(int k=0;k<3;++k){gradient[k]+=j[k]*residual;for(int l=0;l<3;++l)normal[k][l]+=j[k]*j[l];}}
  if(!std::isfinite(rss))return r;r.rss=rss*scale*scale;
  double rank_matrix[3][3],rank_rhs[3]={1,1,1},rank_solution[3]{};memcpy(rank_matrix,normal,sizeof normal);if(!solve3(rank_matrix,rank_rhs,rank_solution)){r.status=-2;return r;}
  double g=fmax(fabs(gradient[0]),fmax(fabs(gradient[1]),fabs(gradient[2])));if(rss<1e-20||g<1e-10*fmax(1.,rss)){r.status=1;return r;}
  double matrix[3][3];memcpy(matrix,normal,sizeof normal);for(int k=0;k<3;++k)matrix[k][k]+=lambda*fmax(1e-12,normal[k][k]);double step[3]{};if(!solve3(matrix,gradient,step)){r.status=-2;return r;}double candidate[3];for(int k=0;k<3;++k)candidate[k]=r.p[k]+step[k];double next=0;for(unsigned row=0;row<d.rows;++row){double f;if(!sample(d.points[row][0],candidate,f))return r;double e=(d.points[row][1]-f)/scale;next+=e*e;}
  if(std::isfinite(next)&&next<rss){for(int k=0;k<3;++k)r.p[k]=candidate[k];lambda=fmax(1e-12,lambda*.3);}else{lambda*=10;if(lambda>1e15){r.status=-2;return r;}}
 }r.status=-2;return r;
}
struct spectrum {
 unsigned n=0;double fs=0,real[128]{},imag[128]{},gain=0;
 bool compute(const pico_data_table &d,double sample_rate,unsigned window,bool remove_mean,poll stop=nullptr,void *context=nullptr){n=d.rows;fs=sample_rate;if(!d.valid()||n<8||n>128||(n&(n-1))||!std::isfinite(fs)||fs<=0||window>2)return false;double mean=0;if(remove_mean)for(unsigned i=0;i<n;++i)mean+=d.points[i][1]/n;gain=0;
  for(unsigned i=0;i<n;++i){if(cancelled(stop,context))return false;double angle=6.283185307179586*i/(n-1),w=window==0?1:window==1?.5-.5*cos(angle):.54-.46*cos(angle);gain+=w;real[i]=(d.points[i][1]-mean)*w;imag[i]=0;}
  for(unsigned i=1,j=0;i<n;++i){unsigned bit=n>>1;for(;j&bit;bit>>=1)j^=bit;j^=bit;if(i<j){double t=real[i];real[i]=real[j];real[j]=t;}}
  for(unsigned len=2;len<=n;len<<=1){if(cancelled(stop,context))return false;for(unsigned base=0;base<n;base+=len)for(unsigned j=0;j<len/2;++j){double a=-6.283185307179586*j/len,c=cos(a),s=sin(a);unsigned i=base+j,k=i+len/2;double re=c*real[k]-s*imag[k],im=s*real[k]+c*imag[k];real[k]=real[i]-re;imag[k]=imag[i]-im;real[i]+=re;imag[i]+=im;}}
  return gain>0;
 }
 double frequency(unsigned bin)const{return double(bin)*fs/n;}
 double amplitude(unsigned bin)const{return hypot(real[bin],imag[bin])/gain*(bin&&bin!=n/2?2:1);}
 double phase(unsigned bin)const{double peak=0;for(unsigned i=0;i<=n/2;++i)peak=fmax(peak,amplitude(i));return amplitude(bin)>fmax(1e-300,peak*1e-12)?atan2(imag[bin],real[bin])*180/3.141592653589793:NAN;}
};
inline bool filtered(const pico_data_table &in,pico_data_table &out,double fs,double cutoff,unsigned mode,unsigned span,poll stop=nullptr,void *context=nullptr){if(!in.valid()||!in.rows||mode>2||!std::isfinite(fs)||!std::isfinite(cutoff)||fs<=0||cutoff<=0||cutoff>=fs/2||span<3||span>31||!(span&1))return false;out=in;double low=in.points[0][1],a=exp(-6.283185307179586*cutoff/fs);for(unsigned i=0;i<in.rows;++i){if(cancelled(stop,context))return false;double v;if(mode==0){unsigned first=i>span/2?i-span/2:0,last=std::min(in.rows-1,i+span/2);v=0;for(unsigned j=first;j<=last;++j)v+=in.points[j][1]/(last-first+1);}else{if(i)low=a*low+(1-a)*in.points[i][1];v=mode==1?low:in.points[i][1]-low;}if(!std::isfinite(v))return false;out.points[i][1]=v;}out.dirty=true;return true;}
inline uint64_t gcd(uint64_t a,uint64_t b){while(b){uint64_t t=a%b;a=b;b=t;}return a;}
inline uint64_t mulmod(uint64_t a,uint64_t b,uint64_t m){uint64_t r=0;a%=m;while(b){if(b&1)r=(r+a)%m;a=(a+a)%m;b>>=1;}return r;}
inline bool modpow(uint64_t a,uint64_t b,uint64_t m,uint64_t &v){if(!m||m>1000000000000ULL||a>1000000000000ULL||b>1000000000000ULL)return false;v=1%m;while(b){if(b&1)v=mulmod(v,a,m);a=mulmod(a,a,m);b>>=1;}return true;}
}
