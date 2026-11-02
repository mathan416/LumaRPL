#pragma once
#include <cmath>
// Nominal annual percentage rate, m equal payment/compounding periods per year.
// Payments below are positive magnitudes; cash-flow NPV uses signed amounts.
namespace pico_finance {
inline bool inputs(double principal,double annual,double periods,double frequency)
{return std::isfinite(principal)&&principal>=0&&std::isfinite(annual)&&annual>-100&&annual<=10000&&std::isfinite(periods)&&periods>=1&&periods<=100000&&periods==std::floor(periods)&&frequency>=1&&frequency<=366&&frequency==std::floor(frequency);}
inline bool payment(double principal,double annual,double periods,double frequency,bool beginning,double &out)
{
 if(!inputs(principal,annual,periods,frequency))return false;
 double r=annual/(100*frequency),den=-std::expm1(-periods*std::log1p(r));
 out=r==0?principal/periods:principal*r/den;if(beginning)out/=1+r;
 return std::isfinite(out)&&out>=0;
}
inline bool growth(double principal,double annual,double periods,double frequency,double &out)
{
 if(!inputs(principal,annual,periods,frequency))return false;
 if(principal==0){out=0;return true;}
 out=principal*std::exp(periods*std::log1p(annual/(100*frequency)));return std::isfinite(out);
}
inline bool amortize(double principal,double annual,double periods,double frequency,bool beginning,unsigned count,double &balance,double &interest,double &paid_principal,bool (*cancel)()=nullptr)
{
 double p;if(!payment(principal,annual,periods,frequency,beginning,p)||count>periods)return false;
 balance=principal;interest=paid_principal=0;double rate=annual/(100*frequency);
 for(unsigned i=0;i<count;++i){if((i&63)==0&&cancel&&cancel())return false;double fee=beginning&&i==0?0:balance*rate;interest+=fee;balance+=fee-p;}
 paid_principal=principal-balance;return std::isfinite(balance)&&std::isfinite(interest);
}
inline bool npv(const double *flows,unsigned count,double percent,double &out)
{
 if(!flows||!count||count>17||!std::isfinite(percent)||percent<=-100||percent>10000)return false;
 double sum=0,correction=0,factor=1,rate=1+percent/100;
 for(unsigned i=0;i<count;++i){if(!std::isfinite(flows[i]))return false;double term=flows[i]/factor-correction,next=sum+term;correction=(next-sum)-term;sum=next;factor*=rate;}
 out=sum;return std::isfinite(sum);
}
}
