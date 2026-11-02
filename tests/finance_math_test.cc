#include "finance_math.h"
#include "math_documents.h"
#include <cassert>
#include <initializer_list>
#include <cstdio>
#include <limits>
static bool cancelled(){return true;}
int main(){double p,f,b,i,a;
 assert(pico_finance::payment(100000,5,360,12,false,p)&&fabs(p-536.8216230121399)<1e-9);
 assert(pico_finance::payment(12000,0,12,12,false,p)&&p==1000);
 assert(pico_finance::payment(12000,1e-10,12,12,false,p)&&fabs(p-1000)<1e-7);
 assert(pico_finance::growth(1000,5,10,1,f)&&fabs(f-1628.894626777442)<1e-9);
 assert(pico_finance::payment(10000,5,60,12,false,p)&&pico_finance::payment(10000,5,60,12,true,f)&&fabs(f*(1+0.05/12)-p)<1e-10);
 for(bool begin:{false,true}){assert(pico_finance::amortize(10000,5,60,12,begin,60,b,i,a)&&fabs(b)<1e-8&&fabs(a-10000)<1e-8);assert(pico_finance::amortize(10000,5,60,12,begin,0,b,i,a)&&b==10000&&i==0&&a==0);}
 assert(pico_finance::growth(0,10000,100000,1,p)&&p==0);assert(!pico_finance::amortize(10000,5,60,12,false,60,b,i,a,cancelled));
 double cash[]={-1000,400,400,400};assert(pico_finance::npv(cash,4,5,p)&&fabs(p-89.29921174819099)<1e-9);assert(pico_finance::npv(cash,4,0,p)&&p==200);
 assert(!pico_finance::payment(1,-100,12,12,false,p));assert(!pico_finance::payment(1,1,1.5,12,false,p));assert(!pico_finance::payment(1,1,12,0,false,p));assert(!pico_finance::growth(1,10000,100000,1,p));assert(!pico_finance::npv(cash,4,-100,p));cash[0]=std::numeric_limits<double>::infinity();assert(!pico_finance::npv(cash,4,5,p));
 pico_matrix_document matrix,loaded;uint8_t data[pico_matrix_document::bytes];assert(matrix.encode(data)==sizeof data&&loaded.decode(data,sizeof data));for(size_t n=0;n<sizeof data;++n){data[n]^=1;assert(!loaded.decode(data,sizeof data));data[n]^=1;}assert(!loaded.decode(data,sizeof data-1));
 pico_finance_document finance,restored;uint8_t bytes[pico_finance_document::bytes];assert(finance.encode(bytes)==sizeof bytes&&restored.decode(bytes,sizeof bytes)&&restored.principal==10000);for(size_t n=0;n<sizeof bytes;++n){bytes[n]^=1;assert(!restored.decode(bytes,sizeof bytes));bytes[n]^=1;}assert(!restored.decode(bytes,sizeof bytes-1));
 puts("PASS: finance independent expected payments/growth/NPV, zero/tiny/negative/domain rates, beginning/end amortization, overflow, bounded versioned matrix/finance records and every-byte corruption rejection");}
