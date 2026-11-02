cmake_minimum_required(VERSION 3.20)
option(PICOCALC_COLOR "256-colour RGB332 calculator (required)" ON)
if(NOT PICOCALC_COLOR)
    message(FATAL_ERROR "Only the colour PicoCalc calculator is supported")
endif()
set(PICO_BOARD pico2_w CACHE STRING "Target board")
set(PICO_PLATFORM rp2350-arm-s CACHE STRING "Arm target")
set(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/../dependencies/pico-sdk" CACHE PATH "Pico SDK")
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(db48x_picocalc C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_MBEDTLS_PATH "${CMAKE_CURRENT_LIST_DIR}/../dependencies/mbedtls-3.6.7")
set(PICO_MBEDTLS_CONFIG_FILE "picocalc_mbedtls_config.h")
pico_sdk_init()
if(NOT PICO_BOARD STREQUAL "pico2_w" OR NOT PICO_PLATFORM STREQUAL "rp2350-arm-s")
    message(FATAL_ERROR "This port requires PICO_BOARD=pico2_w and PICO_PLATFORM=rp2350-arm-s")
endif()
set(DB48X "${CMAKE_CURRENT_LIST_DIR}/../db48x")
set(LOADER "${CMAKE_CURRENT_LIST_DIR}/../dependencies/uf2loader")
file(GLOB CORE "${DB48X}/src/*.cc")
list(REMOVE_ITEM CORE "${DB48X}/src/tests.cc")
add_subdirectory(${LOADER}/common/lcdspi lcdspi)
target_include_directories(lcdspi INTERFACE ${LOADER}/common/i2ckbd)
set(LUA_ROOT "${CMAKE_CURRENT_LIST_DIR}/../dependencies/lua-5.4.9/src")
file(GLOB LUA_CORE "${LUA_ROOT}/*.c")
list(REMOVE_ITEM LUA_CORE "${LUA_ROOT}/lua.c" "${LUA_ROOT}/luac.c")
add_library(picocalc_lua STATIC ${LUA_CORE} lua_runner.c)
target_compile_definitions(picocalc_lua PRIVATE LUAI_MAXCCALLS=40)
target_include_directories(picocalc_lua PUBLIC ${LUA_ROOT} .)
target_compile_options(picocalc_lua PRIVATE -Os -fstack-usage -ffunction-sections -fdata-sections)
add_executable(db48x-picocalc-pico2w
    ${CORE} main.cc target.cc sysmenu.cc platform.cc devices.c storage.c heap.c allocation.cc network.cc wifi_events.c tls_budget.c psram.c psram_bus.c bulk_storage.cc workspace.cc memory_profile.cc dmcp_fonts.cc sdmmc.c desktop.cc HeaderFont.cc SourceFont.cc ReferenceFont.cc calculator_bridge.cc lua_devices.cc
    ${DB48X}/fonts/EditorFont.cc ${DB48X}/fonts/HelpFont.cc
    ${DB48X}/fonts/ReducedFont.cc ${DB48X}/fonts/StackFont.cc
    fatfs/ff.c fatfs/ffunicode.c fatfs/diskio.c
    )
pico_generate_pio_header(db48x-picocalc-pico2w ${CMAKE_CURRENT_LIST_DIR}/psram_spi.pio)
target_include_directories(db48x-picocalc-pico2w PRIVATE
    . fatfs ${DB48X}/src ${DB48X}/src/dm42 ${DB48X}/src/dmcp
    ${LOADER}/ui/lib/sdmmc)
target_compile_definitions(db48x-picocalc-pico2w PRIVATE
    PICOCALC=1 FIRMWARE=1 NDEBUG=1 CONFIG_FIXED_BASED_OBJECTS=1
    DB48X_VERSION="0.10.0-picocalc" __packed=
    HELPFILE_NAME="help/db48x.md" HELPINDEX_NAME="help/db48x.idx"
    PICO_STACK_SIZE=32768 PICO_HEAP_SIZE=98304 PICO_STDIO_USB=0 PICO_MALLOC_PANIC=0)
target_compile_options(db48x-picocalc-pico2w PRIVATE
    -Os -fstack-usage -ffunction-sections -fdata-sections -fno-strict-aliasing
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions;-fno-rtti>)
target_link_libraries(db48x-picocalc-pico2w PRIVATE pico_stdlib hardware_spi hardware_i2c hardware_pwm lcdspi picocalc_lua)
# Required network dependencies are pinned by the SDK revision and SOURCES.lock.
if(NOT TARGET pico_cyw43_arch_lwip_poll OR NOT TARGET pico_mbedtls)
    message(FATAL_ERROR "Initialize Pico SDK lib/cyw43-driver, lib/lwip and lib/mbedtls submodules")
endif()
target_include_directories(db48x-picocalc-pico2w PRIVATE network)
target_link_libraries(db48x-picocalc-pico2w PRIVATE pico_cyw43_arch_lwip_poll pico_mbedtls)
target_link_options(db48x-picocalc-pico2w PRIVATE -Wl,-u,pico_network_memory_profile -Wl,--print-memory-usage -Wl,--wrap=cyw43_cb_process_async_event)
pico_set_printf_implementation(db48x-picocalc-pico2w compiler)
pico_enable_stdio_usb(db48x-picocalc-pico2w 0)
pico_enable_stdio_uart(db48x-picocalc-pico2w 0)
target_compile_definitions(db48x-picocalc-pico2w PRIVATE CONFIG_COLOR=1 PICOCALC_RGB332=1 PICOCALC_ARENA_KIB=208)
pico_set_program_name(db48x-picocalc-pico2w "LumaRPL")
file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/../VERSION" LUMARPL_VERSION LIMIT_COUNT 1)
pico_set_program_version(db48x-picocalc-pico2w "${LUMARPL_VERSION}")
pico_add_extra_outputs(db48x-picocalc-pico2w)

pico_set_linker_script(db48x-picocalc-pico2w ${CMAKE_CURRENT_LIST_DIR}/memmap.ld)

