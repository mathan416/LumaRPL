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
    uint8_t data[pico_workbench::bytes];pico_workbench_diagnostic local;pico_resource_trace trace;
    bool ok=pico_resource_load(path,data,sizeof data,backup,[&](size_t size)
        {return candidate.decode(data,size,trace.attempt?&local.backup_decode:&local.primary_decode);},&trace);
    local.primary=trace.primary;local.backup=trace.backup;if(diagnostic)*diagnostic=local;return ok;
}
