#pragma once
#include "math_lab.h"
#include "plot_engine.h"
namespace pico_double_integral {
using sample=bool(*)(void *,double,double,double &);using poll=bool(*)(void *);
struct result {double value=NAN,error=NAN;unsigned evaluations=0;int status=0;}; // 1 estimate, 0 domain, -1 cancellation, -2 no convergence
inline result integrate(const double *bounds,sample evaluate,void *context,poll interrupted=nullptr,void *cancel=nullptr){
 result r;if(!pico_graph_scene::bounds(bounds))return r;double previous=NAN;
 for(unsigned n:{16u,32u,64u}){double sum=0,correction=0;
  for(unsigned j=0;j<=n;++j)for(unsigned i=0;i<=n;++i){if(interrupted&&interrupted(cancel)){r.status=-1;return r;}double f,x=pico_plot_engine::coordinate(bounds[0],bounds[1],i,n),y=pico_plot_engine::coordinate(bounds[2],bounds[3],j,n);++r.evaluations;if(!evaluate(context,x,y,f)||!std::isfinite(f))return r;double term=f*(i==0||i==n?1:i%2?4:2)*(j==0||j==n?1:j%2?4:2)-correction;double next=sum+term;correction=(next-sum)-term;sum=next;}
  r.value=sum*((bounds[1]-bounds[0])/n)*((bounds[3]-bounds[2])/n)/9;if(!std::isfinite(r.value))return r;
  if(std::isfinite(previous)){r.error=fabs(r.value-previous)/15;if(r.error<=fmax(1e-8,1e-7*fabs(r.value))){r.status=1;return r;}}previous=r.value;
 }r.status=-2;return r;
}
}
