; SUPER-CHIP modern. About half a second of sound, then exit.
LD V0, 30
CALL beep_ticks
EXIT

beep_ticks:
  LD ST, V0
  CALL wait_ticks
  RET

wait_ticks:
  LD DT, V0
wait_ticks_loop:
  LD V1, DT
  SNE V1, 0
  RET
  JP wait_ticks_loop
