# 1 Using this reference

Keep this book beside the Programming Guide when you need an exact signature, a limit or the register contract for a subroutine. It describes the APIs shipped in LumaRPL **0.10.0-rc.2**. It does not introduce new firmware services.

There are three distinct interfaces:

| Interface | How you use it | What it can access |
|---|---|---|
| Lua API | Call a function such as screen.text(...) | The documented canvas, native plot viewer, keys, timing, sound and calculator-expression bridge |
| CHIP-8 instruction | Assemble LD, DRW, CALL and other supported mnemonics | The virtual machine's registers, memory, monochrome display, keys and timers |
| Included ROM routine | Put its assembly body in your source and CALL its label | Only what its constituent CHIP-8 instructions can do |

A CALL is not a firmware service request. There is no ROM-call address table for LumaRPL mathematics, Lua, files or the network. The assembly recipes in chapter 8 are source routines you include in your ROM. Unsupported system instructions report an error. The virtual machine's built-in digit fonts are reached with LD F or LD HF, not by calling their addresses.

## How signatures work

Square brackets in a printed API signature mean an optional argument. Do not type those brackets. Lua table literals use real braces, and strings use real quotes. For example, screen.text(x, y, text [, colour]) becomes screen.text(10,20,"Hello","cyan").

Unless a function's entry says otherwise, successful drawing, plot setup, sleeping and sound calls return no values. Argument validation failures raise Lua errors. A surrounding pcall can catch ordinary errors; it should not be used to ignore cancellation or keep retrying invalid input.

The **Lua API and example reference** in appendix A supplies runnable examples alongside the calls. Appendix B supplies instruction examples. The tutorials in the Programming Guide explain the language syntax and source editors.

## Open and run a source file

Press Ctrl+P, choose Lua scripts or CHIP-8 / assembly, then F3 New. F1 saves; F2 saves and runs; F4 opens Tools; F5 returns. Language reference and Open example are separate Tools entries. Unsaved parent source and undo history are preserved when opening a supported nested example.

Assembly examples in this book use **SUPER-CHIP modern**. Select that compatibility profile through the file browser's F4 Actions before running. Keep the .asm source: the generated .ch8 file contains bytes, not your labels and comments.

Save source and important calculator work before experimenting. A saved script is not a saved calculator session. Examples supplied with this book are under docs/manuals/examples/api/ in the source repository.

# 2 Lua runtime contract

## Available language and libraries

The embedded language is Lua 5.4.9. LumaRPL opens base functions and the table, string, math, utf8 and coroutine libraries. Functions such as pcall, pairs, ipairs, tonumber and tostring are available through the base library. The standard io, os, package and debug libraries are not opened. dofile and loadfile are removed.

Use the source editor to open and save programs. There is no general Lua filesystem API, require-based module loader, socket API or arbitrary operating-system interface. screen.sprite is a deliberately narrow file operation for validated calculator sprite documents.

The Lua math library uses radians. Calculator angle settings apply when using calc.eval, not when using math.sin or math.cos. For degrees in Lua, use math.rad before the trigonometric function.

## Memory, output and interruption

| Resource | Contract |
|---|---|
| Lua allocator | 48 KiB budget, including the Lua state, loaded libraries and script allocations |
| Printed output | At most 2,047 stored bytes per submission; further bytes are discarded |
| Canvas | Existing 320 by 320 application framebuffer; no private second framebuffer |
| Lua text call | At most 1,024 UTF-8 bytes in screen.text or screen.measure |
| Calculator bridge | One expression, at most 240 bytes; rendered result must fit 255 text bytes |
| Sleep / key timeout | Integer milliseconds, 0 through 600,000 |
| Native 2D plots | At most three registered curves |

These budgets are not an invitation to allocate a table of exactly 48 KiB: the runtime already consumes part of the budget. Build data incrementally and release references when they are no longer useful. A table with many small objects has overhead beyond its raw values.

