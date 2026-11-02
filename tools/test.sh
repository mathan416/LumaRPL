#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build
cxx="${CXX:-clang++}"
cc="${CC:-clang}"
"$cxx" -std=c++17 -D__packed= -g -O1 -fsanitize=address,undefined \
  -Ipico -Ipico/fatfs -Idb48x/src tests/port_test.cc -o build/port-test
UBSAN_OPTIONS=halt_on_error=1 build/port-test
"$cc" -std=c11 -g -O1 -fsanitize=address,undefined \
  -Ipico -Ipico/fatfs -c pico/storage.c -o build/storage-test.o
"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined \
  -Ipico -Ipico/fatfs tests/storage_test.cc build/storage-test.o -o build/storage-test
UBSAN_OPTIONS=halt_on_error=1 build/storage-test
"$cc" -std=c11 -g -O1 -fsanitize=address,undefined \
  tests/heap_test.c pico/heap.c -o build/heap-test
UBSAN_OPTIONS=halt_on_error=1 build/heap-test
"$cc" -std=c11 -g -O1 -fsanitize=address,undefined \
  -Ipico -Ipico/fatfs tests/fatfs_test.c pico/storage.c \
  pico/fatfs/ff.c pico/fatfs/ffunicode.c -o build/fatfs-test
UBSAN_OPTIONS=halt_on_error=1 build/fatfs-test
"$cxx" -std=c++17 -D__packed= -DCONFIG_COLOR=1 -g -O1 -fsanitize=address,undefined \
  -Ipico -Ipico/fatfs -Idb48x/src tests/colour_test.cc -o build/colour-test
UBSAN_OPTIONS=halt_on_error=1 build/colour-test
"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined \
  -Ipico tests/editor_test.cc -o build/editor-test
UBSAN_OPTIONS=halt_on_error=1 build/editor-test
"$cxx" -std=c++17 -D__packed= -DCONFIG_COLOR=1 -g -O1 -fsanitize=address,undefined \
  -Ipico -Ipico/fatfs -Idb48x/src tests/wireframe_test.cc -o build/wireframe-test
UBSAN_OPTIONS=halt_on_error=1 build/wireframe-test

"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined \
  -Ipico tests/chip8_test.cc -o build/chip8-test
UBSAN_OPTIONS=halt_on_error=1 build/chip8-test

"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined \
  -Ipico tests/port6_tools_test.cc -o build/port6-tools-test
UBSAN_OPTIONS=halt_on_error=1 build/port6-tools-test

"$cxx" -std=c++17 -D__packed= -DCONFIG_COLOR=1 -g -O1 -fsanitize=address,undefined \
  -Ipico -Ipico/fatfs -Idb48x/src tests/lua_canvas_test.cc -o build/lua-canvas-test
UBSAN_OPTIONS=halt_on_error=1 build/lua-canvas-test

"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined \
  -Ipico tests/plot_engine_test.cc -o build/plot-engine-test
UBSAN_OPTIONS=halt_on_error=1 build/plot-engine-test

"$cc" -std=c11 -g -O1 -fsanitize=address,undefined \
  -Ipico tests/battery_test.c -o build/battery-test
UBSAN_OPTIONS=halt_on_error=1 build/battery-test

"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined \
  -Ipico tests/plot_analysis_test.cc -o build/plot-analysis-test
UBSAN_OPTIONS=halt_on_error=1 build/plot-analysis-test

"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined \
  -Ipico tests/workbench_test.cc -o build/workbench-test
UBSAN_OPTIONS=halt_on_error=1 build/workbench-test

"$cc" -std=c11 -g -O1 -fsanitize=address,undefined -Ipico -Ipico/fatfs \
  -c pico/fatfs/ff.c -o build/workbench-ff.o
"$cc" -std=c11 -g -O1 -fsanitize=address,undefined -Ipico -Ipico/fatfs \
  -c pico/fatfs/ffunicode.c -o build/workbench-ffunicode.o
"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico -Ipico/fatfs \
  tests/workbench_file_test.cc build/storage-test.o build/workbench-ff.o \
  build/workbench-ffunicode.o -o build/workbench-file-test
UBSAN_OPTIONS=halt_on_error=1 build/workbench-file-test

"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico \
  tests/data_table_test.cc -o build/data-table-test
UBSAN_OPTIONS=halt_on_error=1 build/data-table-test
"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico -Ipico/fatfs \
  tests/data_file_test.cc build/storage-test.o build/workbench-ff.o \
  build/workbench-ffunicode.o -o build/data-file-test
UBSAN_OPTIONS=halt_on_error=1 build/data-file-test

"$cc" -std=c11 -g -O1 -fsanitize=address,undefined -Ipico \
  tests/sd_read_test.c -o build/sd-read-test
