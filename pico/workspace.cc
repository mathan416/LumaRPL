#include "workspace.h"
#include <stdint.h>
alignas(8) static uint8_t shared_workspace[16384];
static pico_workspace_owner owner=pico_workspace_owner::none;
static size_t peak;
extern "C" __attribute__((weak)) int pico_compute_network_begin(){return 1;}
extern "C" __attribute__((weak)) void pico_compute_network_end(){}

void *pico_workspace_acquire(pico_workspace_owner requested,size_t size)
{
    if(owner!=pico_workspace_owner::none||requested==pico_workspace_owner::none||!size||size>sizeof shared_workspace)return nullptr;
    if(requested==pico_workspace_owner::numeric&&!pico_compute_network_begin())return nullptr;
    owner=requested;if(size>peak)peak=size;return shared_workspace;
}
void pico_workspace_release(void *pointer){if(pointer==shared_workspace){if(owner==pico_workspace_owner::numeric)pico_compute_network_end();owner=pico_workspace_owner::none;}}
size_t pico_workspace_peak(){return peak;}
