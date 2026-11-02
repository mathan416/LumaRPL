#include "geometry_math.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>
using namespace pico_geometry;
int main()
{
 for(double scale:{1e-200,1e-13,1e-8,1.,1e50,1e200}){
  double a[]={-4*scale,0},b[]={4*scale,0},c[]={0,-4*scale},d[]={0,4*scale},center[]={0,0},hits[2][2],out[2];
  assert(intersection(a,b,c,d,out)==relation::unique&&out[0]==0&&out[1]==0);
  assert(intersection(a,b,a,b,out)==relation::coincident);
  c[0]=-4*scale;d[0]=4*scale;c[1]=d[1]=scale;assert(intersection(a,b,c,d,out)==relation::parallel);
  assert(line_circle(a,b,center,scale,hits)==2);assert(fabs(hits[0][0]/scale-1)<1e-12&&fabs(hits[1][0]/scale+1)<1e-12);
  a[1]=b[1]=scale;assert(line_circle(a,b,center,scale,hits)==1&&hits[0][0]==0&&fabs(hits[0][1]/scale-1)<1e-12);
  a[1]=b[1]=10*scale;assert(line_circle(a,b,center,scale,hits)==0);
  assert(line_circle(a,b,center,0,hits)==0);a[1]=b[1]=0;assert(line_circle(a,b,center,0,hits)==1&&hits[0][0]==0&&hits[0][1]==0);
  assert(line_circle(a,a,center,scale,hits)==-1);
 }
 double a[]={-1e-6,1e-7},b[]={1e-6,1e-7},c[]={0,0},hits[2][2];assert(line_circle(a,b,c,1e-8,hits)==0);
 a[1]=b[1]=0;assert(line_circle(a,b,c,1e-13,hits)==2);
 // Small roots remain accurate when the anchor is much farther along the line.
 a[0]=-1e50;b[0]=1e50;assert(line_circle(a,b,c,1e-13,hits)==2&&fabs(hits[0][0]/1e-13-1)<1e-12);
 a[0]=a[1]=-1e50;b[0]=b[1]=1e50;assert(line_circle(a,b,c,1e-13,hits)==2);assert(fabs(hits[0][0]/1e-13-1/std::sqrt(2.))<1e-12&&hits[0][0]==hits[0][1]);assert(line_circle(a,b,c,0,hits)==1);
 double center[]={1e15,-1e15},ta[]={1e15-4,-1e15+2},tb[]={1e15+4,-1e15+2};assert(line_circle(ta,tb,center,2,hits)==1&&hits[0][0]==center[0]&&hits[0][1]==center[1]+2);
 puts("PASS: scale-invariant intersections, disjoint/tangent/two-hit/point circles, degeneracy and translated/far-anchor geometry from 1e-200 to 1e200");
}
