#pragma once
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef int (*pico_network_cancel_fn)(void *);
typedef void (*pico_network_progress_fn)(void *,const char *);
typedef struct pico_network_result {int code,tls_error,link_status,ntp_status;int64_t utc_time;uint32_t join_ms;unsigned join_attempts;uint32_t radio_event,radio_status,radio_reason;int radio_event_valid;size_t tls_peak,session_bytes;char message[96];} pico_network_result;
typedef struct pico_network_status {int radio_on,connected;} pico_network_status;
enum { PICO_NET_JOINING, PICO_NET_TIME, PICO_NET_HTTPS, PICO_NET_SUCCESS, PICO_NET_FAILED, PICO_NET_CANCELLED };
typedef struct pico_network_attempt {
    uint32_t started_ms,elapsed_ms,event,status,reason;
    int16_t link_status;
    uint8_t manual,stage,attempt,event_valid;
} pico_network_attempt;
typedef struct pico_network_diagnostics {
    pico_network_attempt attempts[3];
    uint32_t next_retry_ms,driver_heap_bytes;
    uint8_t count,clock_pending;
} pico_network_diagnostics;
// Read-only snapshot; never initiates a connection or includes credentials.
void pico_network_get_diagnostics(pico_network_diagnostics *);
// Called by the SDK event observer; contains no credentials or packet payload.
void pico_network_wifi_event(unsigned interface,unsigned event,unsigned status,unsigned reason);
// Observes current state without initializing or connecting the radio.
pico_network_status pico_network_get_status(void);
// Boot-only silent time sync. Start at boot after loading valid credentials;
// poll from the cooperative input loop. Returns validated UTC once, else zero.
void pico_network_time_start(const char *ssid,const char *password);
int64_t pico_network_time_poll(void);
void pico_network_time_cancel(void);
// Notify only after a successful credential save; retire a different boot snapshot.
void pico_network_credentials_saved(const char *ssid,const char *password);
// Foreground connection takes sole radio ownership. Failed requests may resume
// remaining clock retries; cancellation and successful synchronization end them.
// utc=0 obtains network time over SNTP before HTTPS; nonzero is a manual UTC override.
// SNTP is not authenticated. TLS chain/hostname/date validation remains required.
int pico_network_rates(const char *ssid,const char *password,int64_t utc,
                       char *body,size_t capacity,pico_network_result *,
                       pico_network_cancel_fn,pico_network_progress_fn,void *context);
int pico_network_rates_dated(const char *ssid,const char *password,int64_t utc,const char *rate_date,
                       char *body,size_t capacity,pico_network_result *,
                       pico_network_cancel_fn,pico_network_progress_fn,void *context);
#ifdef __cplusplus
}
#endif
