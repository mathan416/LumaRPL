#pragma once
#include "sprite.h"
#include "resource_file.h"
inline bool pico_sprite_save(const char *path,const pico_sprite &sprite){uint8_t data[pico_sprite::bytes];sprite.encode(data);return pico_resource_save(path,data,sizeof data);}
inline bool pico_sprite_load(const char *path,pico_sprite &sprite,bool &backup){uint8_t data[pico_sprite::bytes];return pico_resource_load(path,data,sizeof data,backup,[&](size_t n){return sprite.decode(data,n);});}
