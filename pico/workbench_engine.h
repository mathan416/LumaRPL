#pragma once
#include "workbench.h"
#include "calculus.h"
#include "variables.h"
#include "solve.h"
#include "list.h"
// The engine may store trial solutions. A transient child directory keeps those
// writes out of the user's directory while retaining read access to parents.
struct pico_workbench_scope
{
    size_t directories;
    directory_g parent;symbol_g temporary_name;
    bool entered=false,created=false;
    save<bool> halted;
    settings::PrepareForFunctionEvaluation evaluation;
    settings::SavePrecision precision;
    settings::SaveHardwareFloatingPoint hardware;
    settings::SaveSolverIterations iterations;
    settings::SaveSolverImprecision imprecision;
    settings::SaveSolverShuffles shuffles;
    settings::SaveSymbolicSolver symbolic;
    save<symbol_g *> independent,dependent;
    pico_workbench_scope():directories(rt.directories()),parent(rt.variables(0)),
        halted(program::halted,false),precision(16),hardware(false),iterations(200),imprecision(6),shuffles(1),symbolic(false),
        independent(expression::independent,nullptr),dependent(expression::dependent,nullptr)
    {
        if(!parent)return;
        for(unsigned n=0;n<256;++n)
        {
            char name[32];snprintf(name,sizeof name,"WBTemporary%u",n);temporary_name=symbol::make(name);
            if(!temporary_name)return;if(parent->recall(temporary_name))continue;
            directory_g empty=rt.make<directory>(object::ID_directory);if(!empty)return;
            object_g stored=directory::store_here(temporary_name,empty);if(!stored)return;
            created=true;entered=rt.enter(directory_p(+stored));return;
        }
    }
    ~pico_workbench_scope()
    {
        if(entered)rt.updir(rt.directories()-directories);
        if(created)const_cast<directory *>(+parent)->purge(temporary_name,true);
    }
    bool bind(const char *name,double value)
    {symbol_g symbol=symbol::make(name);algebraic_g number=hwdouble::make(value);return entered&&symbol&&number&&directory::store_here(symbol,number);}
};
inline expression_p pico_workbench_parse(const char *source)
{
    // Internal residuals add parentheses to a user formula of at most 159 bytes.
    size_t n=strnlen(source,336);if(!n||n>=336||strchr(source,'\'')){rt.syntax_error();return nullptr;}
    char text[340];snprintf(text,sizeof text,"'%s'",source);program_g parsed=program::parse(utf8(text),strlen(text));expression_g result;
    if(parsed)for(object_p item:*parsed){if(result||item->type()!=object::ID_expression){rt.syntax_error();return nullptr;}result=expression_p(item);}
    return result;
}
inline bool pico_workbench_equation(const char *input,char *left,char *right)
{
    if(!pico_graph_scene::formula(input,false)){rt.syntax_error();return false;}
    const char *equal=strchr(input,'=');
    if(equal&&(strchr(equal+1,'=')||equal==input||!equal[1])){rt.syntax_error();return false;}
    if(equal){size_t n=equal-input;memcpy(left,input,n);left[n]=0;snprintf(right,160,"%s",equal+1);}
    else{snprintf(left,160,"%s",input);strcpy(right,"0");}
    expression_g a=pico_formula_parse(left),b=pico_formula_parse(right);
    return a&&b;
}
inline expression_p pico_workbench_residual(const char *input)
{
    char left[160],right[160],text[336];if(!pico_workbench_equation(input,left,right))return nullptr;
    snprintf(text,sizeof text,"(%s)-(%s)",left,right);return pico_workbench_parse(text);
}
inline bool pico_workbench_double(algebraic_g value,double &number)
{
    settings::SaveHardwareFloatingPoint hardware(true);
    if(!value||!value->is_real()||!algebraic::to_hwdouble(value))return false;
    number=value->type()==object::ID_hwdouble?hwdouble_p(+value)->value():value->type()==object::ID_hwfloat?hwfloat_p(+value)->value():NAN;
    return pico_workbench::number(number);
}
inline bool pico_workbench_value(expression_r equation,double &number)
{
    stack_depth_restore stack;
    if(!equation)return false;auto outcome=equation->run();
    if(outcome!=object::OK||rt.depth()!=stack.depth+1)return false;
    object_g object=rt.pop();return pico_workbench_double(object->as_algebraic(),number);
}
inline bool pico_workbench_check(const char *input,double &residual)
{
    char left[160],right[160];if(!pico_workbench_equation(input,left,right))return false;
    expression_g a=pico_formula_parse(left),b=pico_formula_parse(right);double av,bv;
    if(!pico_workbench_value(a,av)||!pico_workbench_value(b,bv))return false;
    residual=(av-bv)/std::fmax(1.0,std::fmax(std::fabs(av),std::fabs(bv)));
    return std::isfinite(residual)&&std::fabs(residual)<=1e-8;
}
inline bool pico_workbench_solve(const pico_workbench &work,bool system,double *values,double *residuals)
{
    stack_depth_restore stack;pico_workbench_scope scope;if(!scope.entered||!work.valid())return false;
    int count=system?work.count:1;settings::SaveSolverIterations shared_iterations(200/count);const char *names[]={"X","Y","Z"};
    expression_g equations[3];symbol_g symbols[3];algebraic_g guesses[3];
    for(int i=0;i<count;++i)
    {
        equations[i]=pico_workbench_residual(system?work.equations[i]:work.single);symbols[i]=symbol::make(names[i]);
        guesses[i]=hwdouble::make(system?work.guesses[i]:work.guess);
        if(!equations[i]||!symbols[i]||!guesses[i]||!scope.bind(names[i],system?work.guesses[i]:work.guess))return false;
    }
    if(!system)
    {
        program_g equation=program_p(+equations[0]);algebraic_g variable=algebraic_p(+symbols[0]);
        algebraic_g solution=Root::solve(equation,variable,guesses[0]);
        if(!pico_workbench_double(solution,values[0])||rt.error())return false;
    }
    else
    {
        list_g eqs=count==2?list::make(equations[0],equations[1]):list::make(equations[0],equations[1],equations[2]);
        list_g vars=count==2?list::make(symbols[0],symbols[1]):list::make(symbols[0],symbols[1],symbols[2]);
        list_g gs=count==2?list::make(guesses[0],guesses[1]):list::make(guesses[0],guesses[1],guesses[2]);
        if(!eqs||!vars||!gs)return false;
        list_g solved=Root::multiple_equation_solver(eqs,vars,gs);if(!solved||rt.error())return false;
                if(solved->items()!=unsigned(count))return false;
        for(int i=0;i<count;++i)
        {
            object_g object=directory::recall_all(symbols[i],false);if(!object)return false;
            algebraic_g value=object->as_algebraic();if(!pico_workbench_double(value,values[i]))return false;
        }
    }
    for(int i=0;i<count;++i)if(!scope.bind(names[i],values[i]))return false;
    for(int i=0;i<count;++i)if(!pico_workbench_check(system?work.equations[i]:work.single,residuals[i]))return false;
    return !program::halted;
}
inline expression_p pico_workbench_isolate(const char *source)
{
    stack_depth_restore stack;expression_g result;
    {
        pico_workbench_scope scope;if(!scope.entered)return nullptr;
        settings::SaveNumericalResults numerical(false);settings::SaveNumericalConstants constants(false);
        expression_g equation=pico_workbench_residual(source);symbol_g name=symbol::make("X");
        if(equation&&name)result=Root::isolate(equation,name);
    }
    return result;
}
struct pico_workbench_sampler
{
    pico_workbench_scope scope;
    expression_g equation;
    explicit pico_workbench_sampler(const char *formula):equation(pico_workbench_residual(formula)){}
    int sample(double x,const pico_workbench *work,double &value)
    {
        cleaner temporary;
        if(!scope.entered||!equation||!scope.bind("X",x))return -1;
        if(work)for(int i=0;i<3;++i){char name[2]={char('A'+i),0};if(!scope.bind(name,work->parameters[i].value))return -1;}
        if(program::interrupted())return -2;
        if(pico_workbench_value(equation,value))return 1;
        if(program::halted)return -2;
        if(rt.error()&&!strcmp(cstring(rt.error()),"Out of memory"))return -1;
        rt.clear_error();return 0;
    }
};
