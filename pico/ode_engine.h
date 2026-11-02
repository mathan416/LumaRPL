#pragma once
#include "ode.h"
#include "workbench_engine.h"
struct pico_ode_context
{
    pico_workbench_scope scope;settings::SaveHardwareFloatingPoint hardware;
    expression_g equation;pico_ode_scene &scene;
    double *values=nullptr,first=0,last=1;pico_ode::result info;
    explicit pico_ode_context(pico_ode_scene &model):hardware(true),equation(pico_formula_parse(model.formula)),scene(model){}
    bool valid()const{return scope.entered&&equation&&!rt.error()&&scene.valid();}
    int slope(double x,double y,double &value)
    {
        cleaner temporary;if(program::interrupted()){rt.clear_error();return -2;}
        if(!scope.bind("X",x)||!scope.bind("Y",y))return -1;
        bool ok=pico_workbench_value(equation,value);
        if(program::halted){rt.clear_error();return -2;}
        if(rt.error()&&!strcmp(cstring(rt.error()),"Out of memory"))return -1;
        if(!ok){rt.clear_error();return 0;}return 1;
    }
    static int callback(void *opaque,double x,double y,double *value){return static_cast<pico_ode_context *>(opaque)->slope(x,y,*value);}
    int rebuild(const double *window,double *samples)
    {
        first=window[0];last=window[1];values=samples;info=pico_ode::solve(callback,this,scene.x0,scene.y0,first,last,samples);if(info.code<0)return info.code;
        for(int j=0;j<15;++j)for(int i=0;i<21;++i)
        {
            samples[300+j*21+i]=NAN;if(!(scene.flags&1))continue;
            double x=window[0]+(window[1]-window[0])*(i+.5)/21,y=window[2]+(window[3]-window[2])*(j+.5)/15,f;
            int code=slope(x,y,f);if(code<0)return code;if(code==1)samples[300+j*21+i]=f;
        }
        return 1;
    }
    int sample(double x,double &y)const
    {y=(scene.flags&2)?pico_ode::interpolate(values,first,last,x):NAN;return std::isfinite(y)?1:0;}
};
