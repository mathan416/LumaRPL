#pragma once
#include <stddef.h>
extern "C" int pico_compute_network_begin();
extern "C" void pico_compute_network_end();
enum class pico_workspace_owner {none,editor,plot,source,numeric};
void *pico_workspace_acquire(pico_workspace_owner owner,size_t size);
void pico_workspace_release(void *pointer);
size_t pico_workspace_peak();
struct pico_workspace_lease
{
    void *data;
    pico_workspace_lease(pico_workspace_owner owner,size_t size):data(pico_workspace_acquire(owner,size)){}
    ~pico_workspace_lease(){release();}
    void release(){if(data)pico_workspace_release(data);data=nullptr;}
    bool acquire(pico_workspace_owner owner,size_t size){if(data)return false;data=pico_workspace_acquire(owner,size);return data!=nullptr;}
    pico_workspace_lease(const pico_workspace_lease &)=delete;
};
