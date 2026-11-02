#pragma once
#include "surface_evaluator.h"
#include <cstdlib>
#include "lua_runner.h"
#include "graph_scene.h"
#include "plot_engine.h"
#include "plot_analysis.h"
#include "calculus.h"
#include "grob.h"
#include "stack.h"
#include "stack_menu.h"
#include "workbench_engine.h"
#include "data_engine.h"
inline void surface_check(bool ok,const char *message)
{
    if(!ok) { fprintf(stderr,"FAIL 3D evaluator: %s\n",message);std::exit(1); }
}
#include "form_engine_test.h"
#include "notebook_engine_test.h"
extern const unsigned char PicoHeaderFont_sparse_font_data[];
extern "C" int pico_lua_cancel(void){return 0;}
#include "port14_engine_test.h"
#include "port15_engine_test.h"
#include "layout_engine_test.h"
#include "port16_engine_test.h"
#include "port17_engine_test.h"
inline void run_surface_engine_tests()
{
    font_p header=font_p(PicoHeaderFont_sparse_font_data);
    surface_check(header->height()>=12 && header->height()<21,"compact header height");
    surface_check(header->width(utf8("Wed 31/Dec/2099 23:59:59 Session"))<=248,"date/time/session fit");
    surface_check(header->width(utf8("2099-12-31 23:59:59 ABCDEFGHIJKLMNO"))<=248,"custom session name fit");
    printf("PASS: compact header %u px high, date/time/session width %u / 248 px\n",unsigned(header->height()),unsigned(header->width(utf8("Wed 31/Dec/2099 23:59:59 Session"))));
    run_port14_engine_tests();
    run_port15_engine_tests();
    run_layout_engine_tests();
    run_port16_engine_tests();
    run_port17_engine_tests();
    size_t depth=rt.depth();
    auto precision=Settings.Precision();bool hardware=Settings.HardwareFloatingPoint();
    auto commands=[](const char *source) {
        program_g p=program::parse(utf8(source),strlen(source));return p && p->run()==object::OK;
    };
    surface_check(commands("99 'X' STO 77 'Y' STO 42"),"prepare stored variables and stack");
    {
        auto press=[](int key){pico_stack_menu_key(ui,Stack,key,false,false);pico_stack_menu_key(ui,Stack,0,false,false);};
        surface_check(commands("10 20 30"),"prepare sticky stack menu test");
        ui.clear_shift();press(18);press(18);press(18);
        surface_check(Stack.interactive==3,"select stack level three");
        press(28);surface_check(ui.shift_plane()==1,"Tab selects Roll Up plane");
        press(41);surface_check(ui.shift_plane()==1,"Roll Up plane survives key release");
        press(41);surface_check(ui.shift_plane()==1,"repeated Roll Up stays selected");
        surface_check(rt.stack(0)->as_int32(0,false)==20,"repeated Roll Up preserves direction");
        press(23);surface_check(Stack.interactive==2&&ui.shift_plane()==1,"arrows move one level and preserve plane");
        press(28);surface_check(ui.shift_plane()==2,"Tab selects third plane");
        press(41);surface_check(ui.shift_plane()==2,"third plane survives operation");
        press(28);surface_check(ui.shift_plane()==0,"Tab wraps to first plane");
        press(28);press(13);surface_check(!Stack.interactive&&ui.shift_plane()==0,"Enter exits shifted stack cleanly");
        press(18);press(28);press(33);surface_check(!Stack.interactive&&ui.shift_plane()==0,"Esc exits shifted stack cleanly");
        rt.drop(3);
        press(28);press(41);surface_check(ui.shift_plane()==1,"ordinary calculator menu rows stay selected");
        ui.clear_shift();
        printf("PASS: actual UI sticky stack planes, repeated rolls, Tab cycle, navigation and exit cleanup\n");
    }
    {
        surface_check(commands("11 'A' STO 12 'B' STO 13 'C' STO 14 'Z' STO"),"prepare existing parameter variables");
        pico_workbench work;double values[3],residuals[3];auto dirs=rt.directories();auto variables=rt.variables(0)->count();
        auto old_angle=Settings.AngleMode();Settings.AngleMode(object::ID_Rad);
        surface_check(pico_workbench_solve(work,false,values,residuals)&&std::fabs(values[0]-.7390851332151607)<1e-8,"workbench local cosine solve");
        surface_check(rt.directories()==dirs&&rt.depth()==depth+1,"workbench solver scope cleanup");
        surface_check(pico_workbench_solve(work,true,values,residuals)&&std::fabs(values[0]-2)<1e-8&&std::fabs(values[1]-1)<1e-8,"workbench two variable system");
        work.count=3;strcpy(work.equations[0],"X+Y+Z=6");strcpy(work.equations[1],"X-Y=0");strcpy(work.equations[2],"Z=2");
        surface_check(pico_workbench_solve(work,true,values,residuals)&&std::fabs(values[0]-2)<1e-8&&std::fabs(values[1]-2)<1e-8&&std::fabs(values[2]-2)<1e-8,"workbench three variable system");
        work.count=2;strcpy(work.equations[0],"X^2+Y^2=1");strcpy(work.equations[1],"(X-1)^2+Y^2=1");work.guesses[0]=.4;work.guesses[1]=.8;
        surface_check(pico_workbench_solve(work,true,values,residuals)&&std::fabs(values[0]-.5)<1e-8&&std::fabs(values[1]-std::sqrt(.75))<1e-8,"workbench nonlinear system");
        strcpy(work.equations[0],"X+Y=1");strcpy(work.equations[1],"X+Y=2");
        surface_check(!pico_workbench_solve(work,true,values,residuals),"reject inconsistent system");rt.clear_error();
        strcpy(work.equations[0],"X+Y=1");strcpy(work.equations[1],"2*X+2*Y=2");
        surface_check(!pico_workbench_solve(work,true,values,residuals),"reject singular system");rt.clear_error();
        expression_g isolated=pico_workbench_isolate("2*X+3=7");surface_check(isolated,"workbench symbolic isolate");
        char exact[160];surface_check(pico_formula_source(isolated,exact,sizeof exact)&&!strcmp(exact,"X=2"),"symbolic isolate preserves exact equality");
        {
            pico_workbench_sampler sampler("A*SIN(B*X+C)");double value;
            surface_check(sampler.sample(0,&work,value)==1&&value==0,"workbench scoped parameter sample");
            work.parameters[2].value=1;
            surface_check(sampler.sample(0,&work,value)==1&&std::fabs(value-std::sin(1.0))<1e-10,"workbench changed slider sample");
            for(int i=0;i<64;++i){rt.gc();work.adjust(0,i<32?1:-1);surface_check(sampler.sample(.5,&work,value)==1,"repeated parameter redraw under GC");}
        }
        rt.gc();surface_check(pico_formula_source(isolated,exact,sizeof exact)&&!strcmp(exact,"X=2"),"symbolic result survives scope cleanup and GC");
        {
            auto dirs=rt.directories();pico_workbench_sampler sampler("X^2-4");double z;
            surface_check(sampler.sample(2,nullptr,z)==1&&z==0,"residual plot sample");
            object_g copied=integer::make(123);surface_check(rt.push(copied),"copy plotted result");
        }
        surface_check(rt.depth()==depth+2,"plot result survives scoped bindings");rt.drop(1);
        expression_g unsupported=pico_workbench_isolate("SIN(X^2)=X");surface_check(!unsupported,"unsupported symbolic isolation");rt.clear_error();
        strcpy(work.single,"X^2+1=0");surface_check(!pico_workbench_solve(work,false,values,residuals),"workbench no real solution");rt.clear_error();
        // Prime the simulator tail slot before queuing Esc.
        strcpy(work.single,"COS(X)=X");key_pop_all();key_push(33);key_pop();key_push(33);program::count_interrupted=33;
        surface_check(!pico_workbench_solve(work,false,values,residuals),"solver Esc cancellation");key_pop_all();rt.clear_error();
        {
            pico_workbench_sampler sampler("X^2");double value;
            program::halted=true;surface_check(sampler.sample(1,nullptr,value)==-2,"parameter sampler cancellation");
        }
        surface_check(!program::halted,"restore cancellation state after sampler");
        {
            pico_workbench_scope scope;surface_check(scope.entered&&scope.bind("A",42),"prepare allocation failure scope");
            surface_check(!rt.allocate(1024*1024),"bounded calculator allocation fails");
        }
        rt.clear_error();
        Settings.AngleMode(old_angle);
        surface_check(rt.directories()==dirs&&rt.depth()==depth+1&&rt.variables(0)->count()==variables,"workbench failure cleanup and temporary child removal");
        for(int i=0;i<4;++i){const char *names[]={"A","B","C","Z"};symbol_g name=symbol::make(names[i]);object_g value=directory::recall_all(name,false);surface_check(value&&value->as_int32(0,false)==11+i,"preserve preexisting parameter and Z values");}
        printf("PASS: workbench upstream solvers, isolation, parameter sampling and scope cleanup\n");
    }
    {
        pico_data_table table;table.demo();pico_data_summary summary;pico_data_fit fit;
        surface_check(pico_data_describe(table,summary),"data summary with upstream stats");
        surface_check(std::fabs(summary.values[0][2]-2.5)<1e-12&&std::fabs(summary.values[1][2]-6)<1e-12,"XY means");
        surface_check(pico_data_regress(table,fit)&&std::fabs(fit.intercept-1)<1e-10&&std::fabs(fit.slope-2)<1e-10,"upstream linear regression");
        for(unsigned i=0;i<table.rows;++i){table.points[i][0]=i+1;table.points[i][1]=3*std::pow(double(i+1),2);}
        table.model=3;surface_check(pico_data_regress(table,fit)&&std::fabs(fit.intercept-3)<1e-8&&std::fabs(fit.slope-2)<1e-8,"upstream power regression");
        table.model=1;for(unsigned i=0;i<table.rows;++i)table.points[i][1]=4+2*std::log(table.points[i][0]);
        surface_check(pico_data_regress(table,fit)&&std::fabs(fit.intercept-4)<1e-8&&std::fabs(fit.slope-2)<1e-8,"upstream logarithmic regression");
        table.model=2;for(unsigned i=0;i<table.rows;++i)table.points[i][1]=2*std::exp(.5*table.points[i][0]);
        surface_check(pico_data_regress(table,fit)&&std::fabs(fit.intercept-2)<1e-8&&std::fabs(fit.slope-.5)<1e-8,"upstream exponential regression");
        table.model=0;for(unsigned i=0;i<table.rows;++i)table.points[i][0]=1;
        surface_check(!pico_data_regress(table,fit),"reject constant X fit");rt.clear_error();
        surface_check(rt.depth()==depth+1,"data stats preserve stack");
        printf("PASS: upstream data summary/regression, invalid fits and stack preservation\n");
    }
    run_form_engine_tests();
    pico_notebook_engine_tests(surface_check);
    symbol_g *oldx=expression::independent,*oldy=expression::dependent;
    {
        pico_surface_evaluator e("X^2-Y^2");surface_check(e.valid(),"parse saddle");
        for(int j=0;j<17;++j) for(int i=0;i<17;++i)
        {
            if(i==8) rt.gc();
            double x=-3+i*0.375,y=-3+j*0.375,z;
            auto status=e.sample(x,y,z);
            surface_check(status==pico_surface_evaluator::finite && std::fabs(z-(x*x-y*y))<1e-10,"289 saddle samples");
            surface_check(rt.depth()==depth+1,"sample preserves stack depth");
        }
    }
    surface_check(expression::independent==oldx && expression::dependent==oldy,"restore plot bindings");
    {
        settings::SaveAngleMode radians(object::ID_Rad);
        pico_surface_evaluator e("SIN(X^2+Y^2)");double z;
        surface_check(e.valid() && e.sample(1,2,z)==pico_surface_evaluator::finite && std::fabs(z-std::sin(5.0))<1e-12,"ripple and radians");
    }
    {
        pico_surface_evaluator e("SQRT(-1)");double z;
        surface_check(e.valid() && e.sample(0,0,z)==pico_surface_evaluator::hole,"complex sample becomes a hole");
    }
    {
        pico_surface_evaluator e("7");double z;
        surface_check(e.valid() && e.sample(0,0,z)==pico_surface_evaluator::finite && z==7,"constant plane");
    }
    {
        pico_surface_evaluator e("1/(X-Y)");double z;
        surface_check(e.valid() && e.sample(1,1,z)==pico_surface_evaluator::hole,"singularity hole");
        surface_check(!rt.error() && e.sample(2,1,z)==pico_surface_evaluator::finite && z==1,"recover after singularity");
        program::halted=true;
        surface_check(e.sample(2,1,z)==pico_surface_evaluator::cancelled,"cancellation");
    }
    {
        pico_surface_evaluator e("X+Y",false);double z;
        surface_check(e.valid()&&e.sample(2,0,z)==pico_surface_evaluator::finite&&z==79,"2D curve binds X and preserves stored Y");
    }
    {
        pico_surface_evaluator a("X^2",false),b("X+Y",false),c("7",false);
        surface_check(a.valid()&&b.valid()&&c.valid(),"simultaneous native curve evaluators");
        pico_surface_evaluator *evaluators[]={&a,&b,&c};
        auto sample=[](void *opaque,int curve,double x,double y,double *z)->int {
            auto **e=static_cast<pico_surface_evaluator **>(opaque);
            rt.gc();auto result=e[curve]->sample(x,y,*z);
            return result==pico_surface_evaluator::finite?1:result==pico_surface_evaluator::hole?0:-1;
        };
        pico_plot_request request{};double bounds[]={-3,3,-2,4};memcpy(request.window,bounds,sizeof bounds);
        request.active[0]=request.active[1]=request.active[2]=1;double samples[900];
        surface_check(pico_plot_engine::sample_curves(request,sample,evaluators,samples)==1,"shared native engine with real DB48X and forced GC");
        for(int i=0;i<300;++i)
        {
            double x=pico_plot_engine::coordinate(-3,3,i,299);
            surface_check(std::fabs(samples[i]-x*x)<1e-12&&std::fabs(samples[300+i]-(x+77))<1e-12&&samples[600+i]==7,"native multi-curve numeric values and stored Y");
        }
    }
    {
        double scalar;
        surface_check(pico_formula_scalar("1/3",scalar)&&std::fabs(scalar-1.0/3)<1e-14,"calculus bound fraction expression");
        surface_check(pico_formula_scalar("PI",scalar)&&std::fabs(scalar-3.141592653589793)<1e-14,"calculus bound Pi constant");
        surface_check(!pico_formula_scalar("SQRT(-1)",scalar),"complex calculus bound rejected");rt.clear_error();
        expression_g derivative=pico_formula_calculus("X^3",false);char source[160];
        surface_check(derivative&&pico_formula_source(derivative,source,sizeof source),"symbolic derivative editable source");
        {pico_surface_evaluator evaluate(source,false);double value;surface_check(evaluate.valid()&&evaluate.sample(2,0,value)==pico_surface_evaluator::finite&&std::fabs(value-12)<1e-10,"symbolic derivative value");}
        expression_g primitive=pico_formula_calculus("X^2",true);
        surface_check(primitive&&pico_formula_source(primitive,source,sizeof source),"symbolic primitive editable source");
        {pico_surface_evaluator evaluate(source,false);double value;surface_check(evaluate.valid()&&evaluate.sample(3,0,value)==pico_surface_evaluator::finite&&std::fabs(value-9)<1e-10,"symbolic primitive value");}
        {
            settings::SaveAngleMode radians(object::ID_Rad);
            expression_g trig=pico_formula_calculus("SIN(X)",false);char trig_source[160];
            surface_check(trig&&pico_formula_source(trig,trig_source,sizeof trig_source),"symbolic trigonometric derivative");
            pico_surface_evaluator evaluate(trig_source,false);double value;surface_check(evaluate.valid()&&evaluate.sample(0,0,value)==pico_surface_evaluator::finite&&std::fabs(value-1)<1e-10,"trig derivative in radians");
        }
        surface_check(!pico_formula_calculus("SIN(X^2)",true)&&rt.error(),"unsupported primitive reports engine error");rt.clear_error();
        surface_check(!pico_formula_parse("X' 42 '"),"formula statements rejected");rt.clear_error();
        object_g definite=pico_formula_definite("X^2","0","1");char exact[64];
        surface_check(definite,"symbolic definite integral result");size_t rendered=definite->render(exact,sizeof exact);exact[rendered<sizeof exact?rendered:sizeof exact-1]=0;
        surface_check(!strcmp(exact,"¹/₃"),"exact definite integral equals one third");
        {char fraction_source[160];surface_check(pico_formula_source(definite,fraction_source,sizeof fraction_source),"exact definite result can become a curve");pico_surface_evaluator evaluate(fraction_source,false);double value;surface_check(evaluate.valid()&&evaluate.sample(0,0,value)==pico_surface_evaluator::finite&&std::fabs(value-1.0/3)<1e-12,"fraction result plots as a constant");}
        definite=pico_formula_definite("X^2","1","0");surface_check(definite,"reversed symbolic definite integral");
        rendered=definite->render(exact,sizeof exact);exact[rendered<sizeof exact?rendered:sizeof exact-1]=0;surface_check(!strcmp(exact,"-¹/₃"),"reversed exact integral sign");
        {
            settings::SaveAngleMode radians(object::ID_Rad);object_g trig_integral=pico_formula_definite("SIN(X)","0","PI");
            surface_check(trig_integral,"symbolic integral with PI bound");size_t n=trig_integral->render(exact,sizeof exact);exact[n<sizeof exact?n:sizeof exact-1]=0;
            char result_source[160];surface_check(pico_formula_source(trig_integral,result_source,sizeof result_source),"symbolic sine integral remains editable");
            pico_surface_evaluator evaluate(result_source,false);double value;surface_check(evaluate.valid()&&evaluate.sample(0,0,value)==pico_surface_evaluator::finite&&std::fabs(value-2)<1e-12,"symbolic sine integral evaluates to two");
        }
        object_g display=object_p(+primitive);grapher layout(300,150,Settings.ResultFont(),grob::pattern::black,grob::pattern::white,false,true);layout.duration=1000;
        grob_g image=display->graph(layout);surface_check(image&&image->width()<=300&&image->height()<=150,"bounded typeset formula preview");
        rt.gc();surface_check(pico_formula_source(primitive,source,sizeof source),"calculus result survives GC");
        surface_check(rt.depth()==depth+1,"calculus preserves stack sentinel");
    }
    {
        pico_surface_evaluator curve("X^2-4",false),line("X",false);pico_surface_evaluator *evaluators[]={&curve,&line};
        auto sample=[](void *opaque,int c,double x,double y,double *z)->int {
            auto **e=static_cast<pico_surface_evaluator **>(opaque);rt.gc();auto result=e[c]->sample(x,y,*z);
            return result==pico_surface_evaluator::finite?1:result==pico_surface_evaluator::hole?0:-1;
        };
        auto roots=pico_plot_analysis::run(pico_plot_analysis::operation::roots,sample,evaluators,0,1,-3,3,0);
        surface_check(roots.state==pico_plot_analysis::status::ok&&roots.count==2&&std::fabs(roots.points[0].x+2)<1e-8,"real DB48X native roots");
        auto crossings=pico_plot_analysis::run(pico_plot_analysis::operation::intersections,sample,evaluators,0,1,-3,3,0);
        surface_check(crossings.state==pico_plot_analysis::status::ok&&crossings.count==2,"real DB48X intersections");
        auto between=pico_plot_analysis::run(pico_plot_analysis::operation::between,sample,evaluators,0,1,0,1,0);
        surface_check(between.state==pico_plot_analysis::status::ok&&std::fabs(between.value-25.0/6)<1e-8,"real DB48X area between curves");
        auto integral=pico_plot_analysis::run(pico_plot_analysis::operation::integral,sample,evaluators,0,1,0,1,0);
        surface_check(integral.state==pico_plot_analysis::status::ok&&std::fabs(integral.value+11.0/3)<1e-10,"real DB48X signed integration");
        printf("PASS: native analysis with real DB48X, symbolic calculus, editable results, bounded preview and GC/stack preservation\n");
    }
    surface_check(!program::halted,"restore cancellation state");
    {
        pico_surface_evaluator e("X' 42 '");surface_check(!e.valid(),"reject multiple statements");
    }
    rt.clear_error();
    surface_check(Settings.Precision()==precision && Settings.HardwareFloatingPoint()==hardware,"restore calculator precision and arithmetic mode");
    surface_check(commands("X Y"),"read stored variables");
    char buffer[64]={};object_p y=rt.pop();size_t rendered=y->render(buffer,sizeof buffer);buffer[rendered<sizeof buffer?rendered:sizeof buffer-1]=0;
    surface_check(!strcmp(buffer,"77"),"stored Y preserved");
    object_p x=rt.pop();rendered=x->render(buffer,sizeof buffer);buffer[rendered<sizeof buffer?rendered:sizeof buffer-1]=0;
    surface_check(!strcmp(buffer,"99"),"stored X preserved");
    object_p sentinel=rt.pop();rendered=sentinel->render(buffer,sizeof buffer);buffer[rendered<sizeof buffer?rendered:sizeof buffer-1]=0;
    surface_check(!strcmp(buffer,"42") && rt.depth()==depth,"stack values preserved");
    {
        pico_lua_result result;const char *script="print(calc.eval('1/3')); print(calc.eval('2^10'))";
        surface_check(pico_lua_run(script,strlen(script),&result),"Lua and actual calculator integration");
        surface_check(!strcmp(result.output,"¹/₃\n1 024\n") && rt.depth()==depth,"Lua RPL values and preserved stack");
        printf("PASS: Lua/DB48X integration peak %u bytes\n",unsigned(result.peak));
    }
    {
        pico_lua_result result;pico_lua_session *console=pico_lua_open(&result);surface_check(console,"real Lua console open");
        const char *code="fraction=calc.eval('1/3')";
        surface_check(pico_lua_submit(console,code,strlen(code),1,&result)==1,"console calculator binding");
        surface_check(pico_lua_submit(console,"fraction",8,1,&result)==1&&!strcmp(result.output,"¹/₃\n")&&rt.depth()==depth,"console globals and calculator stack");
        pico_lua_close(console);
    }
    {
        pico_lua_result result;const char *code="plot.clear();plot.window(-3,3,-2,4);plot.function2d(function(x)return x*x end);plot.show();plot.surface(function(x,y)return x*x-y*y end);plot.show();print(calc.eval('1/3'))";
        surface_check(pico_lua_run(code,strlen(code),&result),"Lua 2D/3D through the shared native sampler");
        surface_check(!strcmp(result.output,"¹/₃\n")&&rt.depth()==depth,"plot callbacks preserve calculator stack and bridge");
        printf("PASS: Lua native plots and calculator bridge peak %u bytes\n",unsigned(result.peak));
    }
    {
        char result[256];
        surface_check(pico_lua_eval("1/3",result,sizeof result) && !strcmp(result,"¹/₃"),"Lua calculator bridge exact fraction");
        surface_check(rt.depth()==depth,"Lua bridge preserves stack");
        surface_check(!pico_lua_eval("X' 2 '",result,sizeof result),"Lua bridge rejects multiple statements");
        program_g parsed=program::parse(utf8("« 2 3 + »"),strlen("« 2 3 + »"));
        object_g code;for(object_p item:*parsed)code=item;
        surface_check(rt.push(code)&&object::static_object(object::ID_Debug)->evaluate()==object::OK,"start native RPL debugger");
        surface_check(rt.run_stepping(),"debugger next instruction");
        for(int n=0;n<3;++n)surface_check(object::static_object(object::ID_SingleStep)->evaluate()==object::OK,"RPL single step");
        surface_check(rt.depth()==depth+1,"RPL debugger result depth");
        object_p value=rt.pop();result[0]=0;size_t written=value->render(result,sizeof result);result[written<sizeof result?written:sizeof result-1]=0;surface_check(!strcmp(result,"5"),"RPL debugger arithmetic result");
        object::static_object(object::ID_Kill)->evaluate();
        surface_check(!program::halted && !rt.run_stepping(),"RPL debugger cleanup");
        parsed=program::parse(utf8("« « 2 3 + » EVAL 2 * »"),strlen("« « 2 3 + » EVAL 2 * »"));
        for(object_p item:*parsed)code=item;
        surface_check(rt.push(code)&&object::static_object(object::ID_Debug)->evaluate()==object::OK,"debug nested program");
        surface_check(object::static_object(object::ID_StepOver)->evaluate()==object::OK,"step over program literal");
        surface_check(object::static_object(object::ID_StepOver)->evaluate()==object::OK,"step over nested EVAL");
        surface_check(object::static_object(object::ID_Continue)->evaluate()==object::OK,"continue debugger");
        value=rt.pop();written=value->render(result,sizeof result);result[written<sizeof result?written:sizeof result-1]=0;
        surface_check(!strcmp(result,"10") && rt.depth()==depth,"step-over/continue result");
        object::static_object(object::ID_Kill)->evaluate();
        const char broken[]="«\n2 3 +\n\"unterminated";
        parsed=program::parse(utf8(broken),strlen(broken));
        surface_check(!parsed && rt.error(),"RPL syntax diagnostic");
        uintptr_t pos=reinterpret_cast<uintptr_t>(rt.source()),base=reinterpret_cast<uintptr_t>(broken);
        surface_check(pos>=base && pos<=base+strlen(broken),"RPL source error location");rt.clear_error();
        parsed=program::parse(utf8("'X^2'"),strlen("'X^2'"));object_g equation;for(object_p item:*parsed)equation=item;
        text_g editable=equation->as_text(true);size_t length;utf8 bytes=editable->value(&length);
        surface_check(length>=2&&length<=161&&bytes[0]=='\''&&bytes[length-1]=='\'',"editable legacy function scene capture");
        char formula[160]={};memcpy(formula,bytes+1,length-2);surface_check(pico_graph_scene::formula(formula),"scene formula UTF-8 source");
        puts("PASS: real Lua-to-RPL bridge and native RPL single-step/debugger cleanup");
    }
    puts("PASS: actual DB48X 3D evaluator, 289 saddle samples, constant plane, domain holes, cancellation, GC/stack and stored X/Y preservation");
    fflush(stdout);std::exit(0);
}
