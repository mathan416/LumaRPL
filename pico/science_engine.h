#pragma once
#include "science_document.h"
#include "science_numeric.h"
#include "quantity_engine.h"
#include "integer.h"
struct pico_science_evaluator {
    pico_workbench_scope scope;expression_g formula;settings::SaveHardwareFloatingPoint hardware;unsigned count=0;
    explicit pico_science_evaluator(const char *text):formula(pico_formula_parse(text)),hardware(true){}
    bool valid()const{return scope.entered&&formula&&!rt.error();}
    static bool fit(void *p,double x,const double *parameters,double &result){auto &e=*static_cast<pico_science_evaluator *>(p);cleaner temporary;if(!e.scope.bind("X",x)||!e.scope.bind("A",parameters[0])||!e.scope.bind("B",parameters[1])||!e.scope.bind("C",parameters[2]))return false;return pico_workbench_value(e.formula,result);}
    bool recurrence(double n,double u,double &result){cleaner temporary;return scope.bind("N",n)&&scope.bind("U",u)&&pico_workbench_value(formula,result);}
};
struct pico_uncertainty_result {double nominal=0,sigma=0,contribution[3]{};char unit[64]{};};
inline bool pico_uncertainty(const pico_science_document &d,pico_uncertainty_result &r){
    if(!d.valid()){rt.value_error();return false;}stack_depth_restore stack;pico_workbench_scope scope;if(!scope.entered)return false;settings::SaveHardwareFloatingPoint hardware(true);settings::SaveAngleMode radians(object::ID_Rad);
    expression_g formula=pico_formula_parse(d.formula);if(!formula)return false;double v[3],s[3];char declarations[3][64]{};
    for(int i=0;i<3;++i){if(pico_quantity_affine(d.value[i])||pico_quantity_affine(d.sigma[i])||pico_quantity_affine(d.units[i])||pico_quantity_affine(d.output)){rt.dimension_error();return false;}
        algebraic_g input=pico_quantity_parse(d.value[i]);if(!input)return false;
        if(*d.units[i])snprintf(declarations[i],sizeof declarations[i],"%s",d.units[i]);else if(!pico_quantity_unit_name(input,declarations[i],sizeof declarations[i])){rt.value_error();return false;}
        if(!pico_quantity_value(input,declarations[i],v[i])||!pico_quantity_input(d.sigma[i],declarations[i],s[i])||s[i]<0)return false;
        char name[]={char('A'+i),0};if(!pico_quantity_bind(name,v[i],declarations[i]))return false;
    }
    object_g raw=pico_form_run_expression(d.formula);algebraic_g result=raw?raw->as_algebraic():nullptr;if(!result)return false;
    if(*d.output)snprintf(r.unit,sizeof r.unit,"%s",d.output);else if(!pico_quantity_unit_name(result,r.unit,sizeof r.unit)){rt.value_error();return false;}
    if(!pico_quantity_value(result,r.unit,r.nominal))return false;
    for(int i=0;i<3;++i){if(s[i]==0)continue;char name[]={char('A'+i),0};double h=6e-6*fmax(1e-12,fmax(fabs(v[i]),s[i])),plus,minus;
        auto evaluate=[&](double x,double &number){cleaner temporary;if(program::interrupted()){rt.interrupted_error();return false;}if(!pico_quantity_bind(name,x,declarations[i]))return false;object_g raw=pico_form_run_expression(d.formula);return raw&&pico_quantity_value(raw->as_algebraic(),r.unit,number);};
        if(v[i]+h==v[i]||!evaluate(v[i]+h,plus)||!evaluate(v[i]-h,minus)||!pico_quantity_bind(name,v[i],declarations[i]))return false;
        double forward=(plus-r.nominal)/h,backward=(r.nominal-minus)/h,derivative=(plus-minus)/(2*h);if(!std::isfinite(derivative)||fabs(forward-backward)>1e-3*fmax(1e-10,fabs(derivative))){rt.value_error();return false;}
        r.contribution[i]=fabs(derivative)*s[i];r.sigma=hypot(r.sigma,r.contribution[i]);
    }
    return std::isfinite(r.sigma);
}
inline object_p pico_uncertainty_values(const pico_uncertainty_result &r){return array::build(2,0,[](size_t,size_t,size_t row,size_t,void *p)->object_p{auto &r=*static_cast<const pico_uncertainty_result *>(p);return pico_quantity_number(row?r.sigma:r.nominal,r.unit);},const_cast<pico_uncertainty_result *>(&r));}
inline bool pico_science_integer(const char *text,uint64_t &number){if(!*text)return false;number=0;for(const char *p=text;*p;++p){if(*p<'0'||*p>'9'||number>100000000000ULL)return false;number=number*10+(*p-'0');if(number>1000000000000ULL)return false;}return true;}
inline object_p pico_discrete_calculate(const pico_science_document &d,unsigned action){uint64_t a,b,m;if(!pico_science_integer(d.integers[0],a)||!pico_science_integer(d.integers[1],b)||!pico_science_integer(d.integers[2],m)||action>6){rt.value_error();return nullptr;}
 stack_depth_restore stack;object_g result;{pico_workbench_scope scope;if(!scope.entered)return nullptr;settings::SaveNumericalResults exact(false);settings::SaveHardwareFloatingPoint hardware(false);
 if(action==0)result=integer::make(pico_science::gcd(a,b));
 if(action==1){algebraic_g x=integer::make(a),y=integer::make(b),g=integer::make(pico_science::gcd(a,b));result=(!a||!b)?object_p(integer::make(0)):object_p(+(x/g*y));}
 if(action==2){if(!b){rt.zero_divide_error();return nullptr;}result=integer::make(a%b);}
 if(action==3){uint64_t v;if(!pico_science::modpow(a,b,m,v)){rt.value_error();return nullptr;}result=integer::make(v);}
 if(action==4){if(a<2||a>1000000000ULL){rt.value_error();return nullptr;}uint64_t n=a;unsigned count=0;for(uint64_t factor=2;factor<=n/factor;factor+=factor==2?1:2){if((factor&255)==1&&program::interrupted()){rt.interrupted_error();return nullptr;}while(n%factor==0){object_g f=integer::make(factor);if(!f||!rt.push(f))return nullptr;++count;n/=factor;}}if(n>1){object_g f=integer::make(n);if(!f||!rt.push(f))return nullptr;++count;}result=list::list_from_stack(count,object::ID_list);}
 if(action>=5){if(a>1000||b>a){rt.value_error();return nullptr;}algebraic_g value=integer::make(1);for(uint64_t i=1;i<=b;++i){if(program::interrupted()){rt.interrupted_error();return nullptr;}algebraic_g x=integer::make(a-i+1),y=integer::make(i);value=value*x;if(action==5)value=value/y;if(!value)return nullptr;}result=object_p(+value);}
 if(result)result=rt.clone(result);}
 return result;
}
