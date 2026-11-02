#pragma once
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
// All platform callbacks return before Lua can allocate or raise an error.
enum { PICO_LUA_CLEAR,PICO_LUA_PIXEL,PICO_LUA_LINE,PICO_LUA_RECT,PICO_LUA_CIRCLE,PICO_LUA_TEXT,PICO_LUA_SHOW };
void pico_lua_device_begin(void);
void pico_lua_device_reset(void);
void pico_lua_device_end(void);
void pico_lua_draw(int operation,const int *args,uint32_t colour,const char *text,size_t length);
int pico_lua_sprite(const char *name,int x,int y); // 1 success, 0 invalid/unavailable
void pico_lua_text_measure(int profile,const char *,size_t,int *width,int *height);
int pico_lua_key_poll(void); // -1 none, -2 Esc interruption, otherwise key code
int pico_lua_key_down(int code);
int pico_lua_sleep(unsigned milliseconds); // nonzero if interrupted
uint32_t pico_lua_millis(void);
int pico_lua_beep(unsigned frequency,unsigned milliseconds);
#ifdef __cplusplus
}
#endif
