; SUPER-CHIP modern. Draw 237 at (8,10), then wait for a key.
CLS
LD V0, 237
LD V1, 8
LD V2, 10
CALL draw_u8_decimal
LD V0, K
EXIT

draw_digit:
  LD F, V0
  DRW V1, V2, 5
  RET

draw_u8_decimal:
  LD V8, V1
  LD V9, V2
  LD I, decimal_digits
  LD B, V0
  LOAD V2
  LD V3, V0
  LD V4, V1
  LD V5, V2
  LD V1, V8
  LD V2, V9
  LD V0, V3
  CALL draw_digit
  ADD V1, 5
  LD V0, V4
  CALL draw_digit
  ADD V1, 5
  LD V0, V5
  CALL draw_digit
  RET

decimal_digits:
  DB 0, 0, 0
