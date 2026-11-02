#pragma once
#include "form_document.h"
#include "workbench_engine.h"
#include "unit.h"

inline bool pico_form_name(const char *source)
{
    if(!pico_form_document::identifier(source)){rt.invalid_name_error();return false;}
    expression_g expression=pico_formula_parse(source);
    symbol_g name=expression?expression->as_quoted<symbol>():nullptr;
    if(!name||!name->matches(source)){rt.clear_error();rt.invalid_name_error();return false;}
    return true;
}
inline object_p pico_form_run_expression(const char *source)
{
    stack_depth_restore stack;
    expression_g equation=pico_formula_parse(source);
    if(!equation||equation->run()!=object::OK||rt.depth()!=stack.depth+1)return nullptr;
    return rt.pop();
}
inline unit_p pico_form_unit(const char *source)
{
    char text[96];if(!*source)return nullptr;if(strnlen(source,80)>=80){rt.value_error();return nullptr;}snprintf(text,sizeof text,"1_%s",source);object_g value=pico_form_run_expression(text);
    unit_p units=value?unit::get(value):nullptr;if(!units&&!rt.error())rt.type_error();return units;
}
inline unit_p pico_form_unit_one(unit_p source)
{
    if(!source)return nullptr;unit_g rooted=source;algebraic_g one=integer::make(1),dimension=rooted->uexpr();return unit::make(one,dimension);
}
inline bool pico_form_real_input(object_g &input,const char *declaration)
{
    algebraic_g scalar=input?input->as_algebraic():nullptr;if(!scalar)return false;
    if(*declaration)
    {
        unit_g target=pico_form_unit(declaration);if(!target)return false;
        if(unit::get(scalar)){if(!target->convert(scalar))return false;input=object_p(+scalar);}
        else{algebraic_g dimension=target->uexpr();input=unit::make(scalar,dimension);if(!input)return false;scalar=input->as_algebraic();}
    }
    unit_g units=unit::get(scalar);algebraic_g magnitude=units?units->value():+scalar;double number;
    return pico_workbench_double(magnitude,number);
}
inline object_p pico_form_calculate(const pico_form_document &form,int swept=-1,double swept_value=0,unit_g *captured_unit=nullptr,bool apply_sweep=true,unit_p swept_unit=nullptr)
{
    stack_depth_restore stack;object_g result;unit_g sweep_root=swept_unit;
    auto precision=Settings.Precision();bool hardware=Settings.HardwareFloatingPoint();
    bool numerical=Settings.NumericalResults(),constants=Settings.NumericalConstants(),angle_units=Settings.SetAngleUnits(),symbolic_solver=Settings.SymbolicSolver();
    auto iterations=Settings.SolverIterations(),imprecision=Settings.SolverImprecision(),shuffles=Settings.SolverShuffles();
    if(!form.valid()||swept< -1||swept>=int(form.count)||(swept>=0&&!pico_workbench::number(swept_value))){rt.value_error();return nullptr;}
    {
        pico_workbench_scope scope;if(!scope.entered)return nullptr;
        settings::SavePrecision saved_precision(precision);settings::SaveHardwareFloatingPoint saved_hardware(hardware);
        settings::SaveNumericalResults saved_numerical(numerical);settings::SaveNumericalConstants saved_constants(constants);
        settings::SaveSetAngleUnits restore_angle_units(angle_units);settings::SaveSymbolicSolver restore_symbolic(symbolic_solver);
        settings::SaveSolverIterations restore_iterations(iterations);settings::SaveSolverImprecision restore_imprecision(imprecision);settings::SaveSolverShuffles restore_shuffles(shuffles);
        for(unsigned i=0;i<form.count;++i)
        {
            if(program::interrupted()){rt.interrupted_error();return nullptr;}
            const auto &field=form.fields[i];if(!pico_form_name(field.name))return nullptr;
            object_g input=int(i)==swept&&apply_sweep?object_p(hwdouble::make(swept_value)):pico_form_run_expression(field.input);if(!input)return nullptr;
            if(int(i)==swept&&apply_sweep&&sweep_root){algebraic_g value=input->as_algebraic(),dimension=sweep_root->uexpr();input=unit::make(value,dimension);}
            {settings::SavePrecision conversion_precision(16);if(!pico_form_real_input(input,field.units)){if(!rt.error())rt.type_error();return nullptr;}}
            if(int(i)==swept&&captured_unit){unit_g units=unit::get(input);unit_g captured=pico_form_unit_one(units);*captured_unit=captured?unit_p(rt.clone(captured)):nullptr;}
            symbol_g name=symbol::make(field.name);
            if(!name||!directory::store_here(name,input))return nullptr;
        }
        if(program::interrupted()){rt.interrupted_error();return nullptr;}
        object_g value=pico_form_run_expression(form.formula);
        if(value&&!value->as_algebraic()){rt.type_error();return nullptr;}
        if(value&&*form.output_units)
        {
            unit_g target=pico_form_unit(form.output_units);algebraic_g converted=value->as_algebraic();
            if(!target||!target->convert(converted))return nullptr;value=object_p(+converted);
        }
        if(value)result=rt.clone(value);
    }
    return result;
}

struct pico_form_curve
{
    const pico_form_document &form;int swept;unit_g xunit,yunit;bool prepared=false;
    pico_form_curve(const pico_form_document &f,int input):form(f),swept(input){}
    bool prepare()
    {
        object_g baseline=pico_form_calculate(form,swept,0,&xunit,false);if(!baseline)return false;
        unit_g units=unit::get(baseline);yunit=pico_form_unit_one(units);prepared=true;return !rt.error();
    }
    int sample(double x,double &y)
    {
        cleaner temporary;settings::SavePrecision precision(16);settings::SaveHardwareFloatingPoint hardware(true);
        if(!prepared&&!prepare()){if(rt.error()&&!strcmp(cstring(rt.error()),"Out of memory"))return -1;if(rt.error()&&!strcmp(cstring(rt.error()),"Interrupted")){rt.clear_error();return -2;}rt.clear_error();return 0;}
        object_g value=pico_form_calculate(form,swept,x,nullptr,true,xunit);
        if(rt.error()&&!strcmp(cstring(rt.error()),"Out of memory"))return -1;
        if(rt.error()&&!strcmp(cstring(rt.error()),"Interrupted")){rt.clear_error();return -2;}
        algebraic_g scalar=value?value->as_algebraic():nullptr;
        if(yunit&&scalar){if(!yunit->convert(scalar)){rt.clear_error();return 0;}unit_g units=unit::get(scalar);scalar=units?units->value():nullptr;}
        else if(unit::get(scalar))scalar=nullptr;
        bool ok=pico_workbench_double(scalar,y);if(rt.error()&&!strcmp(cstring(rt.error()),"Out of memory"))return -1;rt.clear_error();return ok?1:0;
    }
};
