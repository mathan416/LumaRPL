// Stock Lua 5.4 string library with an interruption point. GPL-3.0 wrapper;
// lstrlib.c itself is compiled unmodified from the authenticated Lua archive.
//
// Pattern matching (find/match/gmatch/gsub) can backtrack for minutes without
// executing a Lua instruction, so the count hook never sees Esc. Every
// l_unlikely() test in this library is routed through pico_lua_strlib_poll(),
// which returns its argument unchanged and, every few thousand steps, raises
// "Interrupted" on the Lua thread that entered the string function (see
// lua_runner.c). Builds must compile this file instead of lstrlib.c.
int pico_lua_strlib_poll(int condition);
#define luai_likely(x) (__builtin_expect(((x) != 0), 1))
#define luai_unlikely(x) (__builtin_expect(pico_lua_strlib_poll((x) != 0), 0))
#include "lstrlib.c"
