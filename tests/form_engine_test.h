#pragma once
#include "form_engine.h"
inline void run_form_engine_tests()
{
    size_t depth=rt.depth(),directories=rt.directories(),variables=rt.variables(0)->count();auto precision=Settings.Precision();bool hardware=Settings.HardwareFloatingPoint();
    pico_form_document form;object_g result=pico_form_calculate(form);surface_check(result&&result->as_int32(0,false)==3,"form Ohm calculation");
    rt.gc();surface_check(result&&result->as_int32(0,false)==3,"form result survives scope teardown and GC");
    strcpy(form.formula,"VOLTAGE");result=pico_form_calculate(form);rt.gc();
    surface_check(result&&result->as_int32(0,false)==12,"form input alias cloned before scoped directory removal");
    strcpy(form.formula,"VOLTAGE/RESISTANCE");
    strcpy(form.fields[0].input,"1/3");strcpy(form.fields[1].input,"2");result=pico_form_calculate(form);surface_check(result&&result->is_fraction(),"form exact fractional inputs and result");
    strcpy(form.fields[1].input,"0");result=pico_form_calculate(form);surface_check(!result,"form division-by-zero failure");rt.clear_error();
    strcpy(form.fields[1].input,"2");strcpy(form.fields[0].name,"SIN");result=pico_form_calculate(form);surface_check(!result,"reject reserved form input name");rt.clear_error();
    strcpy(form.fields[0].name,"VOLTAGE");strcpy(form.fields[0].input,"SQRT(-1)");
    {settings::SaveComplexResults complex(true);result=pico_form_calculate(form);surface_check(!result,"reject non-real input");rt.clear_error();
    form.count=0;strcpy(form.formula,"SQRT(-1)");result=pico_form_calculate(form);surface_check(result&&result->is_complex(),"complex form output uses calculator mode");}
    form=pico_form_document();strcpy(form.fields[0].input,"10^101");result=pico_form_calculate(form);surface_check(!result,"reject input beyond finite scalar bound");rt.clear_error();
    form=pico_form_document();key_pop_all();key_push(33);key_pop();key_push(33);program::count_interrupted=33;
    result=pico_form_calculate(form);surface_check(!result,"form cancellation");key_pop_all();rt.clear_error();surface_check(!program::halted,"form cancellation state restored");
    {pico_workbench_scope fixture;surface_check(fixture.entered&&fixture.bind("VOLTAGE",99),"form variable fixture");result=pico_form_calculate(form);surface_check(result,"form nested calculation");symbol_g name=symbol::make("VOLTAGE");object_g original=directory::recall_all(name,false);double value;algebraic_g scalar=original?original->as_algebraic():nullptr;surface_check(pico_workbench_double(scalar,value)&&value==99,"preserve existing named variable");}
    surface_check(rt.depth()==depth&&rt.directories()==directories&&rt.variables(0)->count()==variables,"forms preserve stack and directory values");
    surface_check(Settings.Precision()==precision&&Settings.HardwareFloatingPoint()==hardware,"forms preserve arithmetic settings");
    printf("PASS: form real engine, exact arithmetic, names, failures, GC and caller preservation\n");
}
