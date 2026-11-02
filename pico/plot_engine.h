#pragma once
#include "plot_api.h"
#include "wireframe.h"
#include <cmath>
namespace pico_plot_engine {
constexpr int columns=300,curves=3;
typedef bool (*poll_function)(void *,int row);
inline bool bounds(const double *v)
{
    for(int i=0;i<4;++i)if(!std::isfinite(v[i])||std::fabs(v[i])>1e100)return false;
    return v[0]<v[1]&&v[2]<v[3];
}
inline double coordinate(double low,double high,int index,int last)
{double t=double(index)/last;return low*(1-t)+high*t;}
inline int sample_curves(const pico_plot_request &request,pico_plot_sample sample,void *context,
                         double *values,poll_function poll=nullptr,void *poll_context=nullptr)
{
    if(!bounds(request.window))return -1;
    for(int c=0;c<curves;++c)
    {
        for(int i=0;i<columns;++i)values[c*columns+i]=NAN;
        if(!request.active[c])continue;
        for(int i=0;i<columns;++i)
        {
            if(poll&&poll(poll_context,-1))return -2;
            double value=NAN,x=coordinate(request.window[0],request.window[1],i,columns-1);
            int result=sample(context,c,x,0,&value);if(result<0)return result;
            if(result==1&&std::isfinite(value))values[c*columns+i]=value;
        }
    }
    return 1;
}
typedef int (*path_function)(void *,double,double *,double *);
// Same native 300-sample budget and cancellation contract as function plots.
inline int sample_path(double first,double last,path_function sample,void *context,double *values,poll_function poll=nullptr,void *poll_context=nullptr)
{
    if(!sample||!values||!std::isfinite(first)||!std::isfinite(last)||first>=last||std::fabs(first)>1e100||std::fabs(last)>1e100)return -1;
    for(int i=0;i<columns;++i)
    {
        values[i]=values[columns+i]=NAN;if(poll&&poll(poll_context,-1))return -2;
        double x,y;int result=sample(context,coordinate(first,last,i,columns-1),&x,&y);
        if(result<0)return result;
        if(result==1&&std::isfinite(x)&&std::isfinite(y)){values[i]=x;values[columns+i]=y;}
    }
    return 1;
}
inline int sample_surface(const pico_plot_request &request,pico_plot_sample sample,void *context,
                          double *values,double &minimum,double &maximum,int &count,
                          poll_function poll=nullptr,void *poll_context=nullptr)
{
    if(!bounds(request.bounds3d))return -1;count=0;
    constexpr int side=pico_wireframe::side;
    for(int j=0;j<side;++j)
    {
        for(int i=0;i<side;++i)
        {
            if(poll&&poll(poll_context,j))return -2;
            double x=coordinate(request.bounds3d[0],request.bounds3d[1],i,side-1);
            double y=coordinate(request.bounds3d[2],request.bounds3d[3],j,side-1),value=NAN;
            int result=sample(context,0,x,y,&value);if(result<0)return result;
            values[j*side+i]=NAN;
            if(result!=1||!std::isfinite(value))continue;
            if(!count)minimum=maximum=value;
            else{minimum=std::fmin(minimum,value);maximum=std::fmax(maximum,value);}
            ++count;values[j*side+i]=value;
        }
    }
    if(!count)return 0;
    for(int i=0;i<side*side;++i)if(std::isfinite(values[i]))values[i]=pico_wireframe::normalize(values[i],minimum,maximum);
    return 1;
}
}
