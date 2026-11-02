#pragma once
#include <stdint.h>
struct pico_lua_input {
    uint32_t down[8]={};int queue[16]={};unsigned head=0,tail=0;bool cancelled=false;
    static int normalized(int c){if(c>='A'&&c<='Z')c+=32;if(c==10)c=13;if(c==127)c=8;return c;}
    void feed(int c,int state)
    {
        if(c<0||c>255)return;
        if((c==27||c==0xb1||c==0x85)&&state==1){cancelled=true;return;}
        c=normalized(c);uint32_t bit=1u<<(c%32);
        if(state==3)down[c/32]&=~bit;
        else if(state==1||state==2)down[c/32]|=bit;
        if(state==1){unsigned next=(head+1)%16;if(next==tail)tail=(tail+1)%16;queue[head]=c;head=next;}
    }
    int poll(){if(cancelled)return -2;if(head==tail)return -1;int c=queue[tail];tail=(tail+1)%16;return c;}
    int held(int c)const{if(cancelled)return -1;c=normalized(c);return c>=0&&c<256&&bool(down[c/32]&(1u<<(c%32)));}
};