Esc interrupts supported execution, waits and native plots. The instruction hook checks cancellation every 1,000 Lua instructions; drawing and device operations also cooperate. This is not a promise of a fixed latency for an arbitrary long C-library operation. Do not catch an interruption and immediately restart the same long operation.

## Script and console lifetime

A file run creates a Lua session. Its globals, font profile and callback references end when that run closes. The console keeps them across submissions until the console session is closed. They do not become calculator HOME variables and do not survive a restart.

A new session starts with the compact font. Plot definitions keep references to their functions until plot.clear or session closure. A callback can keep a large captured table alive even after the original variable is reassigned.

# 3 Lua canvas API

## Coordinates and colours

The canvas is **320 by 320 pixels**. The origin is at the upper-left; X increases to the right and Y increases downward. screen.width and screen.height are exported integer values, both 320. They are not functions. Treat them as constants; changing a Lua table field does not resize the display.

Drawing coordinates must be finite and in the accepted range. Ordinary drawing positions and line endpoints accept -4096 through 4096 and are rounded using floor(value+0.5). Rectangle dimensions and circle radius accept 0 through 640 using the same rounding rule. The renderer clips to the display; clipping does not make an out-of-range argument valid.

Colours accept an integer 0 through 0xFFFFFF, a string such as "#12ABEF", or a supported name. The display converts RGB requests to its **256-colour RGB332 palette**. A numeric colour is a 24-bit RGB value, not a direct palette index. For exact red use 0xFF0000; the named colours are readable display choices and need not equal primary RGB values.

| Name | Requested RGB |
|---|---|
| black / white | 0x000000 / 0xFFFFFF |
| red / green / blue | 0xFF4040 / 0x40E080 / 0x4080FF |
| cyan / magenta | 0x40E0FF / 0xE060E0 |
| yellow / orange / gray | 0xFFFF40 / 0xFFA040 / 0x808080 |

Names are case-sensitive. A hexadecimal string must contain # followed by exactly six hexadecimal digits. Invalid names or values raise an error; they do not silently select black.

## Drawing calls

| Signature | Behaviour and defaults |
|---|---|
| screen.clear([colour]) | Fill the canvas; default black |
| screen.pixel(x,y[,colour]) | Set one pixel; default white |
| screen.line(x1,y1,x2,y2[,colour]) | Draw between endpoints; default white |
| screen.rect(x,y,width,height[,colour[,filled]]) | Default white and filled; pass false for an outline |
| screen.circle(x,y,radius[,colour[,filled]]) | Centre and radius; default white and outline; pass true to fill |
| screen.text(x,y,text[,colour]) | Draw text at the top-left position using the selected profile; default white |
| screen.show() | Transfer the frame to the LCD; no return values |

The rectangle and circle defaults are different. To request an outline rectangle, pass its colour and then false. To request a filled circle, pass its colour and then true. Lua truthiness applies to filled: 0 is truthy in Lua, so use false for an outline. Passing nil explicitly for filled is false, even though an omitted rectangle filled argument defaults to true.

Draw a complete frame, then call screen.show once. It refreshes the full canvas. Repainting after every individual pixel or character makes animation slower. Calls update the shared drawing surface; a frame is not made durable by showing it. Use keyboard.wait when a finished script should keep its canvas visible until a key arrives.

## Text profiles and measurement

| Signature | Result |
|---|---|
| screen.font() | Current profile name |
| screen.font("compact") | Previous profile name; select proportional compact text |
| screen.font("mono") | Previous profile name; select compact fixed cells |
| screen.font("legacy") | Previous profile name; select the earlier header-font behaviour |
| screen.measure(text) | Width, height in pixels, measured with the active profile |

Compact uses proportional 13-pixel text with 16-pixel line spacing; mono uses fixed cells for columns. Legacy retains header-font behaviour. Selection stays within the Lua session without changing calculator settings.

Choose the font before measuring. Measurement follows the same newline layout as drawing. Re-measure after changing profiles; clipping does not word-wrap long text.

