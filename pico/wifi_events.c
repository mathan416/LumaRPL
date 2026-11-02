// Observe firmware join diagnostics without changing SDK event handling.
// GPL-3.0; the pinned CYW43 driver retains its own license.
#include "cyw43_ll.h"
#include "network.h"
void __real_cyw43_cb_process_async_event(void *,const cyw43_async_event_t *);
void __wrap_cyw43_cb_process_async_event(void *context,const cyw43_async_event_t *event)
{
    if(event)pico_network_wifi_event(event->interface,event->event_type,event->status,event->reason);
    __real_cyw43_cb_process_async_event(context,event);
}
