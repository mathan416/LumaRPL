#pragma once
#include "form_engine.h"
#include "ode_engine.h"
inline void run_port15_engine_tests()
{
    settings::SavePrecision test_precision(16);size_t depth=rt.depth(),dirs=rt.directories(),vars=rt.variables(0)->count();auto precision=Settings.Precision();bool hardware=Settings.HardwareFloatingPoint();
    {
        pico_form_document form;strcpy(form.fields[0].units,"V");strcpy(form.fields[1].units,"ohm");strcpy(form.output_units,"A");
        object_g result=pico_form_calculate(form);surface_check(result&&unit::get(result),"unit form returns a quantity");unit_g quantity=unit::get(result);algebraic_g mag=quantity?quantity->value():nullptr;double value;
        {settings::SavePrecision numeric_precision(16);surface_check(pico_workbench_double(mag,value)&&std::fabs(value-3)<1e-10,"Ohm units convert to amperes");}rt.gc();surface_check(unit::get(result),"unit result survives GC");
        pico_form_curve curve{form,1};surface_check(curve.prepare(),"unit sweep preparation");rt.gc();surface_check(curve.sample(2,value)==1&&std::fabs(value-6)<1e-10,"unit-aware form sweep uses labeled magnitudes");
        strcpy(form.formula,"VOLTAGE+RESISTANCE");result=pico_form_calculate(form);surface_check(!result,"incompatible dimensions rejected");rt.clear_error();
        form=pico_form_document();form.count=1;strcpy(form.fields[0].name,"LENGTH");strcpy(form.fields[0].input,"100_cm");strcpy(form.fields[0].units,"m");strcpy(form.formula,"LENGTH");strcpy(form.output_units,"m");
        result=pico_form_calculate(form);quantity=unit::get(result);mag=quantity?quantity->value():nullptr;surface_check(pico_workbench_double(mag,value)&&std::fabs(value-1)<1e-10,"centimetres converted to metres");
        strcpy(form.output_units,"s");result=pico_form_calculate(form);surface_check(!result,"output unit dimension mismatch");rt.clear_error();
        form=pico_form_document();strcpy(form.fields[0].name,"DISTANCE");strcpy(form.fields[1].name,"DURATION");strcpy(form.fields[0].units,"m");strcpy(form.fields[1].units,"s");strcpy(form.formula,"DISTANCE/DURATION");strcpy(form.output_units,"m/s");result=pico_form_calculate(form);quantity=unit::get(result);mag=quantity?quantity->value():nullptr;surface_check(pico_workbench_double(mag,value)&&std::fabs(value-3)<1e-10,"composite unit output");

    }
    {
        pico_ode_scene scene;pico_ode_context context(scene);double cache[900];surface_check(context.valid()&&context.rebuild(scene.window,cache)==1,"ODE native evaluator");
        surface_check(std::fabs(cache[299]-std::exp(2.0))<1e-5,"ODE exponential solution");
        surface_check(std::fabs(cache[0]-std::exp(-2.0))<1e-5&&context.info.evaluations<=8192,"ODE backward integration and budget");
        double value;surface_check(context.sample(0,value)==1&&std::fabs(value-1)<1e-4,"ODE interpolation");rt.gc();surface_check(context.slope(0,2,value)==1&&value==2,"ODE scope survives GC");
    }
    surface_check(rt.depth()==depth&&rt.directories()==dirs&&rt.variables(0)->count()==vars,"port15 preserves caller state");surface_check(Settings.Precision()==precision&&Settings.HardwareFloatingPoint()==hardware,"port15 restores settings");
    puts("PASS: native ODE integration and dimensional forms/conversions/sweeps/GC/caller preservation");
}