## Colour sprites

**screen.sprite(filename,x,y)** returns true on success, or nil and an error string if the sprite is invalid or unavailable. Unlike the other drawing coordinates, X and Y must be **integers** from -4096 through 4096. Bad coordinate types/ranges still raise Lua argument errors.

Use a filename such as "DEMO.sprite" in /db48x/sprites/. The lower-case extension is required. Names are at most 80 bytes, with no embedded zero or directory separators. A .sprite contains 16 by 16 RGB332 pixels, transparency and a checksum. The loader can recover a valid backup. This is not a general filesystem API.

Opaque pixels draw; transparent pixels leave the canvas unchanged. Drawing clips at screen edges. Call screen.show afterward. Loading reads SD; repeated loads in an animation loop can be expensive.

# 4 Calculator, input, timing and sound

## calc.eval(expression)

Returns **text**, rendered by the DB48X calculator engine. Give it one algebraic expression without surrounding quotes, at most 240 bytes. For example, calc.eval("1/3") returns an exact fraction representation; it is not a Lua number. An expression can use calculator variables and relevant calculator settings. It is not a general sequence of RPL commands or a program-execution API.

The bridge restores the calculator stack depth and its temporary decimal-format override, and clears its reported engine error before returning to Lua. Invalid input or a result too long for the bridge's output buffer raises a Lua error. There is no second nil,error return contract here: use pcall if the failure is expected.

Only use tonumber on returned text when you know it is a plain Lua numeric spelling. Units, fractions, grouped numbers, symbolic values and complex values need interpretation. Prefer Lua numbers for ordinary Lua arithmetic; use the calculator bridge when you need calculator semantics.

## Keyboard calls

| Signature | Result and behaviour |
|---|---|
| keyboard.poll() | A queued key name/character, or nil if none is available |
| keyboard.down(key) | Boolean held state for the named key or single-byte character |
| keyboard.wait([timeout_ms]) | A queued key name/character; nil on timeout |

Named keys are left, right, up, down, enter, space, backspace and f1 through f5. Ordinary printable characters are single-character strings. Names are lower-case and case-sensitive. Unknown names raise errors in keyboard.down. The physical Esc key is reserved for interruption; do not depend on receiving an "esc" event.

keyboard.wait with no argument, nil or zero waits without a timeout. A positive integer timeout may be at most 600,000 ms. Waiting cooperates with device polling and interruption. A queued event and a current held state answer different questions; do not substitute keyboard.poll for keyboard.down when implementing continuous movement.

## Timing calls

**system.sleep(milliseconds)** accepts an integer from 0 through 600,000 and returns no values. Zero still yields through the device polling path. Esc can interrupt the sleep.

**system.millis()** returns the unsigned 32-bit millisecond count since device boot, not the calendar time. It wraps after approximately 49.7 days. For elapsed times shorter than one wrap, use (system.millis()-start) % 4294967296. A sleep is cooperative timing, not a guarantee that a frame finishes at an exact deadline.

## Sound calls

**sound.beep(frequency_hz,duration_ms)** accepts an integer frequency of zero, or 20 through 16,000 Hz, and integer duration 0 through 10,000 ms. A zero frequency is silence. It waits for that duration, stops the tone, returns no values and can be interrupted. **sound.stop()** requests silence and returns no values. These APIs do not expose arbitrary audio samples or multi-voice synthesis.

# 5 Native plotting API

Use native plots when you want calculator-style graph interaction. screen drawing is a pixel canvas; plot.function2d and plot.surface register mathematical callbacks for the unified native sampler and viewer.

## Definition and ownership

| Signature | Behaviour |
|---|---|
| plot.clear() | Release registered callbacks and restore Lua plot defaults |
| plot.window(xmin,xmax,ymin,ymax) | Set the 2D view window |
| plot.function2d(callback[,options]) | Append one 2D function; at most three |
| plot.surface(callback[,options]) | Set or replace the surface callback and choose surface mode |
| plot.show() | Open the native viewer; return to the Lua program after leaving it |

