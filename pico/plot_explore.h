#pragma once
#include <cmath>
namespace pico_plot_explore {
struct selection {
    int count=0;
    double first=0,last=0;
    void mark(double x) { if(count!=1){first=x;count=1;}else{last=x;count=2;} }
    void clear(){count=0;}
    bool ready() const{return count==2&&first!=last;}
};
inline bool tangent(double x,double at,double y,double slope,double &value)
{
    value=std::fma(slope,x-at,y);return std::isfinite(value);
}
}