UBSAN_OPTIONS=halt_on_error=1 build/sd-read-test
"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico -Ipico/fatfs \
  tests/sd_workbench_file_test.cc build/storage-test.o build/workbench-ff.o \
  build/workbench-ffunicode.o -o build/sd-workbench-file-test
UBSAN_OPTIONS=halt_on_error=1 build/sd-workbench-file-test

"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico \
  tests/form_document_test.cc -o build/form-document-test
UBSAN_OPTIONS=halt_on_error=1 build/form-document-test
"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico -Ipico/fatfs \
  tests/form_file_test.cc build/storage-test.o build/workbench-ff.o \
  build/workbench-ffunicode.o -o build/form-file-test
UBSAN_OPTIONS=halt_on_error=1 build/form-file-test

"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico \
  tests/notebook_test.cc -o build/notebook-test
UBSAN_OPTIONS=halt_on_error=1 build/notebook-test
"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico -Ipico/fatfs \
  tests/notebook_file_test.cc build/storage-test.o build/workbench-ff.o \
  build/workbench-ffunicode.o -o build/notebook-file-test
UBSAN_OPTIONS=halt_on_error=1 build/notebook-file-test

"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico \
  tests/path_test.cc -o build/path-test
UBSAN_OPTIONS=halt_on_error=1 build/path-test
"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico -Ipico/fatfs \
  tests/path_file_test.cc build/storage-test.o build/workbench-ff.o \
  build/workbench-ffunicode.o -o build/path-file-test
UBSAN_OPTIONS=halt_on_error=1 build/path-file-test

"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico tests/ode_test.cc -o build/ode-test
UBSAN_OPTIONS=halt_on_error=1 build/ode-test
"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico -Ipico/fatfs \
  tests/ode_file_test.cc build/storage-test.o build/workbench-ff.o \
  build/workbench-ffunicode.o -o build/ode-file-test
UBSAN_OPTIONS=halt_on_error=1 build/ode-file-test

"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Idb48x/src tests/scoped_array_test.cc -o build/scoped-array-test
UBSAN_OPTIONS=halt_on_error=1 build/scoped-array-test
"$cc" -std=c11 -g -O1 -fsanitize=address,undefined -Itests/sd_stub -Ipico -Ipico/fatfs tests/sd_deadline_test.c -o build/sd-deadline-test
UBSAN_OPTIONS=halt_on_error=1 build/sd-deadline-test
"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico -Ipico/fatfs \
 tests/recovery_transaction_test.cc build/storage-test.o build/workbench-ff.o build/workbench-ffunicode.o -o build/recovery-transaction-test
UBSAN_OPTIONS=halt_on_error=1 build/recovery-transaction-test

"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico tests/currency_test.cc -o build/currency-test
UBSAN_OPTIONS=halt_on_error=1 build/currency-test
python3 -m unittest discover -s tests -p 'test_currency.py'
"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico -Ipico/fatfs tests/currency_file_test.cc build/storage-test.o build/workbench-ff.o build/workbench-ffunicode.o -o build/currency-file-test
UBSAN_OPTIONS=halt_on_error=1 build/currency-file-test
"$cc" -std=c11 -g -O1 -fsanitize=address,undefined -Ipico -c pico/tls_budget.c -o build/tls-budget-test.o
"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico tests/network_test.cc build/tls-budget-test.o -o build/network-test
UBSAN_OPTIONS=halt_on_error=1 build/network-test
"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico -Ipico/fatfs tests/network_settings_test.cc build/storage-test.o build/workbench-ff.o build/workbench-ffunicode.o -o build/network-settings-test
UBSAN_OPTIONS=halt_on_error=1 build/network-settings-test
"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico tests/ntp_test.cc -o build/ntp-test
UBSAN_OPTIONS=halt_on_error=1 build/ntp-test
"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico tests/timezone_test.cc -o build/timezone-test
UBSAN_OPTIONS=halt_on_error=1 build/timezone-test
"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico -Ipico/fatfs tests/timezone_file_test.cc build/storage-test.o build/workbench-ff.o build/workbench-ffunicode.o -o build/timezone-file-test
UBSAN_OPTIONS=halt_on_error=1 build/timezone-file-test
"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico tests/wall_clock_test.cc -o build/wall-clock-test
UBSAN_OPTIONS=halt_on_error=1 build/wall-clock-test

"$cxx" -std=c++17 -g -O1 -fsanitize=address,undefined -Ipico -Ipico/fatfs tests/currency_preferences_test.cc build/storage-test.o build/workbench-ff.o build/workbench-ffunicode.o -o build/currency-preferences-test
UBSAN_OPTIONS=halt_on_error=1 build/currency-preferences-test
