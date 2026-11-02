#pragma once
#include "exploration_documents.h"
#include <algorithm>
namespace pico_geometry {
enum class relation {unique,parallel,coincident,degenerate};
// Compensate both products: a single FMA would leave the rounding error of
// the other product and turn an exactly collinear diagonal into an offset.
inline double cross_product(double ax,double ay,double bx,double by)
{double product=ay*bx;return std::fma(ax,by,-product)+std::fma(-ay,bx,product);}
inline double distance(const double *a,const double *b){return std::hypot(b[0]-a[0],b[1]-a[1]);}
inline relation intersection(const double *a,const double *b,const double *c,const double *d,double *out)
{
 double ux=b[0]-a[0],uy=b[1]-a[1],vx=d[0]-c[0],vy=d[1]-c[1],un=std::hypot(ux,uy),vn=std::hypot(vx,vy);
 if(un==0||vn==0||!std::isfinite(un)||!std::isfinite(vn))return relation::degenerate;
 ux/=un;uy/=un;vx/=vn;vy/=vn;double cross=cross_product(ux,uy,vx,vy),dx=c[0]-a[0],dy=c[1]-a[1];
 if(std::fabs(cross)<1e-12){double separation=std::hypot(dx,dy);return separation==0||std::fabs(cross_product(dx/separation,dy/separation,ux,uy))<=1e-12?relation::coincident:relation::parallel;}
 double t=cross_product(dx,dy,vx,vy)/cross;out[0]=std::fma(t,ux,a[0]);out[1]=std::fma(t,uy,a[1]);return std::isfinite(out[0])&&std::isfinite(out[1])?relation::unique:relation::degenerate;
}
inline int line_circle(const double *a,const double *b,const double *center,double radius,double out[2][2])
{
 double n=distance(a,b);if(n==0||!std::isfinite(n)||!std::isfinite(radius)||radius<0)return -1;
 double ux=(b[0]-a[0])/n,uy=(b[1]-a[1])/n,dx=center[0]-a[0],dy=center[1]-a[1];
 double normal=cross_product(dx,dy,ux,uy),distance=std::fabs(normal);
 if(!std::isfinite(distance))return -1;
 if(radius==0){double separation=std::hypot(dx,dy);if(separation&&std::fabs(cross_product(dx/separation,dy/separation,ux,uy))>1e-12)return 0;for(int i=0;i<2;++i){out[i][0]=center[0];out[i][1]=center[1];}return 1;}
 // Compare distances in the same units; avoid a unit-sized tolerance floor
 // and squaring tiny/large radii. Projection from the center avoids losing
 // a small intersection offset when the line's anchor is far away.
 double tolerance=1e-12*std::fmax(radius,distance);
 if(distance>radius&&distance-radius>tolerance)return 0;
 bool tangent=std::fabs(distance-radius)<=tolerance;
 double ratio=std::fmin(1.,distance/radius),h=tangent?0:radius*std::sqrt((1-ratio)*(1+ratio));
 double foot[2]={std::fma(-normal,uy,center[0]),std::fma(normal,ux,center[1])};
 for(int i=0;i<2;++i){double offset=i?-h:h;out[i][0]=std::fma(offset,ux,foot[0]);out[i][1]=std::fma(offset,uy,foot[1]);}return tangent?1:2;
}
inline bool clip_line(const double *a,const double *b,const double *window,double out[2][2])
{
 double low=-INFINITY,high=INFINITY;for(int j=0;j<2;++j){double v=b[j]-a[j];if(v==0){if(a[j]<window[2*j]||a[j]>window[2*j+1])return false;}else{double p=(window[2*j]-a[j])/v,q=(window[2*j+1]-a[j])/v;if(p>q)std::swap(p,q);low=std::fmax(low,p);high=std::fmin(high,q);}}
 if(!std::isfinite(low)||!std::isfinite(high)||low>high)return false;for(int i=0;i<2;++i)for(int j=0;j<2;++j)out[i][j]=std::fma(i?high:low,b[j]-a[j],a[j]);return true;
}
}
