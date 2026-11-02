; Original PicoCalc demo. Move with W A S D; Esc returns to calculator.
CLS
LD V0, 28
LD V1, 12
loop:
CLS
LD I, sprite
DRW V0, V1, 5
LD V2, 5
SKNP V2
ADD V1, 255
LD V2, 7
SKNP V2
ADD V0, 255
LD V2, 8
SKNP V2
ADD V1, 1
LD V2, 9
SKNP V2
ADD V0, 1
LD V2, 2
LD DT, V2
wait:
LD V2, DT
SE V2, 0
JP wait
JP loop
sprite:
DB 0xF0, 0x90, 0xF0, 0x90, 0x90
