; SUPER-CHIP modern. Inspect V0=255 in the debugger after the call.
LD V0, 250
LD V1, 20
CALL add_saturating
EXIT

add_saturating:
  ADD V0, V1
  SE VF, 1
  RET
  LD V0, 255
  RET
