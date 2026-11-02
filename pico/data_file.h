#pragma once
#include "data_table.h"
#include "resource_file.h"
#include <new>
inline bool pico_data_load(const char *path,pico_data_table &table,bool &backup)
{
    char *data=static_cast<char *>(malloc(16385));if(!data)return false;
    bool ok=pico_resource_load(path,data,16384,backup,[&](size_t size){return table.parse_csv(data,size);});free(data);return ok;
}
inline bool pico_data_save(const char *path,const pico_data_table &table)
{
    char *data=static_cast<char *>(malloc(16385));if(!data)return false;size_t size=table.csv(data,16385);
    bool ok=size&&pico_resource_save(path,data,size);free(data);return ok;
}
