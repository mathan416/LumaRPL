#pragma once
#include "math_lab.h"
#include "complex_engine.h"
#include "plot_engine.h"
inline object::id pico_lab_angle(unsigned n){return n==0?object::ID_Deg:n==1?object::ID_Rad:n==2?object::ID_Grad:object::ID_PiRadians;}
inline bool pico_lab_real(algebraic_g value,double &number){settings::SaveNumericalResults numerical(true);settings::SaveNumericalConstants constants(true);if(!value||!algebraic::to_decimal(value))return false;return pico_workbench_double(value,number);}
struct pico_lab_vector_context {const pico_math_lab *doc;unsigned which;algebraic_g scale;unit_g units;};
inline object_p pico_lab_vector_item(size_t,size_t,size_t row,size_t,void *opaque){auto &c=*static_cast<pico_lab_vector_context *>(opaque);algebraic_g v=pico_quantity_parse(c.doc->vectors[c.which][row]);if(!v)return nullptr;if(c.units){if(!unit::get(v)){algebraic_g dimension=c.units->uexpr();v=unit::make(v,dimension);}if(!v||!c.units->convert(v))return nullptr;}return c.scale?object_p(+(v*c.scale)):object_p(+v);}
inline algebraic_p pico_lab_vector_component(pico_lab_vector_context &c,unsigned row){object_g value=pico_lab_vector_item(0,0,row,0,&c);return value?value->as_algebraic():nullptr;}
// Guided vector fields are rectangular components, including physical quantities.
// Upstream array dot/add/cross also infer polar vectors from angle-unit entries.
inline algebraic_p pico_lab_rect_dot(array_r a,array_r b,unsigned count){algebraic_g sum;for(unsigned i=0;i<count;++i){object_g av=a->at(i),bv=b->at(i);algebraic_g x=av?av->as_algebraic():nullptr,y=bv?bv->as_algebraic():nullptr;if(!x||!y)return nullptr;algebraic_g term=x*y;if(!term)return nullptr;sum=sum?sum+term:term;if(!sum)return nullptr;}return sum;}
struct pico_lab_rect_context {array_g a,b;unsigned action;};
inline object_p pico_lab_rect_item(size_t,size_t,size_t row,size_t,void *p){auto &c=*static_cast<pico_lab_rect_context *>(p);if(c.action<2){object_g av=c.a->at(row),bv=c.b->at(row);algebraic_g a=av?av->as_algebraic():nullptr,b=bv?bv->as_algebraic():nullptr;return a&&b?object_p(c.action?+(a-b):+(a+b)):nullptr;}unsigned i=(row+1)%3,j=(row+2)%3;object_g ai=c.a->at(i),aj=c.a->at(j),bi=c.b->at(i),bj=c.b->at(j);algebraic_g x=ai?ai->as_algebraic():nullptr,y=aj?aj->as_algebraic():nullptr,z=bi?bi->as_algebraic():nullptr,w=bj?bj->as_algebraic():nullptr;if(!x||!y||!z||!w)return nullptr;algebraic_g first=x*w,last=y*z;return first&&last?object_p(+(first-last)):nullptr;}
inline array_p pico_lab_rect_array(array_r a,array_r b,unsigned count,unsigned action){pico_lab_rect_context context{a,b,action};return array::build(count,0,pico_lab_rect_item,&context);}
inline object_p pico_lab_vector(const pico_math_lab &doc,unsigned action)
{
 if(!doc.valid()||action>12){rt.value_error();return nullptr;}
 stack_depth_restore stack;object_g answer;auto requested=Settings.Precision();
 {pico_workbench_scope scope;if(!scope.entered)return nullptr;settings::SavePrecision precision(requested);settings::SaveMaxRewrites rewrites(200);settings::SaveMaxFactorIterations factors(10000);settings::SaveAngleMode angle(pico_lab_angle(doc.angle));settings::SaveHardwareFloatingPoint hardware(false);settings::SaveNumericalResults exact(false);settings::SaveNumericalConstants constants(false);
 pico_lab_vector_context ca{&doc,0},cb{&doc,1};for(unsigned which=0;which<2;++which)for(unsigned i=0;i<doc.dimensions;++i){algebraic_g v=pico_quantity_parse(doc.vectors[which][i]);if(!v)return nullptr;unit_g u=unit::get(v);if(u){if(which==0&&!ca.units)ca.units=pico_form_unit_one(u);if(which==1&&!cb.units)cb.units=pico_form_unit_one(u);}}array_g a=array::build(doc.dimensions,0,pico_lab_vector_item,&ca),b=array::build(doc.dimensions,0,pico_lab_vector_item,&cb);if(!a||!b)return nullptr;
 if(action==0)answer=object_p(+a);if(action==1)answer=object_p(+b);if(action==2)answer=pico_lab_rect_dot(a,b,doc.dimensions);
 if(action==3){if(doc.dimensions==3)answer=pico_lab_rect_array(a,b,doc.dimensions,2);else{algebraic_g a0=pico_lab_vector_component(ca,0),a1=pico_lab_vector_component(ca,1),b0=pico_lab_vector_component(cb,0),b1=pico_lab_vector_component(cb,1);if(a0&&a1&&b0&&b1)answer=object_p(+(a0*b1-a1*b0));}}
 if(action==4)answer=a->norm();if(action==5){array_g difference=pico_lab_rect_array(a,b,doc.dimensions,1);if(difference)answer=difference->norm();}
 if(action==6){algebraic_g denominator=pico_lab_rect_dot(b,b,doc.dimensions),numerator=pico_lab_rect_dot(a,b,doc.dimensions);if(!denominator||!numerator)return nullptr;if(denominator->is_zero()){rt.zero_divide_error();return nullptr;}cb.scale=numerator/denominator;if(cb.scale)answer=array::build(doc.dimensions,0,pico_lab_vector_item,&cb);}
 if(action==8)answer=pico_lab_rect_array(a,b,doc.dimensions,0);if(action==9)answer=pico_lab_rect_array(a,b,doc.dimensions,1);
 if(action>=10&&action<=12){if(action==12){algebraic_g square=pico_lab_rect_dot(a,a,doc.dimensions);if(!square)return nullptr;if(square->is_zero()){rt.zero_divide_error();return nullptr;}}algebraic_g k=action==12?a->norm():pico_quantity_parse(doc.scalar);if(!k)return nullptr;if(action>10){if(k->is_zero()){rt.zero_divide_error();return nullptr;}algebraic_g one=integer::make(1);ca.scale=one/k;}else ca.scale=k;if(ca.scale)answer=array::build(doc.dimensions,0,pico_lab_vector_item,&ca);}
 if(action==7){double av[3]{},bv[3]{},sa=0,sb=0;for(unsigned i=0;i<doc.dimensions;++i){object_g avalue=pico_lab_vector_item(0,0,i,0,&ca),bvalue=pico_lab_vector_item(0,0,i,0,&cb);algebraic_g aa=avalue?avalue->as_algebraic():nullptr,bb=bvalue?bvalue->as_algebraic():nullptr;unit_g au=unit::get(aa),bu=unit::get(bb);if(au)aa=au->value();if(bu)bb=bu->value();if(!aa||!bb||!pico_lab_real(aa,av[i])||!pico_lab_real(bb,bv[i]))return nullptr;sa=fmax(sa,fabs(av[i]));sb=fmax(sb,fabs(bv[i]));}if(sa==0||sb==0){rt.value_error();return nullptr;}double dot=0,aa=0,bb=0;for(unsigned i=0;i<doc.dimensions;++i){double x=av[i]/sa,y=bv[i]/sb;dot+=x*y;aa+=x*x;bb+=y*y;}double ratio=dot/(sqrt(aa)*sqrt(bb));answer=hwdouble::make(acos(fmax(-1.,fmin(1.,ratio)))*180/3.14159265358979323846);}

 if(answer)answer=rt.clone(answer);
 }return answer;
}
inline object_p pico_lab_algebra(const pico_math_lab &doc,unsigned action)
{
 if(!doc.valid()||action>4){rt.value_error();return nullptr;}
 stack_depth_restore stack;object_g answer;auto requested=Settings.Precision();
 {pico_workbench_scope scope;if(!scope.entered)return nullptr;settings::SavePrecision precision(requested);settings::SaveMaxRewrites rewrites(200);settings::SaveMaxFactorIterations factors(10000);settings::SaveAngleMode angle(pico_lab_angle(doc.angle));settings::SaveHardwareFloatingPoint hardware(false);settings::SaveNumericalResults exact(false);settings::SaveNumericalConstants constants(false);
 expression_g e=pico_formula_parse(doc.algebra);if(!e)return nullptr;
 if(action==0)answer=e->simplify();if(action==1)answer=e->expand();if(action==2)answer=e->collect();if(action==3){if(!rt.push(object_p(+e))||object::static_object(object::ID_PartFrac)->evaluate()!=object::OK)return nullptr;answer=rt.pop();}
 if(action==4){symbol_g variable=symbol::make(doc.variable);expression_g replacement=pico_formula_parse(doc.replacement);if(variable&&replacement)answer=e->substitute(variable,replacement);}
 if(answer)answer=rt.clone(answer);
 }return answer;
}
struct pico_lab_derivatives {expression_g f,g,fx,fy,gx,gy,fxx,fxy,fyy;unsigned mode;};
inline object_p pico_lab_derivative_item(size_t,size_t,size_t row,size_t col,void *p){auto &d=*static_cast<pico_lab_derivatives *>(p);if(d.mode==0)return row?object_p(+d.fy):object_p(+d.fx);if(d.mode==1)return row?(col?object_p(+d.gy):object_p(+d.gx)):(col?object_p(+d.fy):object_p(+d.fx));return row?(col?object_p(+d.fyy):object_p(+d.fxy)):(col?object_p(+d.fxy):object_p(+d.fxx));}
inline object_p pico_lab_calculus(const pico_math_lab &doc,unsigned action)
{
 if(!doc.valid()||action>4){rt.value_error();return nullptr;}
 stack_depth_restore stack;object_g answer;auto requested=Settings.Precision();
 {pico_workbench_scope scope;if(!scope.entered)return nullptr;settings::SavePrecision precision(requested);settings::SaveMaxRewrites rewrites(200);settings::SaveMaxFactorIterations factors(10000);settings::SaveAngleMode angle(pico_lab_angle(doc.angle));settings::SaveHardwareFloatingPoint hardware(false);settings::SaveNumericalResults exact(false);settings::SaveNumericalConstants constants(false);
 pico_lab_derivatives d;d.f=pico_formula_parse(doc.function);symbol_g x=symbol::make("X"),y=symbol::make("Y");if(!d.f||!x||!y)return nullptr;
 if(action!=1)d.fx=d.f->derivative(x);if(action!=0)d.fy=d.f->derivative(y);if((action!=1&&!d.fx)||(action!=0&&!d.fy))return nullptr;
 if(action==0)answer=object_p(+d.fx);if(action==1)answer=object_p(+d.fy);if(action==2){d.mode=0;answer=array::build(2,0,pico_lab_derivative_item,&d);}
 if(action==3){d.g=pico_formula_parse(doc.other);if(!d.g)return nullptr;d.gx=d.g->derivative(x);d.gy=d.g->derivative(y);if(!d.gx||!d.gy)return nullptr;d.mode=1;answer=array::build(2,2,pico_lab_derivative_item,&d);}
 if(action==4){d.fxx=d.fx->derivative(x);d.fxy=d.fx->derivative(y);d.fyy=d.fy->derivative(y);if(!d.fxx||!d.fxy||!d.fyy)return nullptr;d.mode=2;answer=array::build(2,2,pico_lab_derivative_item,&d);}
 if(answer)answer=rt.clone(answer);
 }return answer;
}
#include "math_lab_numeric.h"
#include "surface_evaluator.h"
inline bool pico_lab_plane(const pico_math_lab &doc,double *plane)
{
 stack_depth_restore stack;settings::SaveAngleMode angle(pico_lab_angle(doc.angle));pico_surface_evaluator e(doc.function);if(!e.valid())return false;
 double xa,xb,ya,yb,hx=6e-6*fmax(1.,fabs(doc.x)),hy=6e-6*fmax(1.,fabs(doc.y));
 if(e.sample(doc.x,doc.y,plane[0])!=pico_surface_evaluator::finite||e.sample(doc.x+hx,doc.y,xa)!=pico_surface_evaluator::finite||e.sample(doc.x-hx,doc.y,xb)!=pico_surface_evaluator::finite||e.sample(doc.x,doc.y+hy,ya)!=pico_surface_evaluator::finite||e.sample(doc.x,doc.y-hy,yb)!=pico_surface_evaluator::finite)return false;
 plane[1]=(xa-xb)/(2*hx);plane[2]=(ya-yb)/(2*hy);
 if(!std::isfinite(plane[1])||!std::isfinite(plane[2]))return false;
 // Reject obvious cusps and poorly resolved slopes rather than draw a false tangent.
 return fabs((xa-plane[0])/hx-(plane[0]-xb)/hx)<=1e-3*fmax(1.,fabs(plane[1]))&&fabs((ya-plane[0])/hy-(plane[0]-yb)/hy)<=1e-3*fmax(1.,fabs(plane[2]));
}
