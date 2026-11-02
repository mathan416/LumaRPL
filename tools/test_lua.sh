#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/lua-tests
# Match clang++'s platform-default C++ runtime when linking C and C++ objects.
case $(uname -s) in
  Darwin) lua_cxx_runtime=-lc++ ;;
  *) lua_cxx_runtime=-lstdc++ ;;
esac
lua_test_sources=$(rg --files dependencies/lua-5.4.9/src | rg '\.c$' | rg -v '/(lua|luac)\.c$')
clang++ -std=c++17 -O1 -g -fsanitize=address,undefined -Ipico -c tests/plot_engine_adapter.cc -o build/lua-tests/plot.o
clang -DLUAI_MAXCCALLS=40 -std=c11 -O1 -g -fsanitize=address,undefined -Idependencies/lua-5.4.9/src -Ipico $lua_test_sources pico/lua_runner.c tests/lua_test.c tests/lua_device_stub.c build/lua-tests/plot.o "$lua_cxx_runtime" -lm -o build/lua-tests/test
UBSAN_OPTIONS=halt_on_error=1 build/lua-tests/test
