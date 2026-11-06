# LumaRPL API & Assembly examples

For firmware 0.10.0-rc.3 and the companion API & Assembly Reference.
These are complete programs. No native Arm development or firmware calls are required.

## Lua

Copy a `.lua` file into `/db48x/lua/` on the SD card. Open it through
Ctrl+P > Lua scripts, then use F2 Save & run (editor) or F2 Run (browser).

- `canvas.lua`: measured compact text and a frame; press a key to finish.
- `plot.lua`: square/log curves in the native viewer; return normally to continue the script.
- `bridge.lua`: exact calculator text and a deliberately rejected quoted expression.
- `errors.lua`: a caught invalid font selection, unchanged compact profile, and canvas dimensions.

## CHIP-8 / SUPER-CHIP

Copy an `.asm` file into `/db48x/chip8/`. In the CHIP-8 browser choose
F4 Actions > Compatibility profile > SUPER-CHIP modern before running.
The assembler produces a separate `.ch8` ROM; retain the `.asm` for editing.

- `digit.asm`: draws hexadecimal A; press and release a mapped key to finish.
- `decimal.asm`: draws 237; try changing the initial V0 to 7 to display 007.
- `timing.asm`: uses beep_ticks and wait_ticks for approximately half a second.
- `key.asm`: waits for a new keypad press/release, then briefly draws its hexadecimal value.
- `saturating.asm`: debugger example; after the call, V0=255 and VF=1. It draws nothing.

Physical X maps to CHIP-8 key 0. Esc interrupts the surrounding application.

The callable labels in these files are included source routines, not resident
firmware APIs. To reuse one, copy its body and dependencies into your source.
The assembler has no include directive. Read the companion's register, memory,
profile and call-depth contracts before adapting them.

## Verification and license

Host tests use the production Lua runner, calculator bridge, native sampler,
assembler and VM. Host input/timer simulation does not verify physical LCD,
sound or timing. Examples and original explanatory text follow the repository's
GPL-3.0 terms; full license text is included in the downloadable example ZIP.
Save your own code before replacing an example.
