#pragma once
#include "notebook.h"
#include "resource_file.h"
#include <cstdlib>
inline bool pico_notebook_save(const char *path,const pico_notebook &book)
{
    auto *data=static_cast<uint8_t *>(malloc(pico_notebook::bytes));if(!data)return false;
    size_t size=book.encode(data,pico_notebook::bytes);bool ok=size&&pico_resource_save(path,data,size);free(data);return ok;
}
inline bool pico_notebook_load(const char *path,pico_notebook &book,bool &backup)
{
    auto *data=static_cast<uint8_t *>(malloc(pico_notebook::bytes));if(!data)return false;
    bool ok=pico_resource_load(path,data,pico_notebook::bytes,backup,[&](size_t size){return book.decode(data,size);});free(data);return ok;
}
