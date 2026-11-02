#include "math_lab_numeric.h"
#include <cassert>
#include <cstdio>
#include <limits>
static void near(double a,double b,double tol=1e-11){if(!std::isfinite(a)||fabs(a-b)>tol*fmax(fabs(b),1e-100)){fprintf(stderr,"Mismatch %.17g expected %.17g\n",a,b);assert(false);}}
static bool polynomial(void *,double x,double y,double &v){v=x*x+y*y;return true;}
static bool constant(void *,double,double,double &v){v=1;return true;}
static bool nonsmooth(void *,double x,double y,double &v){v=exp(100*x+y);return true;}
static bool hole(void *,double x,double,double &v){v=sqrt(x-.5);return std::isfinite(v);}
static bool cancel(void *p){return ++*static_cast<unsigned *>(p)>10;}
int main(){
 pico_probability::distribution d;near(d.cdf(0),.5);near(d.mass(0),.3989422804014327);near(d.interval(-1,1),.6826894921370859);near(d.cdf(8,true),6.220960574271784e-16);near(d.interval(8,9),6.21983198586583e-16);near(d.quantile(.975),1.959963984540054);near(d.interval(0,1e-20),.3989422804014327e-20);assert(d.quantile(.5)==0);assert(std::isnan(d.quantile(0))&&std::isnan(d.quantile(1)));
 d.kind=1;d.first=10;d.second=.5;near(d.mass(5),.24609375);near(d.cdf(4),.376953125);near(d.cdf(4,true),.623046875);near(d.interval(2,4),.3662109375);near(d.quantile(.5),5);assert(d.mass(3.5)==0);d.second=0;assert(d.mass(0)==1&&d.cdf(0)==1&&d.quantile(.9)==0);d.second=1;assert(d.mass(10)==1&&d.cdf(9)==0&&d.quantile(.9)==10);d.first=0;assert(d.mass(0)==1&&d.quantile(.99)==0);
 d.first=2000;d.second=.5;near(d.cdf(999)+d.cdf(999,true),1);d.kind=2;d.first=3;near(d.mass(0),.04978706836786394);near(d.cdf(2),.4231900811268435);near(d.cdf(2,true),.5768099188731565);near(d.quantile(.95),6);d.first=1000;near(d.cdf(999),.4957947558197845);near(d.cdf(999)+d.cdf(999,true),1);for(double p:{.001,.5,.99,.99999}){double q=d.quantile(p);assert(d.cdf(q)>=p-1e-11&&d.cdf(q-1)<p+1e-11);}
 unsigned cancelled_calls=0;assert(std::isnan(d.quantile(.9,cancel,&cancelled_calls)));d.first=1001;assert(!d.valid());d.first=-1;assert(!d.valid());d.kind=1;d.first=1.5;assert(!d.valid());
 pico_region_fill fill;fill.relation[0]=1;fill.relation[1]=-1;double values[]={1,3,NAN};assert(fill.contains(0,1,values)&&fill.contains(0,2,values)&&!fill.contains(0,4,values));fill.relation[0]=2;assert(!fill.contains(0,1,values));fill.density=true;fill.first=0;fill.last=1;assert(!fill.contains(2,2,values));values[0]=NAN;assert(!fill.contains(0,2,values));
 pico_math_lab original,loaded;uint8_t wire[pico_math_lab::bytes];assert(original.valid()&&original.encode(wire)==sizeof wire&&loaded.decode(wire,sizeof wire));uint8_t again[sizeof wire];assert(loaded.encode(again)&&!memcmp(wire,again,sizeof wire));wire[100]^=1;assert(!loaded.decode(wire,sizeof wire));assert(original.encode(wire));wire[8]=4;uint32_t crc=pico_math_lab::checksum(wire,sizeof wire-4);for(int i=0;i<4;++i)wire[sizeof wire-4+i]=crc>>(i*8);assert(!loaded.decode(wire,sizeof wire)&&loaded.dimensions==3);assert(!loaded.decode(wire,sizeof wire-1));
 double bounds[]={0,1,0,1};auto result=pico_double_integral::integrate(bounds,polynomial,nullptr);assert(result.status==1&&result.evaluations<=5603);near(result.value,2./3);bounds[0]=-2;bounds[1]=2;bounds[2]=-3;bounds[3]=3;result=pico_double_integral::integrate(bounds,constant,nullptr);assert(result.status==1);near(result.value,24);unsigned calls=0;result=pico_double_integral::integrate(bounds,constant,nullptr,cancel,&calls);assert(result.status==-1&&result.evaluations==10);double unit[]={0,1,0,1};result=pico_double_integral::integrate(unit,hole,nullptr);assert(result.status==0);result=pico_double_integral::integrate(unit,nonsmooth,nullptr);assert(result.status==-2&&result.evaluations==5603);
 pico_math_lab invalid_quantile;invalid_quantile.percentile=NAN;assert(!invalid_quantile.valid());
 puts("PASS: probability values/tails/quantiles/bounds; region membership; versioned CRC document validation; double-integral accuracy/domain/cancellation/refinement budget");
}
