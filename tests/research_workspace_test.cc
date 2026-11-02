#include "workspace.h"
#include <cassert>
#include <cstdio>
static bool blocked=true;static unsigned starts=0,ends=0;
extern "C" int pico_compute_network_begin(){if(blocked)return 0;++starts;return 1;}
extern "C" void pico_compute_network_end(){++ends;}
int main(){assert(!pico_workspace_acquire(pico_workspace_owner::numeric,1024));assert(!pico_workspace_acquire(pico_workspace_owner::numeric,16385));assert(!pico_workspace_acquire(pico_workspace_owner::numeric,0));blocked=false;{pico_workspace_lease simulation(pico_workspace_owner::numeric,16384);assert(simulation.data&&starts==1);assert(!pico_workspace_acquire(pico_workspace_owner::plot,7200));pico_workspace_release(reinterpret_cast<void *>(1));assert(ends==0);simulation.release();assert(ends==1&&!simulation.data);pico_workspace_lease plot(pico_workspace_owner::plot,7200);assert(plot.data&&starts==1);}assert(ends==1&&pico_workspace_peak()==16384);{pico_workspace_lease fit(pico_workspace_owner::numeric,8192);assert(fit.data&&starts==2);}assert(ends==2);puts("PASS: numerical/network admission, shared workspace exclusion, oversize/empty requests, release and full-size simulation reuse");}
