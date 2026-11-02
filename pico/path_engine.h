#pragma once
#include "path_scene.h"
#include "workbench_engine.h"
struct pico_path_sampler
{
    pico_workbench_scope scope;
    settings::SaveHardwareFloatingPoint hardware;
    expression_g x,y,radius;
    const pico_path_scene &scene;
    explicit pico_path_sampler(const pico_path_scene &s):hardware(true),scene(s)
    {
        if(!scope.entered||!s.valid())return;
        if(s.polar)radius=pico_formula_parse(s.radius);
        else{x=pico_formula_parse(s.x);y=pico_formula_parse(s.y);}
    }
    bool valid()const{return scope.entered&&(scene.polar?bool(radius):bool(x)&&bool(y))&&!rt.error();}
    int sample(double t,double &px,double &py)
    {
        cleaner temporary;
        if(program::interrupted())return -2;
        if(!valid()||!scope.bind("T",t))return -1;
        bool ok;
        if(scene.polar)
        {
            double r;ok=pico_workbench_value(radius,r);
            if(ok){double theta=pico_path_scene::radians(t,scene.angle);px=r*std::cos(theta);py=r*std::sin(theta);ok=std::isfinite(px)&&std::isfinite(py);}
        }
        else ok=pico_workbench_value(x,px)&&pico_workbench_value(y,py);
        if(program::halted){rt.clear_error();return -2;}
        if(rt.error()&&!strcmp(cstring(rt.error()),"Out of memory"))return -1;
        if(!ok){rt.clear_error();return 0;}return 1;
    }
};
