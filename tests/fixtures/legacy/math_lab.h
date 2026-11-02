#pragma once
#include "graph_scene.h"
#include <cmath>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <initializer_list>
namespace pico_probability {
using poll=bool(*)(void *);
struct distribution {
 int kind=0;double first=0,second=1; // normal mean/sd, binomial n/p, Poisson lambda
 bool valid()const{return kind>=0&&kind<=2&&std::isfinite(first)&&std::isfinite(second)&&fabs(first)<=1e50&&(kind==0?second>0&&second<=1e50:kind==1?first>=0&&first<=2000&&first==floor(first)&&second>=0&&second<=1:first>0&&first<=1000);}
 double mass(double x)const{
  if(!valid()||!std::isfinite(x))return NAN;
  if(kind==0){double z=(x-first)/second;return exp(-.5*z*z)/(second*sqrt(6.2831853071795864769));}
  if(x<0||x!=floor(x)||kind==1&&x>first)return 0;
  if(kind==1){if(second==0)return x==0; if(second==1)return x==first;return exp(lgamma(first+1)-lgamma(x+1)-lgamma(first-x+1)+x*log(second)+(first-x)*log1p(-second));}
  return exp(-first+x*log(first)-lgamma(x+1));
 }
 double log_mass(int k)const{if(kind==1){if(second==0)return k==0?0:-INFINITY;if(second==1)return k==first?0:-INFINITY;return lgamma(first+1)-lgamma(k+1)-lgamma(first-k+1)+k*log(second)+(first-k)*log1p(-second);}return -first+k*log(first)-lgamma(k+1);}
 double sum(int lo,int hi,poll interrupted=nullptr,void *context=nullptr)const{
  lo=lo<0?0:lo;hi=hi>(kind==1?int(first):8192)?(kind==1?int(first):8192):hi;if(lo>hi)return 0;
  double s=-INFINITY;for(int k=lo;k<=hi;++k){if((k-lo)%32==0&&interrupted&&interrupted(context))return NAN;double term=log_mass(k);if(!std::isfinite(term))continue;if(!std::isfinite(s))s=term;else{double a=fmax(s,term);s=a+log1p(exp(fmin(s,term)-a));}}
  return std::isfinite(s)?fmin(1.,exp(s)):0;
 }
 double cdf(double x,bool upper=false,poll interrupted=nullptr,void *context=nullptr)const{
  if(!valid()||!std::isfinite(x))return NAN;
  if(kind==0)return .5*erfc((upper?1:-1)*(x-first)/(second*sqrt(2.)));
  if(x<0)return upper?1:0;if(x>=8192)return upper?0:1;
  if(kind==1&&x>=first)return upper?0:1;int k=int(floor(x));return upper?sum(k+1,kind==1?int(first):8192,interrupted,context):sum(0,k,interrupted,context);
 }
 double interval(double lo,double hi,poll interrupted=nullptr,void *context=nullptr)const{
  if(!valid()||!std::isfinite(lo)||!std::isfinite(hi)||lo>hi)return NAN;
  if(kind==0){double a=(lo-first)/(second*sqrt(2.)),b=(hi-first)/(second*sqrt(2.));if(fabs(a)<1&&fabs(b)<1)return fmax(0.,.5*(erf(b)-erf(a)));return fmax(0.,lo>=first?cdf(lo,true)-cdf(hi,true):cdf(hi)-cdf(lo));}
  if(hi<0||lo>8192)return 0;return sum(int(fmax(0.,ceil(lo))),int(fmin(8192.,floor(hi))),interrupted,context);
 }
 double quantile(double p,poll interrupted=nullptr,void *context=nullptr)const{
  if(!valid()||!std::isfinite(p)||p<=0||p>=1)return NAN;
  if(kind==0&&p==.5)return first;
  if(kind){int lo=0,hi=kind==1?int(first):int(ceil(first+16*sqrt(first)+64));while(lo<hi){int mid=lo+(hi-lo)/2;double cumulative=cdf(mid,p>.5,interrupted,context);if(!std::isfinite(cumulative))return NAN;bool reached=p>.5?cumulative<=1-p:cumulative>=p;if(reached)hi=mid;else lo=mid+1;}return lo;}
  double lo=-40,hi=40;for(int i=0;i<100;++i){if(interrupted&&interrupted(context))return NAN;double mid=(lo+hi)/2;bool reached=p>.5?.5*erfc(mid/sqrt(2.))<=1-p:.5*erfc(-mid/sqrt(2.))>=p;if(reached)hi=mid;else lo=mid;}return first+second*(lo+hi)/2;
 }
};
}
struct pico_region_fill {
 int relation[3]={0,0,0};bool density=false;double first=0,last=0;
 bool contains(double x,double y,const double *curves)const{
  if(density&&(x<first||x>last||y<0))return false;
  for(int c=0;c<3;++c)if(relation[c]){if(!std::isfinite(curves[c]))return false;double d=y-curves[c];if(relation[c]>0?(abs(relation[c])==2?d<=0:d<0):(abs(relation[c])==2?d>=0:d>0))return false;}return true;
 }
};
struct pico_math_lab {
 bool dirty=false;unsigned dimensions=3,angle=1;char vectors[2][3][32]={{"1","0","0"},{"0","1","0"}};
 char scalar[32]="2";pico_probability::distribution probability;double at=0,lower=-1,upper=1,percentile=.95;
 char region[3][160]={"X^2","X+2","0"};int relation[3]={1,-1,0};
 char function[160]="X^2+Y^2",other[160]="X*Y",algebra[160]="(X+1)^2",replacement[160]="2";char variable[2]="X";
 double x=1,y=1,window[4]={-4,4,-3,5},bounds[4]={0,1,0,1},surface_window[4]={-3,3,-3,3};
 static constexpr size_t bytes=1792;
 static uint32_t checksum(const uint8_t *p,size_t n){uint32_t c=~0u;while(n--){c^=*p++;for(int bit=0;bit<8;++bit)c=(c>>1)^(0xedb88320u&uint32_t(-int(c&1)));}return ~c;}
 bool valid()const{
  if(angle>3||dimensions<2||dimensions>3||!probability.valid()||!std::isfinite(percentile)||percentile<=0||percentile>=1||!pico_graph_scene::bounds(window)||!pico_graph_scene::bounds(bounds)||!pico_graph_scene::bounds(surface_window)||strnlen(variable,2)!=1||!strchr("XYZABC",variable[0]))return false;
  for(double v:{at,lower,upper,x,y})if(!std::isfinite(v)||fabs(v)>1e100)return false;if(lower>upper)return false;
  if(!*scalar||strnlen(scalar,32)>=32||!pico_text_editor::valid_utf8(scalar,strlen(scalar)))return false;for(const char *p=scalar;*p;++p)if((unsigned char)*p<32||*p==127||*p=='\'')return false;
  for(auto &v:vectors)for(auto &s:v){size_t n=strnlen(s,32);if(!n||n>=32||!pico_text_editor::valid_utf8(s,n))return false;for(size_t i=0;i<n;++i)if((unsigned char)s[i]<32||s[i]==127||s[i]=='\'')return false;}
  for(int i=0;i<3;++i)if(abs(relation[i])>2||!pico_graph_scene::formula(region[i],false))return false;
  return pico_graph_scene::formula(function,false)&&pico_graph_scene::formula(other,false)&&pico_graph_scene::formula(algebra,false)&&pico_graph_scene::formula(replacement,false);
 }
 size_t encode(uint8_t *out)const{
  if(!valid())return 0;memset(out,0,bytes);memcpy(out,"LUMALAB",7);out[7]=1;out[8]=dimensions;out[9]=probability.kind;out[10]=variable[0];out[14]=angle;for(int i=0;i<3;++i)out[11+i]=relation[i]+2;
  size_t at=16;auto copy=[&](const void *p,size_t n){memcpy(out+at,p,n);at+=n;};copy(vectors,sizeof vectors);copy(scalar,32);copy(region,sizeof region);copy(function,160);copy(other,160);copy(algebra,160);copy(replacement,160);
  double values[]={probability.first,probability.second,this->at,lower,upper,percentile,x,y,window[0],window[1],window[2],window[3],bounds[0],bounds[1],bounds[2],bounds[3],surface_window[0],surface_window[1],surface_window[2],surface_window[3]};
  for(double v:values){uint64_t bits;memcpy(&bits,&v,8);for(int i=0;i<8;++i)out[at++]=bits>>(i*8);}uint32_t crc=checksum(out,bytes-4);for(int i=0;i<4;++i)out[bytes-4+i]=crc>>(i*8);return bytes;
 }
 bool decode(const uint8_t *in,size_t n){
  if(n!=bytes||memcmp(in,"LUMALAB",7)||in[7]!=1||in[15])return false;uint32_t crc=0;for(int i=0;i<4;++i)crc|=uint32_t(in[bytes-4+i])<<(i*8);if(crc!=checksum(in,bytes-4))return false;
  pico_math_lab next;next.dimensions=in[8];next.angle=in[14];next.probability.kind=in[9];next.variable[0]=in[10];for(int i=0;i<3;++i)next.relation[i]=int(in[11+i])-2;
  size_t at=16;auto copy=[&](void *p,size_t n){memcpy(p,in+at,n);at+=n;};copy(next.vectors,sizeof vectors);copy(next.scalar,32);copy(next.region,sizeof region);copy(next.function,160);copy(next.other,160);copy(next.algebra,160);copy(next.replacement,160);
  double values[20];for(double &v:values){uint64_t bits=0;for(int i=0;i<8;++i)bits|=uint64_t(in[at++])<<(i*8);memcpy(&v,&bits,8);}next.probability.first=values[0];next.probability.second=values[1];next.at=values[2];next.lower=values[3];next.upper=values[4];next.percentile=values[5];next.x=values[6];next.y=values[7];memcpy(next.window,values+8,32);memcpy(next.bounds,values+12,32);memcpy(next.surface_window,values+16,32);
  if(!next.valid())return false;next.dirty=false;*this=next;return true;
 }
};
