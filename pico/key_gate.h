#pragma once
// Shared raw-key transition gate: block the activating key until its release.
typedef struct {int pressed,blocked;} pico_key_gate;
static inline int pico_key_gate_event(pico_key_gate *g,int code,int state)
{
    if(state==3){if(g->pressed==code)g->pressed=-1;if(g->blocked==code){g->blocked=-1;return 0;}}
    else if(state==1)g->pressed=code;
    return g->blocked!=code;
}
static inline void pico_key_gate_transition(pico_key_gate *g){g->blocked=g->pressed;}
