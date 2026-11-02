#pragma once
#include "finance_math.h"
namespace pico_finance {
enum class outcome {ok,invalid,no_solution,indeterminate,cancelled};
inline bool savings(double principal,double deposit,double annual,double periods,double frequency,bool beginning,double &out)
{
 if(!inputs(principal,annual,periods,frequency)||!std::isfinite(deposit)||deposit<0)return false;
 double base;if(!growth(principal,annual,periods,frequency,base))return false;
 double r=annual/(100*frequency),factor=r==0?periods:std::expm1(periods*std::log1p(r))/r;
 out=deposit==0?base:base+deposit*factor*(beginning?1+r:1);return std::isfinite(out);
}
inline outcome term(double principal,double repayment,double annual,double frequency,bool beginning,double &out)
{
 if(!inputs(principal,annual,1,frequency)||!std::isfinite(repayment)||repayment<=0)return outcome::invalid;
 if(principal==0){out=0;return outcome::ok;}
 double r=annual/(100*frequency),p=repayment*(beginning?1+r:1),part=principal*r/p;
 if(part>=1)return outcome::no_solution;
 out=r==0?principal/p:-std::log1p(-part)/std::log1p(r);
 return std::isfinite(out)&&out>=0&&out<=100000?outcome::ok:outcome::no_solution;
}
inline outcome rate(double principal,double repayment,double periods,double frequency,bool beginning,double &out,bool (*cancel)()=nullptr)
{
 if(!inputs(principal,0,periods,frequency)||!std::isfinite(repayment)||repayment<0)return outcome::invalid;
 if(principal==0)return repayment==0?outcome::indeterminate:outcome::no_solution;
 if(repayment==0)return outcome::no_solution;
 if(beginning&&periods==1)return std::fabs(principal-repayment)<=1e-11*std::fmax(principal,repayment)?outcome::indeterminate:outcome::no_solution;
 double lo=-99.999999,hi=10000,l,h;
 if(!payment(principal,lo,periods,frequency,beginning,l)||!payment(principal,hi,periods,frequency,beginning,h)||repayment<l||repayment>h)return outcome::no_solution;
 for(unsigned i=0;i<200;++i){if(cancel&&cancel())return outcome::cancelled;out=(lo+hi)/2;double p;if(!payment(principal,out,periods,frequency,beginning,p))return outcome::invalid;if(std::fabs(p-repayment)<=1e-11*std::fmax(p,repayment))return outcome::ok;if(p<repayment)lo=out;else hi=out;}
 return outcome::no_solution;
}
// Positive scaling avoids discount overflow near -100%; roots are unchanged.
inline bool irr_value(const double *flows,unsigned count,double u,double &sum)
{
 double scale=-INFINITY;for(unsigned i=0;i<count;++i){if(!std::isfinite(flows[i]))return false;if(flows[i])scale=std::fmax(scale,std::log(std::fabs(flows[i]))-i*u);}
 if(!std::isfinite(scale)){sum=0;return true;}sum=0;double weight=0;
 for(unsigned i=0;i<count;++i)if(flows[i]){double v=std::exp(std::log(std::fabs(flows[i]))-i*u-scale);sum+=std::copysign(v,flows[i]);weight+=v;}
 sum/=std::fmax(1.,weight);return std::isfinite(sum);
}
struct irr_result {outcome state=outcome::invalid;double roots[16]{};unsigned count=0,evaluations=0;};
inline irr_result irr(const double *flows,unsigned count,double low,double high,bool (*cancel)()=nullptr)
{
 irr_result result;if(!flows||count<1||count>17||!std::isfinite(low)||!std::isfinite(high)||low<=-100||high>10000||low>=high)return result;
 bool positive=false,negative=false;for(unsigned i=0;i<count;++i){if(!std::isfinite(flows[i]))return result;positive|=flows[i]>0;negative|=flows[i]<0;}
 if(!positive&&!negative){result.state=outcome::indeterminate;return result;}result.state=outcome::no_solution;if(!positive||!negative)return result;
 auto evaluate=[&](double u,double &v){if((result.evaluations++&7)==0&&cancel&&cancel()){result.state=outcome::cancelled;return false;}return result.evaluations<=8192&&irr_value(flows,count,u,v);};
 auto add=[&](double u){double r=100*std::expm1(u);for(unsigned i=0;i<result.count;++i)if(std::fabs(result.roots[i]-r)<1e-6*std::fmax(1.,std::fabs(r)))return;if(result.count<16)result.roots[result.count++]=r;};
 double first=std::log1p(low/100),last=std::log1p(high/100),a=first,fa;if(!evaluate(a,fa))return result;if(std::fabs(fa)<1e-12)add(a);
 for(unsigned i=1;i<=1024;++i){double b=first+(last-first)*i/1024,fb;if(!evaluate(b,fb)){if(result.state==outcome::cancelled)result.count=0;return result;}if(std::fabs(fb)<1e-12)add(b);
  if(std::signbit(fa)!=std::signbit(fb)&&fa!=0&&fb!=0){double lo=a,hi=b,flo=fa,mid=0,fm=0;for(unsigned k=0;k<200;++k){mid=(lo+hi)/2;if(!evaluate(mid,fm)){if(result.state==outcome::cancelled)result.count=0;return result;}if(std::fabs(fm)<1e-12||hi-lo<1e-14)break;if(std::signbit(flo)!=std::signbit(fm))hi=mid;else{lo=mid;flo=fm;}}if(std::fabs(fm)<=1e-8)add(mid);}
  a=b;fa=fb;
 }
 result.state=result.count?outcome::ok:outcome::no_solution;return result;
}
}
