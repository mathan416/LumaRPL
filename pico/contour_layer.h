#pragma once
#include "research_engine.h"
#include "bulk_storage.h"
struct pico_contour_segment {float x1,y1,x2,y2;uint32_t level;};
struct pico_contour_layer {
 pico_research::field field;void *context;const double *levels;unsigned count,used=0;int status=0;pico_research::poll halt=nullptr;
 pico_bulk_records<pico_contour_segment,1536> segments{false};
 int rebuild(const double *bounds,double *scratch,pico_research::poll stop){used=0;if(!segments.reset_append())return status=-1;struct collector{pico_contour_layer *self;const double *bounds;bool full=false;} c{this,bounds};
 auto emit=[](void *p,double x,double y,double xx,double yy,unsigned level){auto &c=*static_cast<collector *>(p);auto &s=*c.self;if(s.used>=1536){c.full=true;return false;}pico_contour_segment value{float(pico_wireframe::normalize(x,c.bounds[0],c.bounds[1])),float(pico_wireframe::normalize(y,c.bounds[2],c.bounds[3])),float(pico_wireframe::normalize(xx,c.bounds[0],c.bounds[1])),float(pico_wireframe::normalize(yy,c.bounds[2],c.bounds[3])),level};return s.segments.put(s.used++,value);};
 auto *work=new(scratch) pico_research::contour_work;int r=pico_research::contours(bounds,levels,count,field,context,emit,&c,*work,halt?halt:stop);work->~contour_work();for(unsigned i=0;i<900;++i)scratch[i]=NAN;if(r<0||!segments.good()){used=0;return status=c.full?-3:r<0?r:-1;}return status=1;}
};
