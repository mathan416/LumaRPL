#pragma once
#include "data_table.h"
#include "graph_scene.h"
inline bool pico_data_formula(unsigned model,double a,double b,char *out,size_t capacity)
{
    if(model>3||!std::isfinite(a)||!std::isfinite(b)||std::fabs(a)>1e100||std::fabs(b)>1e100)return false;
    const char *formats[]={"(%.17g)+(%.17g)*X","(%.17g)+(%.17g)*LN(X)","(%.17g)*EXP((%.17g)*X)","(%.17g)*X^(%.17g)"};
    int n=snprintf(out,capacity,formats[model],a,b);return n>0&&size_t(n)<capacity;
}
inline bool pico_data_window(const pico_data_table &data,double *window)
{
    if(!data.valid()||!data.rows)return false;
    for(int c=0;c<2;++c)
    {
        double low=data.points[0][c],high=low;
        for(unsigned i=1;i<data.rows;++i){low=std::fmin(low,data.points[i][c]);high=std::fmax(high,data.points[i][c]);}
        double pad=std::fmax((high-low)*.1,std::fmax(1.0,std::fabs(low)*.1));
        window[2*c]=std::fmax(-1e100,low-pad);window[2*c+1]=std::fmin(1e100,high+pad);
    }
    return pico_graph_scene::bounds(window);
}
inline bool pico_data_curve_scene(const pico_data_table &data,unsigned model,double a,double b,pico_graph_scene &scene)
{
    pico_graph_scene next=scene;
    if(!pico_data_formula(model,a,b,next.curve[0],160)||!pico_data_window(data,next.window))return false;
    next.curve[1][0]=next.curve[2][0]=0;next.multiple=1;if(!next.valid())return false;scene=next;return true;
}
