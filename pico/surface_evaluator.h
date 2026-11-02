#pragma once
#include "expression.h"
#include "algebraic.h"
#include "hwfp.h"
#include "settings.h"
#include <cmath>
#include <cstring>
#include <cstdio>

// Scoped bindings use the same evaluator as upstream two-variable plots.
// GC roots outlive every sample; caller stack and stored X/Y are preserved.
struct pico_surface_evaluator
{
    enum result { finite, hole, cancelled, failed };
    program_g equation;
    symbol_g xname,yname;
    save<symbol_g *> independent,dependent;
    settings::PrepareForFunctionEvaluation evaluation;
    settings::SaveHardwareFloatingPoint hardware;
    settings::SavePrecision precision;
    save<bool> halted;
    explicit pico_surface_evaluator(const char *formula,bool two_variables=true)
        : xname(symbol::make("X")),yname(symbol::make("Y")),
          independent(expression::independent,&xname),
          dependent(expression::dependent,two_variables?&yname:nullptr),hardware(true),precision(16),
          halted(program::halted,false)
    {
        char source[192];
        if(strlen(formula)>159 || strchr(formula,'\'')) { rt.syntax_error();return; }
        snprintf(source,sizeof source,"'%s'",formula);
        program_g parsed=program::parse(utf8(source),strlen(source));
        if(!parsed || !xname || !yname) return;
        for(object_p item:*parsed)
        {
            if(equation || item->type()!=object::ID_expression)
            { equation=nullptr;rt.syntax_error();return; }
            equation=expression_p(item);
        }
        if(!equation) rt.syntax_error();
    }
    bool valid() const { return equation && xname && yname && !rt.error(); }
    result sample(double x,double y,double &z)
    {
        cleaner temporary;
        algebraic_g ax=hwdouble::make(x),ay=hwdouble::make(y);
        if(!ax || !ay) return failed;
        algebraic_g value=algebraic::evaluate_function(equation,ax,ay);
        if(program::halted) return cancelled;
        if(rt.error() && !strcmp(cstring(rt.error()),"Out of memory")) return failed;
        z=NAN;
        if(value && value->is_real())
        {
            if(!algebraic::to_hwdouble(value))
            {
                if(rt.error() && !strcmp(cstring(rt.error()),"Out of memory")) return failed;
                rt.clear_error();return hole;
            }
            if(value->type()==object::ID_hwdouble) z=hwdouble_p(+value)->value();
            else if(value->type()==object::ID_hwfloat) z=hwfloat_p(+value)->value();
        }
        if(rt.error() && !strcmp(cstring(rt.error()),"Out of memory")) return failed;
        rt.clear_error();
        return std::isfinite(z)?finite:hole;
    }
};
