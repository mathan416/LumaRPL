#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct { char output[2048];size_t length,peak;int cancelled,graphics;unsigned error_line;char error[192]; } pico_lua_result;
typedef struct pico_lua_session pico_lua_session;
pico_lua_session *pico_lua_open(pico_lua_result *result);
// interactive: expressions print their results; incomplete chunks return 2.
int pico_lua_submit(pico_lua_session *,const char *,size_t,int interactive,pico_lua_result *);
void pico_lua_close(pico_lua_session *);
// Reader sets size=SIZE_MAX on I/O error. Loading is protected by Lua.
typedef const char *(*pico_lua_reader)(void *,size_t *);
int pico_lua_run_stream(pico_lua_reader,void *,pico_lua_result *);
int pico_lua_run(const char *source,size_t length,pico_lua_result *result);
int pico_lua_cancel(void);
int pico_lua_eval(const char *expression,char *result,size_t capacity);
#ifdef __cplusplus
}
#endif
