#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/arm-tls-v367
armcc=${ARM_CC:-arm-none-eabi-gcc}
armcxx=${ARM_CXX:-arm-none-eabi-g++}
for source in dependencies/mbedtls-3.6.7/library/*.c pico/tls_budget.c pico/heap.c;do
 name=$(basename "$source" .c)
 object="build/arm-tls-v367/$name.o"
 if [ ! -e "$object" ] || [ "$source" -nt "$object" ] || [ pico/network/picocalc_mbedtls_config.h -nt "$object" ];then
  "$armcc" -std=gnu11 -mcpu=cortex-m3 -mthumb -Os -ffunction-sections -fdata-sections -DMBEDTLS_CONFIG_FILE='"picocalc_mbedtls_config.h"' -Ipico/network -Ipico -Idependencies/mbedtls-3.6.7/include -Idependencies/mbedtls-3.6.7/library -c "$source" -o "$object"
 fi
done
# Include the current firmware's heap alignment alongside two boundary cases.
alignment=$(python3 - <<'PYALIGN'
import subprocess
symbols=subprocess.check_output(['arm-none-eabi-nm','build/pico/db48x-picocalc-pico2w.elf'],text=True)
for line in symbols.splitlines():
 fields=line.split()
 if len(fields)==3 and fields[2]=='end':print(int(fields[0],16)%4096);break
else:raise SystemExit('Missing firmware heap start')
PYALIGN
)
for offset in 0 "$alignment" 4092;do
"$armcxx" -std=gnu++17 -mcpu=cortex-m3 -mthumb -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -DMBEDTLS_CONFIG_FILE='"picocalc_mbedtls_config.h"' -Ipico/network -Ipico -Idependencies/mbedtls-3.6.7/include -nostartfiles --specs=nosys.specs -Wl,--gc-sections -Wl,--defsym,TEST_HEAP_OFFSET="$offset" -T tests/arm/memory.ld tests/arm/start.S tests/arm/tls_budget_test.cc build/arm-tls-v367/*.o -o build/arm-tls-v367/test.elf
echo "Heap offset: $offset"
python3 - <<'PY'
import subprocess
subprocess.run(['qemu-system-arm','-M','mps2-an385','-kernel','build/arm-tls-v367/test.elf','-semihosting-config','enable=on,target=native','-nographic'],check=True,timeout=180)
PY
done
