#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
cmake -S dependencies/picotool -B build/picotool -G Ninja \
  -DPICO_SDK_PATH="$PWD/dependencies/pico-sdk" -DPICOTOOL_NO_LIBUSB=1
cmake --build build/picotool -j 8
cmake --install build/picotool --prefix "$PWD/build/picotool-install"
cmake -S pico -B build/pico -G Ninja \
  -DPICO_BOARD=pico2_w -DPICO_PLATFORM=rp2350-arm-s \
  -Dpicotool_DIR="$PWD/build/picotool-install/lib/cmake/picotool" "$@"
cmake --build build/pico -j 8
mkdir -p reports
python3 tools/report_memory.py > reports/memory.json
python3 tools/validate_uf2.py build/pico/db48x-picocalc-pico2w.uf2 > reports/uf2-validation.json
cat reports/memory.json
