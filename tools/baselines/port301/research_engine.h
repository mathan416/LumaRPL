#pragma once
#include "research_document.h"
#include "quantity_engine.h"
struct pico_research_evaluator {
 pico_workbench_scope scope;expression_g equation;bool failed=false,cancelled=false;settings::SaveAngleMode radians;settings::SaveHardwareFloatingPoint hardware;
 explicit pico_research_evaluator(const char *text,bool equality=false):equation(equality?pico_workbench_residual(text):pico_formula_parse(text)),radians(object::ID_Rad),hardware(true){}
 bool valid()const{return scope.entered&&equation&&!rt.error();}
 static bool field(void *p,double x,double y,double &value){auto &e=*static_cast<pico_research_evaluator *>(p);cleaner temporary;if(e.failed||e.cancelled)return false;if(program::interrupted()){e.cancelled=true;return false;}bool ok=e.scope.bind("X",x)&&e.scope.bind("Y",y)&&pico_workbench_value(e.equation,value);if(!ok&&rt.error()){if(!strcmp(cstring(rt.error()),"Out of memory"))e.failed=true;else if(!strcmp(cstring(rt.error()),"Interrupted"))e.cancelled=true;else rt.clear_error();}return ok;}
};
struct pico_research_uncertainty_evaluator {
 pico_workbench_scope scope;const pico_research_document &doc;char output[64]{};
 settings::SaveAngleMode radians;settings::SaveHardwareFloatingPoint hardware;
 explicit pico_research_uncertainty_evaluator(const pico_research_document &d):doc(d),radians(object::ID_Rad),hardware(true){snprintf(output,sizeof output,"%s",d.units[5]);}
 bool valid()const{return scope.entered&&!pico_quantity_affine(doc.units[5]);}
 static bool evaluate(void *p,const double *v,double &result){auto &e=*static_cast<pico_research_uncertainty_evaluator *>(p);cleaner temporary;if(program::interrupted())return false;for(unsigned i=0;i<3;++i){char name[]={char('A'+i),0};if(pico_quantity_affine(e.doc.units[i+2])||!pico_quantity_bind(name,v[i],e.doc.units[i+2]))return false;}object_g value=pico_form_run_expression(e.doc.formula[3]);if(!value)return false;if(!*e.output&&!pico_quantity_unit_name(value->as_algebraic(),e.output,sizeof e.output))return false;return pico_quantity_value(value->as_algebraic(),e.output,result,false);}
};
inline bool pico_research_taylor(const char *text,double at,unsigned degree,double *coefficient){if(degree>4)return false;stack_depth_restore stack;pico_workbench_scope scope;if(!scope.entered)return false;settings::SaveAngleMode radians(object::ID_Rad);settings::SaveMaxRewrites rewrites(200);expression_g current=pico_formula_parse(text);symbol_g x=symbol::make("X");if(!current||!x)return false;double factorial=1;for(unsigned i=0;i<=degree;++i){if(program::interrupted()||!scope.bind("X",at))return false;double value;if(!pico_workbench_value(current,value))return false;coefficient[i]=value/factorial;if(i<degree){current=current->derivative(x);if(!current)return false;factorial*=i+1;}}return true;}