All return no values on success. Invalid arguments, too many curves, missing callbacks, callback errors or an unavailable viewer raise errors. These calls do not replace the calculator's separately saved graph scenes. plot.clear restores the Lua 2D window to X=-10..10, Y=-6..6, the surface bounds to X/Y=-3..3 and the camera defaults.

Call plot.clear before changing from a set of curves to a different plot. Adding a 2D curve chooses 2D mode; adding a surface chooses surface mode. Retained callbacks still occupy references until cleared or the session ends.

## Callback contract

A 2D callback receives x and returns one number or nil. A surface callback receives x,y and returns one number or nil. A finite number is sampled; nil or a non-finite numeric result makes a gap. Other result types are errors. An exception from the callback aborts the plot operation and reports its error.

Callbacks may be evaluated many times and in an order chosen by sampling, tracing and redraw. Make them mathematical functions: avoid printing, sleeping, drawing or changing shared state inside them. Do not call plot.clear/window/function2d/surface/show during sampling: changes to the active plot are rejected. Captured parameter values may be used; there is no Lua parameter-slider management API in this release.

## Windows and options

2D and surface bounds must be finite, ordered (minimum strictly less than maximum) and within +/-1e100. An equal pair is invalid. Very large ranges can still be unsuitable for a useful graph.

| 2D option | Meaning |
|---|---|
| colour | A canvas-style colour value; omitted uses the next default curve colour |
| label | Printable ASCII, at most 79 bytes; omitted uses a generated function label |

| Surface option | Default and range |
|---|---|
| xmin,xmax,ymin,ymax | -3,3,-3,3; same finite ordered bound rules |
| yaw | 0.65 radians; finite and absolute value at most 10,000 |
| pitch | 0.55 radians; -1.4 through 1.4 |
| zoom | 1; 0.4 through 2.5 |

Surface options are not the same as 2D colour/label options. A subsequent surface definition inherits unspecified current surface settings; use plot.clear for a clean default. The current API uses the spelling **colour**, not color.

Use the native viewer's displayed controls. Normal viewer Back returns from plot.show; an interrupted sampling operation can instead raise the interruption error. A viewer temporarily owns the display: redraw your canvas after returning rather than assuming earlier pixels remain.

# 6 CHIP-8 ROM interface

## Memory and register map

| Resource | Contract |
|---|---|
| Address space | 4,096 bytes, 0x000 through 0xFFF |
| ROM origin | 0x200; code and data together at most 3,584 bytes |
| Small digit font | 0x050..0x09F; sixteen five-byte glyphs |
| Large digit font | 0x0A0..0x13F; sixteen ten-byte expanded glyphs |
| V0..VF | Sixteen eight-bit registers; VF is used for flags |
| I | Memory index register |
| CALL stack | Sixteen return addresses; not a general data stack |
| DT / ST | Eight-bit delay/sound timers, decremented at approximately 60 Hz |
| Classic / high display | 64 by 32 / 128 by 64 logical monochrome pixels |

A loaded ROM resets the machine, registers and virtual memory, installs the fonts and copies the file at 0x200. Fonts and interpreter-reserved low memory are not general scratch space for portable code. Put data in labelled space within your ROM, or explicitly choose unused addresses within the 4 KiB map. Do not let data and executed code collide.

CALL saves the return address and jumps to an address in this same virtual memory. RET pops it. Nesting past sixteen calls reports overflow; RET without a matching CALL reports underflow. There is no automatic register save. Every subroutine needs a documented register contract.

Ordinary byte arithmetic wraps modulo 256. ADD Vx,Vy sets VF on carry; ADD Vx,byte does not set carry. SUB/SUBN set VF for no borrow. Avoid using VF as a persistent value or a destination that competes with the instruction's flag result.

## Built-in services reached by instructions

