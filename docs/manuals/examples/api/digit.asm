; SUPER-CHIP modern. Show hexadecimal A at (12,8).
CLS
LD V0, 10
LD V1, 12
LD V2, 8
CALL draw_digit
LD V0, K
EXIT

draw_digit:
  LD F, V0
  DRW V1, V2, 5
  RET
