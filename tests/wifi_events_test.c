#include "cyw43_ll.h"
#include "network.h"
#include <assert.h>
#include <stdio.h>
static unsigned observed,forwarded;
static cyw43_async_event_t packet={.event_type=7,.status=1,.reason=17,.interface=0};
void pico_network_wifi_event(unsigned interface,unsigned event,unsigned status,unsigned reason)
{
    assert(!forwarded&&interface==0&&event==7&&status==1&&reason==17);++observed;
}
void __real_cyw43_cb_process_async_event(void *context,const cyw43_async_event_t *event)
{
    assert(context==&packet&&event==&packet&&observed==1);++forwarded;
}
void __wrap_cyw43_cb_process_async_event(void *,const cyw43_async_event_t *);
int main(void)
{
    __wrap_cyw43_cb_process_async_event(&packet,&packet);
    assert(observed==1&&forwarded==1);
    puts("PASS: actual firmware observer forwards unchanged SDK event and context exactly once");
}
