#pragma once
#include "form_document.h"
#include "resource_file.h"
inline bool pico_form_save(const char *path,const pico_form_document &form)
{
    uint8_t data[pico_form_document::bytes];size_t size=form.encode(data,sizeof data);return size&&pico_resource_save(path,data,size);
}
inline bool pico_form_load(const char *path,pico_form_document &form,bool &backup)
{
    uint8_t data[pico_form_document::bytes];
    return pico_resource_load(path,data,sizeof data,backup,[&](size_t size){return form.decode(data,size);});
}