- LD F,Vx chooses the small glyph for the low hexadecimal digit of Vx; DRW draws it.
- LD HF,Vx chooses the large glyph and requires a SUPER-CHIP profile.
- LD B,Vx writes the decimal hundreds, tens and ones as three bytes at I.
- LD Vx,K waits for a new keypad press and release.
- LD Vx,DT reads the delay timer; LD DT,Vx and LD ST,Vx set the timers.
- RND Vx,mask produces a masked pseudo-random byte. The generator is reset to a fixed seed on ROM load; this is not secure or unpredictable randomness.

SAVE/LOAD copy registers to/from virtual memory at I. They are not SD save/load operations. FLAGSAVE/FLAGLOAD use eight temporary SUPER-CHIP flag registers, not a persistent document. See the profile table before relying on how I changes after a transfer.

## Keypad and pixels

| PicoCalc keys | Hexadecimal CHIP-8 values |
|---|---|
| 1 2 3 4 | 1 2 3 C |
| Q W E R | 4 5 6 D |
| A S D F | 7 8 9 E |
| Z X C V | A 0 B F |

For example, physical X is keypad value zero; it is not the letter X inside the ROM. SKP/SKNP test held state; LD Vx,K waits for a new press followed by release. Esc belongs to the surrounding application and interrupts the ROM.

DRW XORs sprite pixels. Drawing the same image twice at the same position erases it. VF becomes 1 if a lit pixel was erased. The starting coordinate is reduced modulo the logical display size; pixels extending beyond the right/bottom edge are clipped, not wrapped to the opposite edge.

DRW height 1..15 reads one byte per row for an eight-pixel-wide sprite. Height zero reads 32 bytes for a 16 by 16 sprite and requires SUPER-CHIP. Classic mode scales logical pixels within the backing display; that does not give a ROM colour pixels. Colour .sprite files belong to the Lua/native sprite interface, not CHIP-8's memory format.

# 7 Assembler and profile rules

## Source accepted by this assembler

Write one instruction per line. Separate operands with commas or whitespace. Opcodes, register names and keyword operands are case-insensitive; labels are case-sensitive. A label ends in a colon and can share a line with an instruction. Semicolon or # begins a comment.

Numbers are non-negative integers parsed using C-style base detection: 10 is decimal, 0x10 is hexadecimal, and a leading zero introduces octal. Use 0x rather than calculator #...h notation; # begins an assembly comment. Binary literals, arithmetic expressions in operands, macros, ORG and include directives are not supported.

DB emits bytes and DW emits high-byte-first 16-bit words. Both accept multiple values. Instruction addresses must be even. An odd number of DB bytes before an instruction needs a padding byte. Referencing a label in DW emits its address; it does not execute a call.

| Assembler limit | Maximum |
|---|---|
| Code and data | 3,584 bytes |
| Labels | 96 |
| Label name | 31 bytes, excluding the colon |
| Source line | 255 bytes, excluding newline |
| Tokens on a line | 20, including label/opcode/operands |
| Byte / word / address / DRW height | 255 / 65,535 / 4,095 / 15 |

Keep names simple: letters, digits and underscores, beginning with a letter or underscore. A source build is bounded; a long error line or excessive label table is rejected rather than creating a larger ROM. The editor's independent file and history limits still apply.

## Profile differences implemented by LumaRPL

| Behaviour | COSMAC VIP | CHIP-48 | SUPER-CHIP modern | SUPER-CHIP legacy |
|---|---|---|---|---|
| Shift source | Vy in two-register form | Vx | Vx | Vx |
| OR/AND/XOR reset VF | Yes | No | No | No |
| I after SAVE/LOAD through Vx | Increase by x+1 | Increase by x | Unchanged | Unchanged |
| Offset jump register | V0 | Vx from instruction | Vx from instruction | Vx from instruction |
| Draw waits for timer tick | Yes | No | No | In low resolution |
| EXIT, HIGH/LOW, large font, flags | Unsupported | Unsupported | Supported | Supported |

