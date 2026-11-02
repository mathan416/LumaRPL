#pragma once
#include "lua_device.h"
#include "target.h"
inline bool pico_lua_canvas(surface &screen,int op,const int *a,pattern c)
{
    switch(op)
    {
    case PICO_LUA_CLEAR:screen.fill(c);return true;
    case PICO_LUA_PIXEL:screen.fill(a[0],a[1],a[0],a[1],c);return true;
    case PICO_LUA_LINE:screen.line(a[0],a[1],a[2],a[3],1,c);return true;
    case PICO_LUA_RECT:
        if(a[2]&&a[3])
        {
            int x=a[0],y=a[1],r=x+a[2]-1,b=y+a[3]-1;
            if(a[4])screen.fill(x,y,r,b,c);
            else{screen.line(x,y,r,y,1,c);screen.line(x,b,r,b,1,c);screen.line(x,y,x,b,1,c);screen.line(r,y,r,b,1,c);}
        }return true;
    case PICO_LUA_CIRCLE:screen.circle(a[0],a[1],a[2],a[4]?0:1,c);return true;
    default:return false;
    }
}
