.syntax unified
.thumb
.section .vectors,"a"
.word 0x20400000
.word reset+1
.text
.global reset
.thumb_func
reset:
ldr r0, =_sidata
ldr r1, =_sdata
ldr r2, =_edata
1: cmp r1,r2
bcs 2f
ldr r3,[r0],#4
str r3,[r1],#4
b 1b
2: ldr r1,=_sbss
ldr r2,=_ebss
movs r3,#0
3: cmp r1,r2
bcs 4f
str r3,[r1],#4
b 3b
4: bl main
bl exit
b .
