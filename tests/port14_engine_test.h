#pragma once
#include "path_engine.h"
#include "data_curve.h"
#include "form_engine.h"
inline void run_port14_engine_tests()
{
    size_t depth=rt.depth(),dirs=rt.directories(),variables=rt.variables(0)->count();auto precision=Settings.Precision();bool hardware=Settings.HardwareFloatingPoint();
    {
        settings::SaveAngleMode angle(object::ID_Rad);pico_path_scene path;
        pico_path_sampler sampler(path);double x,y;
        surface_check(sampler.valid()&&sampler.sample(1.5707963267948966,x,y)==1&&std::fabs(x)<1e-12&&std::fabs(y-1)<1e-12,"native parametric circle");
        auto component=[](void *opaque,int c,double t,double,double *v)->int{double x,y;int result=static_cast<pico_path_sampler *>(opaque)->sample(t,x,y);if(result==1)*v=c?y:x;return result;};
        auto dx=pico_plot_analysis::run(pico_plot_analysis::operation::derivative,component,&sampler,0,0,0,0,0);
        auto dy=pico_plot_analysis::run(pico_plot_analysis::operation::derivative,component,&sampler,1,1,0,0,0);double slope;
        surface_check(dx.state==pico_plot_analysis::status::ok&&dy.state==pico_plot_analysis::status::ok&&pico_path_scene::direction(dx.value,dy.value,slope)==pico_path_scene::tangent::vertical,"native path derivative resolves vertical tangent");
        for(int i=0;i<300;++i){rt.gc();surface_check(sampler.sample(i*.01,x,y)==1&&std::fabs(x*x+y*y-1)<1e-12,"path sampling and GC");}
    }
    {
        settings::SaveAngleMode angle(object::ID_Rad);expression_g derivative=pico_formula_calculus("COS(T)",false,"T");pico_path_scene path;
        surface_check(derivative&&pico_formula_source(derivative,path.x,160),"path editor differentiates T rather than X");strcpy(path.y,"0");
        pico_path_sampler sampler(path);double x,y;surface_check(sampler.sample(1.5707963267948966,x,y)==1&&std::fabs(x+1)<1e-12,"T derivative formula evaluates correctly");
    }
    {
        pico_workbench_scope parent;surface_check(parent.entered&&parent.bind("T",123),"prepare existing T binding");
        {settings::SaveAngleMode angle(object::ID_Deg);pico_path_scene path;path.angle=0;pico_path_sampler sampler(path);double x,y;surface_check(sampler.sample(90,x,y)==1&&std::fabs(x)<1e-12&&std::fabs(y-1)<1e-12,"parametric trig honors degrees");}
        symbol_g name=symbol::make("T");object_g original=directory::recall_all(name,false);algebraic_g value=original?original->as_algebraic():nullptr;double t;
        surface_check(pico_workbench_double(value,t)&&t==123,"path restores preexisting T");
    }
    {
        settings::SaveAngleMode angle(object::ID_Deg);pico_path_scene path;path.polar=1;path.angle=0;strcpy(path.radius,"2");path.last=360;
        pico_path_sampler sampler(path);double x,y;surface_check(sampler.valid()&&sampler.sample(90,x,y)==1&&std::fabs(x)<1e-12&&std::fabs(y-2)<1e-12,"polar degrees match native trig");
    }
    {
        settings::SaveAngleMode angle(object::ID_Rad);pico_path_scene path;path.polar=1;strcpy(path.radius,"-2");
        pico_path_sampler sampler(path);double x,y;surface_check(sampler.sample(0,x,y)==1&&x==-2&&y==0,"negative polar radius");
    }
    {
        pico_path_scene path;strcpy(path.x,"SQRT(T)");strcpy(path.y,"T");pico_path_sampler sampler(path);double x,y;
        surface_check(sampler.sample(-1,x,y)==0&&!rt.error(),"path domain gap");
        surface_check(sampler.sample(4,x,y)==1&&x==2&&y==4,"path recovers after domain gap");
        program::halted=true;surface_check(sampler.sample(1,x,y)==-2,"path cancellation");
    }
    {
        pico_form_document form;pico_form_curve curve{form,1};double value;
        surface_check(curve.sample(2,value)==1&&value==6,"Ohm form resistance sweep");
        surface_check(curve.sample(0,value)==0&&!rt.error(),"form sweep division by zero is gap");
        surface_check(curve.sample(4,value)==1&&value==3&&!strcmp(form.fields[1].input,"4"),"form sweep original values kept");
        program::halted=true;surface_check(curve.sample(2,value)==1&&value==6&&program::halted,"swept form preserves old stop state without false cancellation");program::halted=false;
        key_pop_all();key_push(33);key_pop();key_push(33);program::count_interrupted=33;
        surface_check(curve.sample(2,value)==-2&&!rt.error(),"swept form reports Esc cancellation");key_pop_all();
        surface_check(curve.sample(4,value)==1&&value==3,"swept form recovers after cancellation");
        strcpy(form.fields[0].name,"A");strcpy(form.fields[0].input,"2");strcpy(form.fields[1].name,"B");strcpy(form.fields[1].input,"A*3");strcpy(form.formula,"B+1");
        pico_form_curve dependent{form,0};surface_check(dependent.sample(4,value)==1&&value==13,"sweep updates dependent input expressions");
        for(int i=0;i<300;++i){rt.gc();surface_check(dependent.sample(4,value)==1&&value==13,"repeated swept form GC cleanup");}
    }
    {
        const char *expected[]={"(1)+(2)*X","(1)+(2)*LN(X)","(1)*EXP((2)*X)","(1)*X^(2)"};double answers[]={5,1+2*std::log(2.0),std::exp(4.0),4};
        for(unsigned i=0;i<4;++i)
        {
            char source[160];surface_check(pico_data_formula(i,1,2,source,sizeof source)&&!strcmp(source,expected[i]),"fit expression generation");
            pico_workbench_sampler sampler(source);double value;surface_check(sampler.sample(2,nullptr,value)==1&&std::fabs(value-answers[i])<1e-10,"fitted expression evaluates through native core");
        }
    }
    surface_check(rt.depth()==depth&&rt.directories()==dirs&&rt.variables(0)->count()==variables,"port14 preserves caller variables and stack");
    surface_check(Settings.Precision()==precision&&Settings.HardwareFloatingPoint()==hardware&&!program::halted,"port14 restores evaluation settings and cancellation state");
    puts("PASS: real-engine form sweeps, dependent inputs, parametric/polar paths, GC, domains and fitted expressions");
}
