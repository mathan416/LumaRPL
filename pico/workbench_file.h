#pragma once
#include "workbench.h"
#include "resource_file.h"
inline bool pico_workbench_save(const char *path,const pico_workbench &work)
{
    uint8_t data[pico_workbench::bytes];size_t size=work.encode(data,sizeof data);
    return size&&pico_resource_save(path,data,size);
}
struct pico_workbench_diagnostic {pico_resource_diagnostic primary,backup;const char *primary_decode="Not read",*backup_decode="Not read";};
inline bool pico_workbench_load(const char *path,pico_workbench &candidate,bool &backup,pico_workbench_diagnostic *diagnostic=nullptr)
{
    uint8_t data[pico_workbench::bytes];size_t size;backup=false;pico_workbench_diagnostic local;
    if(pico_resource_read(path,data,sizeof data,size,&local.primary)&&candidate.decode(data,size,&local.primary_decode)){pico_storage_verified(path);if(diagnostic)*diagnostic=local;return true;}
    for(const char *suffix:{".rollback",".bak"})
    {
        char alternate[340];if(snprintf(alternate,sizeof alternate,"%s%s",path,suffix)>=int(sizeof alternate))return false;
        if(pico_resource_read(alternate,data,sizeof data,size,&local.backup)&&candidate.decode(data,size,&local.backup_decode))
        {backup=true;pico_storage_recovered(path,alternate);if(diagnostic)*diagnostic=local;return true;}
    }
    if(diagnostic)*diagnostic=local;return false;
}
