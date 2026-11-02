#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/arm-allocation
armcc=${ARM_CC:-arm-none-eabi-gcc}
armcxx=${ARM_CXX:-arm-none-eabi-g++}
"$armcc" -mcpu=cortex-m3 -mthumb -Os -c pico/heap.c -o build/arm-allocation/heap.o
for source in dependencies/lua-5.4.9/src/*.c pico/lua_runner.c tests/lua_device_stub.c; do
 case "$source" in */lua.c|*/luac.c) continue;; esac
 name=$(basename "$source" .c)
 "$armcc" -std=gnu11 -mcpu=cortex-m3 -mthumb -Os -ffunction-sections -fdata-sections -DLUAI_MAXCCALLS=40 -Idependencies/lua-5.4.9/src -Ipico -c "$source" -o "build/arm-allocation/$name.o"
done
alignment=$(python3 - <<'PYALIGN'
import subprocess
symbols=subprocess.check_output(['arm-none-eabi-nm','build/pico/db48x-picocalc-pico2w.elf'],text=True)
for line in symbols.splitlines():
 fields=line.split()
 if len(fields)==3 and fields[2]=='end':print(int(fields[0],16)%4096);break
else:raise SystemExit('Missing firmware heap start')
PYALIGN
)
for offset in 0 "$alignment" 4092; do
 "$armcxx" -std=gnu++17 -mcpu=cortex-m3 -mthumb -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -Idb48x/src -Ipico -nostartfiles --specs=nosys.specs -Wl,--gc-sections -Wl,--defsym,TEST_HEAP_OFFSET="$offset" -T tests/arm/memory.ld tests/arm/start.S tests/arm/allocation_test.cc pico/allocation.cc build/arm-allocation/*.o -lm -o build/arm-allocation/test.elf
 echo "Heap offset: $offset (modulo 4096)"
 python3 - <<'PYTEST'
import subprocess
subprocess.run(['qemu-system-arm','-M','mps2-an385','-kernel','build/arm-allocation/test.elf','-semihosting-config','enable=on,target=native','-nographic'],timeout=60,check=True)
PYTEST
done
