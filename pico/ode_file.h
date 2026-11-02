#pragma once
#include "ode.h"
#include "resource_file.h"
inline bool pico_ode_save(const char *path,const pico_ode_scene &scene)
{uint8_t data[pico_ode_scene::bytes];size_t n=scene.encode(data,sizeof data);return n&&pico_resource_save(path,data,n);}
inline bool pico_ode_load(const char *path,pico_ode_scene &scene,bool &backup)
{
    uint8_t data[pico_ode_scene::bytes];
    return pico_resource_load(path,data,sizeof data,backup,[&](size_t size){return scene.decode(data,size);});
}
