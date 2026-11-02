#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Samples return 1 finite, 0 domain hole, -1 error, -2 interrupted.
 * Lua callbacks catch every Lua error before returning across the C++ boundary. */
typedef int (*pico_plot_sample)(void *,int curve,double x,double y,double *z);
typedef struct {
    int surface,active[3];
    double window[4],bounds3d[4];
    uint32_t colour[3];
    char label[3][80];
    double yaw,pitch,zoom;
} pico_plot_request;
/* Interactive native view; 1 normal close, 0/-1 failure, -2 sampling interruption. */
int pico_native_plot(pico_plot_request *,pico_plot_sample,void *);
#ifdef __cplusplus
}
#endif
