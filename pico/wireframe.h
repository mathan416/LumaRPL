#pragma once
#include <cmath>
// Geometry stays independent of the calculator arena and display driver.
namespace pico_wireframe {
constexpr int side=17;
struct point { float x,y; };
struct camera {
    float yaw=0.65f,pitch=0.55f,zoom=1;
    point project(float x,float y,float z) const
    {
        float horizontal=std::cos(yaw)*x-std::sin(yaw)*y;
        float depth=std::sin(yaw)*x+std::cos(yaw)*y;
        return {160+70*zoom*horizontal,155-70*zoom*(std::sin(pitch)*depth+std::cos(pitch)*z)};
    }
    void rotate(float horizontal,float vertical)
    {
        yaw=std::remainder(yaw+horizontal,6.2831853f);
        pitch=std::fmax(-1.4f,std::fmin(1.4f,pitch+vertical));
    }
    void magnify(float factor) { zoom=std::fmax(0.4f,std::fmin(2.5f,zoom*factor)); }
};
inline float normalize(double value,double low,double high)
{
    if(low==high) return 0;
    // Division before subtraction avoids overflow for opposite large bounds.
    double scale=std::fmax(std::fabs(low),std::fabs(high));
    return float(2*((value/scale-low/scale)/(high/scale-low/scale))-1);
}
}
