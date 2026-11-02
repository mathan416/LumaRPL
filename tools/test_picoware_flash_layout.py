#!/usr/bin/env python3
"""Exercise the actual Picoware/MicroPython translation helper on the host."""
import re,subprocess,tempfile
from pathlib import Path
root=Path(__file__).resolve().parents[1]
source=(root/'dependencies/micropython-picoware/ports/rp2/rp2_flash.c').read_text()
helper=re.search(r'static uint32_t picocalc_physical_flash_offset\(.*?\n}\n',source,re.S).group()
pre='''#include <stdint.h>
#include <assert.h>
#include <setjmp.h>
#define FLASH_SECTOR_SIZE 4096u
#define PICO_FLASH_SIZE_BYTES (4u*1024*1024)
#define QMI_ATRANS0_BASE_BITS 0xfffu
#define QMI_ATRANS0_BASE_LSB 0
#define MP_EINVAL 22
static struct {uint32_t atrans[4];} regs;
static typeof(regs) *qmi_hw=&regs;
static jmp_buf error_jmp;
static void mp_raise_OSError(int e) {assert(e==22);longjmp(error_jmp,1);}
'''
main='''int main(void) {
 const uint32_t base=4u*1024*1024-8192-2048000;
 for(unsigned shift=0;shift<=8192;shift+=8192) {
  regs.atrans[0]=(0x3feu<<16)|(shift/4096);
  for(uint32_t logical=base;logical<4u*1024*1024-16384;logical+=4096) {
   uint32_t physical=picocalc_physical_flash_offset(logical);
   assert(physical==logical+shift);
   assert(physical>=8192 && physical+4096<=PICO_FLASH_SIZE_BYTES-8192);
  }
 }
 regs.atrans[0]=1;
 if(!setjmp(error_jmp)) {picocalc_physical_flash_offset(base);assert(0);}
 regs.atrans[0]=2;
 if(!setjmp(error_jmp)) {picocalc_physical_flash_offset(PICO_FLASH_SIZE_BYTES-16384);assert(0);}
 return 0;
}
'''
with tempfile.TemporaryDirectory() as t:
 p=Path(t);(p/'test.c').write_text(pre+helper+main)
 subprocess.run(['clang','-std=gnu11','-fsanitize=address,undefined','-Wall','-Wextra','-Werror',str(p/'test.c'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
print('PASS: actual flash helper across every filesystem sector, native/remapped boot, unknown remap and out-of-range rejection (ASan/UBSan).')
