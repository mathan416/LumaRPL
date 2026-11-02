ENTRY(reset)
MEMORY { FLASH(rx): ORIGIN = 0, LENGTH = 4M
RAM(rwx): ORIGIN = 0x20000000, LENGTH = 4M }
SECTIONS { .text : { KEEP(*(.vectors)) *(.text*) *(.rodata*) } >FLASH
.ARM.exidx : { *(.ARM.exidx*) } >FLASH
_sidata = LOADADDR(.data);
.data : { _sdata = .; *(.data*) _edata = .; } >RAM AT>FLASH
.bss : { _sbss = .; *(.bss*) *(COMMON) _ebss = .; } >RAM
end = ALIGN(4096) + (DEFINED(TEST_HEAP_OFFSET) ? TEST_HEAP_OFFSET : 0); _end = end; }
