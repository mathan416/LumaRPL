#pragma once
#include "form_engine.h"
#include "currency_rates.h"

inline bool pico_conversion_real(algebraic_g value,double &number)
{
    settings::SavePrecision precision(16);return pico_workbench_double(value,number);
}
// Returns a GC-rootable result; preview never pushes onto the caller's stack.
inline object_p pico_convert_units(const char *amount,const char *from,const char *to)
{
    stack_depth_restore stack;object_g result;auto precision=Settings.Precision();
    {
        pico_workbench_scope scope;if(!scope.entered)return nullptr;
        settings::SaveNumericalResults numerical(false);settings::SavePrecision saved_precision(precision);
        if(program::interrupted()){rt.interrupted_error();return nullptr;}
        object_g input=pico_form_run_expression(amount);algebraic_g value=input?input->as_algebraic():nullptr;
        double finite;if(!value||unit::get(value)||!pico_conversion_real(value,finite)){if(!rt.error())rt.type_error();return nullptr;}
        unit_g source=pico_form_unit(from),target=pico_form_unit(to);if(!source||!target)return nullptr;
        algebraic_g dimension=source->uexpr();unit_g quantity=unit::make(value,dimension);if(!quantity)return nullptr;
        algebraic_g converted=quantity->as_algebraic();if(!target->convert(converted))return nullptr;
        result=rt.clone(converted);
    }
    return result;
}
inline object_p pico_convert_currency(const char *amount,const pico_currency_rates &rates,unsigned from,unsigned to)
{
    if(!rates.valid()||from>=rates.count||to>=rates.count){rt.value_error();return nullptr;}
    stack_depth_restore stack;object_g result;auto precision=Settings.Precision();
    {
        pico_workbench_scope scope;if(!scope.entered)return nullptr;
        settings::SaveNumericalResults numerical(false);settings::SavePrecision saved_precision(precision);
        if(program::interrupted()){rt.interrupted_error();return nullptr;}
        object_g input=pico_form_run_expression(amount);algebraic_g value=input?input->as_algebraic():nullptr;double finite;
        if(!value||unit::get(value)||!pico_conversion_real(value,finite)){if(!rt.error())rt.type_error();return nullptr;}
        object_g numerator=pico_form_run_expression(rates.rows[to].rate),denominator=pico_form_run_expression(rates.rows[from].rate);
        algebraic_g a=numerator?numerator->as_algebraic():nullptr,b=denominator?denominator->as_algebraic():nullptr;
        if(!a||!b)return nullptr;algebraic_g factor=a/b;if(!factor)return nullptr;
        algebraic_g converted=value*factor;if(converted)result=rt.clone(converted);
    }
    return result;
}
