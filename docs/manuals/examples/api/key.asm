; SUPER-CHIP modern. Draw the next keypad value at (8,8).
CLS
CALL wait_key
LD V1, 8
LD V2, 8
CALL draw_digit
LD V0, 30
LD DT, V0
wait_visible:
  LD V0, DT
  SNE V0, 0
  EXIT
  JP wait_visible

wait_key:
  LD V0, K
  RET

draw_digit:
  LD F, V0
  DRW V1, V2, 5
  RET
