#include "geometry_math.h"
#include "finance_extended.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>
static bool cancel(){return true;}
int main()
{
 double value,p;assert(pico_finance::payment(10000,5,60,12,false,p));assert(pico_finance::term(10000,p,5,12,false,value)==pico_finance::outcome::ok&&fabs(value-60)<1e-9);assert(pico_finance::rate(10000,p,60,12,false,value)==pico_finance::outcome::ok&&fabs(value-5)<1e-7);
 assert(pico_finance::rate(1e-20,p*1e-24,60,12,false,value)==pico_finance::outcome::ok&&fabs(value-5)<1e-7);
 assert(pico_finance::term(8000,150,7,12,false,value)==pico_finance::outcome::ok&&fabs(value-64.07334877066)<1e-8);
 assert(pico_finance::term(10000,1,5,12,false,value)==pico_finance::outcome::no_solution);
 assert(pico_finance::rate(100,100,1,12,true,value)==pico_finance::outcome::indeterminate);assert(pico_finance::rate(10000,p,60,12,false,value,cancel)==pico_finance::outcome::cancelled);
 assert(pico_finance::savings(1000,100,0,12,12,false,value)&&value==2200);assert(pico_finance::savings(1000,0,5,12,12,true,value)&&fabs(value-1051.161897881733)<1e-8);
 double flows[]={-100,230,-132};auto roots=pico_finance::irr(flows,3,0,100);assert(roots.state==pico_finance::outcome::ok&&roots.count==2&&fabs(roots.roots[0]-10)<1e-6&&fabs(roots.roots[1]-20)<1e-6);roots=pico_finance::irr(flows,3,11,19);assert(roots.state==pico_finance::outcome::no_solution);roots=pico_finance::irr(flows,3,0,100,cancel);assert(roots.state==pico_finance::outcome::cancelled);double zero[]={0,0};assert(pico_finance::irr(zero,2,0,100).state==pico_finance::outcome::indeterminate);
 double a[]={-1,0},b[]={1,0},c[]={0,-1},d[]={0,1},out[2],hits[2][2],center[]={0,0};assert(pico_geometry::intersection(a,b,c,d,out)==pico_geometry::relation::unique&&out[0]==0&&out[1]==0);assert(pico_geometry::line_circle(a,b,center,1,hits)==2&&fabs(hits[0][0]-1)<1e-12&&fabs(hits[1][0]+1)<1e-12);assert(pico_geometry::intersection(a,b,a,b,out)==pico_geometry::relation::coincident);assert(pico_geometry::intersection(a,a,c,d,out)==pico_geometry::relation::degenerate);double e[]={-1,1},f[]={1,1};assert(pico_geometry::intersection(a,b,e,f,out)==pico_geometry::relation::parallel);assert(pico_geometry::line_circle(e,f,center,1,hits)==1);double window[]={-2,2,-2,2};assert(pico_geometry::clip_line(c,d,window,hits)&&hits[0][1]==-2&&hits[1][1]==2);
 pico_complex_document complex,next_complex;uint8_t cb[pico_complex_document::bytes];assert(complex.encode(cb)==sizeof cb&&next_complex.decode(cb,sizeof cb));for(size_t i=0;i<sizeof cb;++i){cb[i]^=1;assert(!next_complex.decode(cb,sizeof cb));cb[i]^=1;}
 pico_geometry_document geometry,next_geometry;uint8_t gb[pico_geometry_document::bytes];assert(geometry.encode(gb)==sizeof gb&&next_geometry.decode(gb,sizeof gb));for(size_t i=0;i<sizeof gb;++i){gb[i]^=1;assert(!next_geometry.decode(gb,sizeof gb));gb[i]^=1;}
 pico_finance_document finance,next_finance;uint8_t fb[pico_finance_document::bytes];finance.deposit=123;assert(finance.encode(fb)==sizeof fb&&next_finance.decode(fb,sizeof fb)&&next_finance.deposit==123);FILE *file=fopen("resources/finance/LOAN.finance","rb");assert(file);size_t n=fread(fb,1,sizeof fb,file);fclose(file);assert(n==192&&next_finance.decode(fb,n)&&next_finance.principal==10000&&next_finance.deposit==100);
 puts("PASS: rate/term/savings expected values, multiple and missing IRRs, cancellation, geometric relations/tangency/clipping, document corruption rejection and legacy finance compatibility");
}
