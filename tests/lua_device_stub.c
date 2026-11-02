#include "lua_device.h"
#include "lua_runner.h"
#include <string.h>
int test_draws,test_last_op,test_key='a',test_held=1,test_beeps;
uint32_t test_colour;static uint32_t now;
void pico_lua_device_begin(void){test_draws=0;now=0;}
void pico_lua_device_reset(void){}
void pico_lua_device_end(void){}
void pico_lua_draw(int op,const int *a,uint32_t colour,const char *text,size_t length)
{(void)a;(void)text;(void)length;++test_draws;test_last_op=op;test_colour=colour;}
int pico_lua_key_poll(void){int key=test_key;test_key=-1;return key;}
int pico_lua_key_down(int key){return key=='a'&&test_held;}
uint32_t pico_lua_millis(void){return now;}
int pico_lua_sleep(unsigned ms){do{++now;if(pico_lua_cancel())return 1;}while(ms&&--ms);return 0;}
int pico_lua_beep(unsigned frequency,unsigned ms){(void)frequency;++test_beeps;return pico_lua_sleep(ms);}

void pico_lua_text_measure(int profile,const char *text,size_t length,int *width,int *height)
{int col=0,lines=length?1:0;*width=0;for(size_t i=0;i<length;++i){if(text[i]=='\n'){if(col>*width)*width=col;col=0;++lines;}else if(text[i]!='\r')col+=profile==1?10:6;}if(col>*width)*width=col;*height=lines*(profile==2?18:16);}

int pico_lua_sprite(const char *name,int x,int y){(void)x;(void)y;return !strcmp(name,"DEMO.sprite");}
