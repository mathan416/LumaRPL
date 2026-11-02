#pragma once
#include "expression.h"
#include "hwfp.h"
#include "program.h"
#include "settings.h"
#include "text.h"
#include "graph_scene.h"
#include <cstring>
#include <cstdio>
inline expression_p pico_formula_parse(const char *formula)
{
    size_t length=strnlen(formula,160);char input[160];
    if(length>=160){rt.syntax_error();return nullptr;}memcpy(input,formula,length+1);
    if(!pico_graph_scene::formula(input,false)){rt.syntax_error();return nullptr;}
    settings::SaveDecimalComma comma(false);char source[164];snprintf(source,sizeof source,"'%s'",input);
    program_g parsed=program::parse(utf8(source),strlen(source));expression_g equation;
    if(parsed)for(object_p item:*parsed)
    {
        if(equation||item->type()!=object::ID_expression){rt.syntax_error();return nullptr;}
        equation=expression_p(item);
    }
    return equation;
}
inline expression_p pico_formula_calculus(const char *formula,bool primitive,const char *variable="X")
{
    stack_depth_restore stack;save<bool> halted(program::halted,false);
    settings::SaveHardwareFloatingPoint hardware(false);settings::SaveNumericalResults numerical(false);settings::SaveNumericalConstants constants(false);
    expression_g equation=pico_formula_parse(formula);symbol_g x=symbol::make(variable);
    if(!equation||!x)return nullptr;
    return primitive?equation->primitive(x):equation->derivative(x);
}
inline bool pico_formula_source(object_p object,char *destination,size_t capacity)
{
    if(!object||!capacity)return false;object_g value=object;text_g text=value->as_text(true);if(!text)return false;
    size_t length;utf8 bytes=text->value(&length);
    if(length>=2&&bytes[0]=='\''&&bytes[length-1]=='\''){++bytes;length-=2;}
    if(length>=capacity||length>159)return false;
    char copy[160];memcpy(copy,bytes,length);copy[length]=0;
    if(!pico_graph_scene::formula(copy,false))return false;
    memcpy(destination,copy,length+1);return true;
}
inline bool pico_formula_scalar(const char *source,double &number)
{
    stack_depth_restore stack;save<bool> halted(program::halted,false);
    settings::PrepareForFunctionEvaluation evaluation;settings::SaveHardwareFloatingPoint hardware(true);settings::SavePrecision precision(16);
    expression_g equation=pico_formula_parse(source);
    if(!equation||equation->run()!=object::OK||rt.depth()!=stack.depth+1)return false;
    object_g object=rt.pop();algebraic_g value=object->as_algebraic();
    if(!value||!value->is_real()||!algebraic::to_hwdouble(value))return false;
    number=value->type()==object::ID_hwdouble?hwdouble_p(+value)->value():value->type()==object::ID_hwfloat?hwfloat_p(+value)->value():NAN;
    return std::isfinite(number);
}
inline algebraic_p pico_formula_exact_bound(const char *source)
{
    stack_depth_restore stack;expression_g equation=pico_formula_parse(source);
    if(!equation||equation->run()!=object::OK||rt.depth()!=stack.depth+1)return nullptr;
    object_g value=rt.pop();return value->as_algebraic();
}
inline algebraic_p pico_formula_definite(const char *source,const char *lower,const char *upper)
{
    stack_depth_restore stack;save<bool> halted(program::halted,false);
    settings::SaveHardwareFloatingPoint hardware(false);settings::SaveNumericalResults numerical(false);settings::SaveNumericalConstants constants(false);
    expression_g primitive=pico_formula_calculus(source,true);symbol_g x=symbol::make("X");
    if(!primitive||!x)return nullptr;
    algebraic_g a=pico_formula_exact_bound(lower);if(!a)return nullptr;
    algebraic_g b=pico_formula_exact_bound(upper);
    if(!b||(!a->is_real()&&!a->is_symbolic())||(!b->is_real()&&!b->is_symbolic()))return nullptr;
    algebraic_g low=primitive->substitute(x,a),high=primitive->substitute(x,b);
    if(!low||!high)return nullptr;algebraic_g difference=high-low;
    if(!difference)return nullptr;algebraic_g result=difference->evaluate();
    if(result&&result->type()==object::ID_expression&&Settings.AutoSimplify())result=expression_p(+result)->simplify();
    return result;
}
