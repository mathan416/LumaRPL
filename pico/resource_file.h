#pragma once
#include "storage.h"
#include <cstdio>
#include <stddef.h>
#include <initializer_list>
inline bool pico_resource_save(const char *path,const void *data,size_t length)
{
    char temp[330];if(snprintf(temp,sizeof temp,"%s.tmp",path)>=int(sizeof temp))return false;
    FIL file={};UINT written=0;pico_io_reset();bool ok=pico_f_open(&file,temp,FA_WRITE|FA_CREATE_ALWAYS)==FR_OK;
    if(ok){ok=pico_f_write(&file,data,length,&written)==FR_OK&&written==length;if(pico_f_close(&file)!=FR_OK)ok=false;}
    ok=ok&&!pico_io_failed()&&pico_commit_state(temp,path);
    // A failed save keeps the previous file; never leave the partial copy behind.
    if(!ok)pico_f_unlink(temp);
    return ok;
}
struct pico_resource_diagnostic {FRESULT open=FR_OK,read=FR_OK,close=FR_OK;FSIZE_t bytes=0;};
inline bool pico_resource_read(const char *path,void *data,size_t capacity,size_t &length,pico_resource_diagnostic *diagnostic=nullptr)
{
    FIL file={};length=0;pico_resource_diagnostic local;
    local.open=pico_f_open(&file,path,FA_READ);if(local.open!=FR_OK){if(diagnostic)*diagnostic=local;return false;}
    local.bytes=f_size(&file);
    UINT actual=0;bool ok=f_size(&file)<=capacity;
    if(ok){local.read=pico_f_read(&file,data,capacity,&actual);ok=local.read==FR_OK;}
    local.close=pico_f_close(&file);if(local.close!=FR_OK)ok=false;length=actual;if(diagnostic)*diagnostic=local;return ok;
}

// Optional record of what each load attempt saw, for user-facing diagnostics.
// `attempt` is 0 while the primary is decoded and 1 for a recovery copy.
struct pico_resource_trace {pico_resource_diagnostic primary,backup;int attempt=0;};
template<class Decode>
inline bool pico_resource_load(const char *path,void *data,size_t capacity,bool &backup,Decode decode,pico_resource_trace *trace=nullptr)
{
    size_t size;backup=false;if(trace)trace->attempt=0;
    if(pico_resource_read(path,data,capacity,size,trace?&trace->primary:nullptr)&&decode(size)){pico_storage_verified(path);return true;}
    for(const char *suffix:{".rollback",".bak"})
    {
        char alternate[340];int n=snprintf(alternate,sizeof alternate,"%s%s",path,suffix);
        if(n<0||size_t(n)>=sizeof alternate)return false;
        if(trace)trace->attempt=1;
        if(pico_resource_read(alternate,data,capacity,size,trace?&trace->backup:nullptr)&&decode(size))
        {backup=true;pico_storage_recovered(path,alternate);return true;}
    }
    return false;
}
