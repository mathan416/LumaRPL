; SUPER-CHIP tone/timer demo. Esc also exits.
LD V0, 30
LD ST, V0
LD DT, V0
wait:
LD V0, DT
SE V0, 0
JP wait
EXIT