The table describes this implementation, not every historical interpreter. Profile selection is an external run setting, not an assembler directive. The assembler can encode a SUPER-CHIP instruction even when the selected execution profile cannot run it.

Low-resolution scrolling also differs: modern SUPER-CHIP moves 2*n backing-display rows for SCD n and eight backing columns for SCR/SCL; legacy moves n backing rows and four backing columns. Each low-resolution logical pixel occupies two backing pixels per axis. High-resolution scroll counts operate on high-resolution pixels. Choose the expected profile instead of changing constants until a ROM happens to look right.

# 8 Reusable ROM routines

These routines are **included source**, not resident firmware calls. Copy the routine body and any labelled data into your .asm source. There is no include directive: use the complete example file or paste the routines. Keep labels unique and ensure execution exits or jumps around routine/data storage.

All examples below use SUPER-CHIP modern. Every CALL consumes a return-stack entry until its RET. A routine that calls another routine needs that second body too.

## draw_digit

Input: V0 is a hexadecimal digit 0..15; V1=X and V2=Y. Output: the five-row built-in glyph is XOR-drawn. Clobbers: I and VF. Preserves V0..VE. It does not clear the screen, wait, or call screen.show; the VM renderer presents its dirty display through the host.

Copy from the complete digit example. Values above 15 select their low hexadecimal digit, so decimal 16 displays 0, not the decimal string "16".

```asm
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
```

## draw_u8_decimal

Input: V0 is 0..255; V1=X, V2=Y. Output: three decimal digits including leading zeroes. V0 ends as the ones digit; V1 ends at the starting X+10 modulo 256; V2 retains Y. Clobbers V0..V5, V8,V9, I, VF and the three decimal_digits bytes. Preserves V6,V7 and VA..VE. Requires draw_digit and a private three-byte scratch region. The nested calls use two return-stack entries.

The routine uses LD B,V0, then LOAD V2. It assumes SUPER-CHIP's unchanged-I transfer rule. The supplied source is a complete program showing 237. For an input of 7 it intentionally shows 007. Suppressing leading zeroes would be a different routine and contract.

```asm
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
```

## wait_ticks and beep_ticks

wait_ticks accepts V0=0..255 timer ticks, overwrites DT, and waits until DT reaches zero. It clobbers V1; V0 is preserved. beep_ticks sets ST to the same duration and calls wait_ticks, so it also overwrites DT and clobbers V1. Both return with DT zero under normal timer progress. beep_ticks needs two return-stack entries.

These routines wait on the VM's approximately 60 Hz timers, not on an instruction-count delay loop. Thirty ticks is roughly half a second. They do not preserve an existing countdown, and they cannot choose the sound frequency. For frequency selection use Lua's sound API instead.

```asm
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
```

## wait_key

Input: none. Output: V0 receives the next keypad value after a new press and release. Clobbers V0; preserves V1..VF and I. It blocks until a key arrives or the application interrupts execution. A key already held when waiting begins is not a substitute for a new key event.

The complete example draws the selected hexadecimal digit briefly. Press and release X to draw 0.

```asm
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
```

## add_saturating

Input: V0 and V1 unsigned bytes. Output: V0=min(original V0+V1,255). Clobbers V0,VF; preserves V1..VE and I. VF retains the carry flag from the addition. This differs from ordinary wrapping ADD.

The complete program is a debugger example: 250+20 leaves V0=255 and VF=1. It does not draw a result. Try 10+20 to check the non-carry path: V0=30 and VF=0.

```asm
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
```

# 9 Complete Lua examples

## Measured canvas text

Choose the font, measure, draw a frame, then show it once. The saved previous profile makes this helper polite within a longer script. A key wait keeps the canvas visible. Esc interrupts rather than becoming a returned key.

