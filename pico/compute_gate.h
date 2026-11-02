#pragma once
#include <stdint.h>
#include <stddef.h>
// Core-0 cooperative admission, not an allocator or a cross-core mutex.
struct pico_compute_gate {
 bool active=false;uint32_t started=0;
 bool begin(uint32_t now,bool foreground_network,size_t allocated){if(active||foreground_network||allocated>72u*1024u)return false;active=true;started=now;return true;}
 uint32_t end(uint32_t now){if(!active)return 0;active=false;return now-started;}
};
