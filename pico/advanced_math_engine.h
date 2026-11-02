#pragma once
#include "advanced_math.h"
#include "math_lab_engine.h"
#include "polynomial.h"
inline polynomial_p pico_guided_polynomial(const pico_advanced_math &d,unsigned which){
 symbol_g x=symbol::make("X");polynomial_g result=polynomial::make(integer::make(0));if(!x||!result)return nullptr;
 for(unsigned i=0;i<=d.degree[which];++i){algebraic_g coefficient;const char *xunit=d.units[3],*yunit=d.units[4+which];if(*xunit||*yunit){char expected[80];unsigned power=d.degree[which]-i;if(!power)snprintf(expected,sizeof expected,"%s",yunit);else snprintf(expected,sizeof expected,"(%s)/((%s)^%u)",*yunit?yunit:"1",*xunit?xunit:"1",power);double number;if(!pico_quantity_input(d.coefficients[which][i],expected,number))return nullptr;coefficient=hwdouble::make(number);}else coefficient=pico_complex_component(d.coefficients[which][i]);if(!coefficient)return nullptr;if(!i&&coefficient->is_zero()){rt.value_error();return nullptr;}polynomial_g term=polynomial::make(coefficient,x,d.degree[which]-i);if(!term)return nullptr;result=polynomial::add(result,term);if(!result)return nullptr;}return result;
}
// A, roots (repeated entries preserve multiplicity), P(x), A*B, quotient, remainder.
inline object_p pico_polynomial_calculate(const pico_advanced_math &d,unsigned operation,double x=0){
 if(!d.valid()||operation>5){rt.value_error();return nullptr;}stack_depth_restore stack;object_g result;
 {pico_workbench_scope scope;if(!scope.entered)return nullptr;settings::SaveComplexResults complex(true);settings::SaveMaxRewrites rewrites(200);settings::SaveMaxFactorIterations factors(10000);settings::SaveNumericalResults exact(false);settings::SaveNumericalConstants constants(false);
 polynomial_g a=pico_guided_polynomial(d,0);if(!a)return nullptr;
 if(operation==0)result=a->as_expression();
 if(operation==1){symbol_g variable=symbol::make("X");if(variable)result=a->roots(object::ID_array,variable,false);}
 if(operation==2){algebraic_g v=hwdouble::make(x);result=const_cast<polynomial *>(+a)->evaluate(v);}
 if(operation>=3){polynomial_g b=pico_guided_polynomial(d,1);if(!b)return nullptr;if(operation==3){polynomial_g product=polynomial::mul(a,b);if(product)result=product->as_expression();}else{polynomial_g quotient,remainder;if(polynomial::quorem(a,b,quotient,remainder))result=(operation==4?quotient:remainder)->as_expression();}}
 if(result){char output[72]="";if(operation==1&&*d.units[3]){unit_g u=pico_form_unit(d.units[3]);array_g roots=result->as<array>();if(!u||!roots)return nullptr;algebraic_g one=u->as_algebraic();result=roots->map(multiply::evaluate,one);}else{if(operation<=2||operation==5)snprintf(output,sizeof output,"%s",d.units[4]);if(operation==3&&(*d.units[4]||*d.units[5]))snprintf(output,sizeof output,"(%s)*(%s)",*d.units[4]?d.units[4]:"1",*d.units[5]?d.units[5]:"1");if(operation==4&&(*d.units[4]||*d.units[5]))snprintf(output,sizeof output,"(%s)/(%s)",*d.units[4]?d.units[4]:"1",*d.units[5]?d.units[5]:"1");if(*output){unit_g u=pico_form_unit(output);algebraic_g value=result->as_algebraic();if(!u||!value)return nullptr;algebraic_g dimension=u->uexpr();result=unit::make(value,dimension);}}}
 if(result)result=rt.clone(result);
 }return result;
}
struct pico_system_evaluator {
 pico_workbench_scope scope;expression_g f,g;settings::SaveHardwareFloatingPoint hardware;bool second;const pico_advanced_math &document;
 explicit pico_system_evaluator(const pico_advanced_math &d):f(pico_formula_parse(d.slope[0])),g(pico_formula_parse(d.slope[1])),hardware(true),second(d.ode_mode==1),document(d){}
 bool valid()const{return scope.entered&&f&&g&&!rt.error();}
 static bool sample(void *p,double x,const double *y,double *out){auto &e=*static_cast<pico_system_evaluator *>(p);auto &d=e.document;cleaner temporary;bool dimensional=*d.units[0]||*d.units[1]||*d.units[2];
 if(!pico_quantity_bind("X",x,d.units[0])||!pico_quantity_bind("Y",y[0],d.units[1])||!pico_quantity_bind("Z",y[1],d.units[2]))return false;
 if(!dimensional){if(e.second)out[0]=y[1];else if(!pico_workbench_value(e.f,out[0]))return false;return pico_workbench_value(e.g,out[1]);}
 for(int i=0;i<2;++i){char expected[64];if(!strcmp(d.units[i+1],d.units[0]))*expected=0;else snprintf(expected,sizeof expected,"(%s)/(%s)",*d.units[i+1]?d.units[i+1]:"1",*d.units[0]?d.units[0]:"1");object_g raw=i==0&&e.second?object_p(pico_quantity_number(y[1],d.units[2])):pico_form_run_expression(i?d.slope[1]:d.slope[0]);if(!raw||!pico_quantity_value(raw->as_algebraic(),expected,out[i],false))return false;}return true;
 }
};