```lua
screen.clear("black")
local previous=screen.font("compact")
local label="LumaRPL API"
local w,h=screen.measure(label)
screen.text(math.floor((320-w)/2),20,label,"cyan")
screen.rect(8,16,304,h+8,"gray",false)
screen.font(previous)
screen.show()
keyboard.wait()
```

## A plot with a domain gap

The square curve is defined across the window. The logarithm deliberately returns nil at X<=0. That is an explicit gap, not an exception or a zero-valued sample. The callbacks have no drawing, printing or state mutations.

```lua
plot.clear()
plot.window(-4,4,-1,16)
plot.function2d(function(x) return x*x end,
  {colour="cyan",label="Square"})
plot.function2d(function(x)
  if x<=0 then return nil end
  return math.log(x)
end,{colour="orange",label="Log"})
plot.show()
print("Back from graph")
```

## Calculator text and expected failure

The fraction is returned as calculator text. The second request deliberately supplies quotes, which the bridge rejects; pcall turns that ordinary error into false and an error string. The exact fraction's typography may differ from plain ASCII.

```lua
print(calc.eval("1/3"))
local ok,message=pcall(function() return calc.eval("'X'") end)
print(ok,type(message))
```

## Verify an argument error without changing the profile

"tiny" is not an available font profile. Its rejected selection leaves the existing compact profile in place. The final line prints the exported canvas dimensions.

```lua
local ok,message=pcall(function() screen.font("tiny") end)
print(ok,type(message))
print(screen.font())
print(screen.width,screen.height)
```

# 10 Finding the right interface

| Task | Use |
|---|---|
| Exact calculator expression | Lua calc.eval, then interpret the returned text |
| Native curves or surfaces | Lua plot callbacks and plot.show |
| Colour drawing and text | Lua screen API |
| Validated SD colour sprite | Lua screen.sprite |
| Classic monochrome sprite game | CHIP-8 registers, DRW, keys and timers |
| Repeated assembly operation | An included, documented CALL/RET subroutine |
| General file, Wi-Fi or calculator call from a ROM | Not provided by this ROM interface |

## Diagnose a failure

An assembly build error concerns source spelling, operands, alignment or size; it happens before ROM execution. A VM error concerns the executed instruction/profile, memory access or call stack. A blank display can be a correct program that never drew any pixels. Inspect the observable output promised by an example before diagnosing an error.

In Lua, distinguish a thrown error from screen.sprite's nil,error result and keyboard.wait's timeout nil. For native plots, inspect finite bounds and callback returns. In all languages, save source before a large edit and use the editor's error jump when a source location is available.

## Verification scope

The reference is checked against the rc.2 production interfaces. Lua examples run through the production runner and calculator bridge with host device I/O; native plot callbacks use the production sampler. Assembly examples build with the production assembler and run in the production VM, with registers, pixels, timers and call behaviour asserted. Host keys/timer ticks stand in for physical input and time. These checks do not certify LCD appearance, sound output or timing on a device.

# A Lua API and worked examples

The entries below complement the exact contracts in chapters 2-5. Script examples are complete where marked; fragments are intended to be inserted into a program. Save and run examples through the Lua editor. Screen drawings need screen.show; add a key wait to keep a drawing visible at the end of a file run.

@lua-reference

# B CHIP-8 instruction and worked examples

Printed operand placeholders such as Vx and address describe the form; replace them with real registers and labels. Fragments explain an operation and may not draw anything or exit. Use a complete program from chapter 8 as the surrounding ROM, and select the documented profile.

@chip8-reference

# C Quick contract checklist

Before copying a Lua call, check its argument types, default values, return contract, memory cost and whether it needs screen.show. Before copying an assembly routine, check input/output registers, clobbers, data storage, profile and call depth.

The library recipes in this edition are draw_digit, draw_u8_decimal, wait_ticks, beep_ticks, wait_key and add_saturating. They are supplied as examples under the repository's GPL-3.0 terms. Underlying components retain their separate notices. Consult Credits and further reading for the project acknowledgements and the official Lua language manual.
