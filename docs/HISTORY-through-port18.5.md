# Historical guide through port 18.5

These notes record earlier releases. Their menu paths and F-key assignments are historical. Use [PORT19.md](../PORT19.md) for current controls.

# LumaRPL for PicoCalc / Pico 2 W — port 18.5

Separate SD launcher applications for ZX Spectrum, Game Boy and emu8 (NES, C64, Apple II,
Spectrum and other systems) are documented in [EMULATORS.md](EMULATORS.md).
Build their additive archive with `python3 tools/package_emulators.py`.

Native C++ DB48X calculator firmware with a **320×320, 256-colour RGB332 display**,
three themes, setup controls, a HOME-program list, SD browser/source editor,
2D/3D graph tools, CHIP-8/SUPER-CHIP, an assembler/sprite editor, RPL debugging
and bounded Lua 5.4.9 scripting with graphics, input, a console and native plots. No BASIC runtime is involved.

Colour is the only supported calculator build. Earlier monochrome firmware is
retained in historical archives. The user has confirmed the colour build runs.
The user has confirmed the 3D mode works. The user has confirmed the port-5 demos and programming walkthrough worked.
The user has confirmed the unified plots and both port-7 Lua plotting demos work beautifully. The user updated the keyboard BIOS and confirmed battery percentages now display properly. Port-8 analysis/formula/calculus UI needs device confirmation. Host checks and the complete upstream colour-simulator
run are documented in `reports/VALIDATION.md`.

## Port 18.5: responsive editor and Lua text

Cursor movement within the editor viewport updates only the caret and Ln/Col.
The lower controls/footer are retained, and full editor refreshes send once
instead of twice. Scrolling, selection and text changes still redraw as needed.
Lua running/completion messages use compact text. Printed output uses normal
proportional spacing by default; F3 switches to Align columns for tabular output.
All port-18.4 result/navigation improvements are included. Memory budgets and
saved formats remain unchanged; physical responsiveness needs device confirmation.

## Port 18.4: clearer results and navigation

Conversions, calculus, forms and notebooks share a typeset result screen with
F1 Copy to stack, F2 Details and Esc Back. Calculus offers F3 Plot; symbolic
isolation offers F3 Store X. Currency results show the input, observation date
and source using proportional text, without generic preview messages. Solver
and graph-analysis actions appear on the result page rather than after Esc.
Information pages wrap words and support Left/Right page scrolling. Numbered
menu rows explain the 1–8/F1–F5 shortcuts. Confirmations keep the question visible,
start on Cancel, and require an explicit confirmation. Editor F2 is labelled
Save & run to match its behavior. Existing file formats and memory budgets remain.

The user confirms exchange-rate downloading and cached conversion after restart.
Editor recovery, saved files and extended mixed use remain device checks.

## Port 18.3: LumaRPL, credits and compact information screens

LumaRPL is powered by DB48X. Setup → About / credits shows the firmware version,
credits Iain Bennett for design direction and device testing, and acknowledges
the upstream components. Full notices remain in `/db48x/licenses/` and
`/db48x/licenses/PORT-NOTICES.md`. Existing firmware filenames and SD paths are unchanged.

Information, help, results and diagnostics use smaller text, including Memory /
PSRAM and Battery status. Menu selection, source/formula editing, the calculator
stack and graph rendering retain their established sizes. The user confirms PSRAM,
Wi-Fi association, NTP and graph appearance; exchange-rate download is pending
better coverage. See `reports/PORT183.md` and `HARDWARE-TEST.md` for remaining checks.

## Port 18.2: compact Wi-Fi diagnostics

Radio status, update progress and update results use the existing 13-pixel compact
font. Diagnostic results wrap to the pixel width and show 15 rows per page, with
Up/Down scrolling retained. No extra font or framebuffer is allocated.

## Port 18.1: Wi-Fi association recovery

Generic association failures now allow SDK recovery followed by up to two explicit
retries, within the existing 35-second join deadline. WPA2-AES authentication stays
unchanged. Rejected credentials do not trigger security fallback. Update diagnostics
show the radio firmware event, status and reason, rather than calling every failed
association a driver error. Failed updates retain cached rates; Esc cancels.

The user confirms port-18 PSRAM passes on the device. Port-18.1 Wi-Fi still needs
physical verification; see `reports/PORT181.md`. Calculator memory budgets stay intact.

## Port 18: memory and PSRAM

| Budget | Port 17.3 | Port 18 |
|---|---:|---:|
| Colour framebuffer | 200 KiB | **100 KiB** |
| Calculator arena, internal SRAM | 160 KiB | **208 KiB** |
| Bounded general heap, internal SRAM | 64 KiB | **96 KiB** |
| Lua live-payload limit within that heap | 28 KiB | **48 KiB** |
| Reserved stack, internal SRAM | 32 KiB | **32 KiB** |

The framebuffer saves exactly **100 KiB**. Of that, 48 KiB goes to the calculator
arena, 32 KiB to the heap and 16 KiB to one shared working buffer; the remainder
provides room for metadata and additional separation below the stack. Lua's
limit is part of the heap budget, not an additional reservation.

**Setup → Memory / PSRAM** reports the boot self-test, detected capacity and
backing-slot usage. A working standard PicoCalc chip reports **8192 KiB**.
The calculator arena, GC objects, heap and stack always stay in internal SRAM.
PSRAM is explicit serial storage, not an 8 MiB expansion of `malloc` or Lua.

Inactive forms, notebooks, data tables, parametric/polar scenes and ODE scenes
can release **8392 bytes** of document payload from the heap when all have been
visited. Editor undo uses PSRAM with one small hot record in SRAM. The editor's
16 KiB active text buffer shares storage with source loading and native plots;
when running a program, its saved text can be parked in PSRAM and restored afterwards
(Lua return or RPL runtime error), retaining checked undo with an identical snapshot.
Native graph samples are backed in PSRAM while waiting for input, with checksum
validation before reuse. Cache failure triggers fresh sampling. Code never
reuses samples across plot invocations when calculator variables may have changed.

The 8 MiB chip offers **8128 KiB** of backing slots after reserving the first
64 KiB. Slots are 64 KiB each, independently owned and released; actual payloads
are smaller. Documents are written and read back before their SRAM copies are
released. A failed park keeps the current SRAM copy. Missing PSRAM keeps
documents and undo in the bounded heap. PSRAM is volatile: **save to SD before
powering off**. The editor still accepts UTF-8 sources below 16 KiB; this release
does not raise document/file-size limits.

Rendering uses a fixed RGB332 palette (8 red × 8 green × 4 blue levels), with a
small LCD row-transfer buffer. Existing colour settings, RGB565 pixmap files,
programs, scenes and sessions retain their formats. Colours are quantized when
displayed. Fonts and all existing native plotting views use the same framebuffer.

For Wi-Fi, port 18 explicitly requests the SDK's preferred **WPA2-AES** mode,
in place of mixed WPA/WPA2 authentication. Your screenshot's **2.4 GHz Standard
(WPA2)** setting is appropriate; the Pico 2 W uses 2.4 GHz. Keep that router
setting while testing the new firmware. WPA3-only/enterprise networks are not
supported by this firmware's connection workflow. NTP, timezone and dated-rate
behaviour remain as documented below. The authentication fix needs testing on
your network; host HTTPS tests do not establish radio interoperability.

## Upgrade through your existing bootlauncher

1. Save your current calculator session with Ctrl+S, then Ctrl+Q to the loader.
2. Back up the SD card, especially `/db48x/state/`, `/db48x/data/`, `/db48x/chip8/`, `/db48x/lua/` and `/db48x/graphs/`.
3. Extract `dist/DB48X-PicoCalc-Pico2W-Colour-SD.zip`. Merge its `pico2-apps/` and
   `db48x/` folders into the SD root. Preserve your existing state and data files.
4. Keep the existing UF2 Loader **v2.4.1** and root `BOOT2350.uf2`.
5. Select **db48x-picocalc-pico2w-colour.uf2** from the loader menu.

The SD package includes only the current calculator firmware. Previous ports,
including the user-confirmed port 8, remain archived locally under releases/.
Your original PicoCalc-SD-Pico2W package remains untouched.

Calculator sessions are saved to **`/db48x/state/Session-colour.48S`**.
Legacy `Session.48S` import remains available for existing data; original files
are preserved. There is no monochrome fallback in current releases.

The colour framebuffer is **100 KiB**, with a **208 KiB** calculator arena,
bounded **96 KiB** heap and reserved **32 KiB** stack. Bulk storage uses the onboard
8 MiB serial PSRAM when its boot self-test passes; SRAM fallback remains usable.
A single **16 KiB** workspace serves the editor, source loading and native graph samples. No second core is used. Port 17 adds on-demand Pico 2 W Wi-Fi
and verified HTTPS rate updates without reducing these reservations.

## Main keyboard

| Key | Action |
| --- | --- |
| Ctrl+T | Setup: themes, angles, programs, graphs, save/load and launcher |
| Ctrl+P | Programming: RPL, CHIP-8, Lua, sprites and variables |
| Ctrl+G | Graph tools |
| Ctrl+W | Math workbench: equations, systems and parameters |
| Ctrl+D | Data tables, statistics and regression |
| Ctrl+F | Reusable formula forms |
| Ctrl+N | Calculation notebooks |
| Ctrl+S | Save the colour session, including theme and calculator settings |
| Ctrl+L | Reload the saved colour session, with a recovery snapshot first |
| Ctrl+Q | Save and return to UF2 Loader |
| F1–F5 / Ctrl+F5 | Six calculator soft-menu columns |
| Tab | Cycle calculator shift planes |
| Ctrl+B / Ctrl+E | Insert RPL program delimiters `«` / `»` |
| Enter / Esc / Backspace | Evaluate / cancel or interrupt / edit |
| Arrows | Navigate; Left/Right move through UTF-8 calculator text |

In native menus, use Up/Down and Enter, number keys, or F1–F5 for the first
five rows. Esc goes back. Prompts accept Enter to keep the displayed default;
typing replaces it, and Backspace edits the replacement. Left/Right moves the
prompt cursor to edit existing text; long input wraps with a visible cursor.

## Themes and setup

Press Ctrl+T, choose Colour theme, then Classic light, Midnight blue, or Amber
terminal. Choosing a theme changes only display colours. Angle mode offers
Degrees, Radians and Grads. Use Save session or Ctrl+S to retain these choices
across restart. Custom upstream colour commands are also available and persist
in the colour session; themes are not reapplied over a saved colour session.

## Programs and source editor

Finish or cancel any pending calculator entry before opening Ctrl+P.

- **HOME programs** lists top-level program variables, including imported
  programs. Select one and press F1/Enter to run it using the current stack.
  F2 exports a source copy into `/db48x/data/`; exporting does not change the
  original HOME variable. Save the calculator session to persist HOME changes.
- **RPL files / editor** browses `/db48x/data/` in pages of eight, with no fixed
  directory-entry limit. Up/Down selects, Left/Right changes page. Files appear
  in filesystem order. F1/Enter runs, F2 edits, F3 creates, F4 reloads, F5 exits.
- Source files use `.48s` or `.txt`; existing `.48b` objects can run but cannot
  be edited as text. Source editing/running is limited to **less than 16 KiB**
  per file and requires valid UTF-8. Larger programs remain accessible through
  upstream calculator file commands within available calculation memory.
- In the editor, Enter inserts a newline, Tab inserts spaces, arrows move, and
  Backspace deletes a UTF-8 character. Ctrl+B/Ctrl+E insert `«` and `»`.
  **F1 or Ctrl+S saves the file. F2 saves and runs, returning to the calculator.
  F5 or Esc closes, asking before discarding unsaved edits.** Source syntax is
  checked before saving. Files use decimal `.` syntax.
- A successful file save is immediately on SD; it does not require a separate
  session save. Temporary-file promotion retains the previous source as `.bak`.
  Running a program can change calculator variables/stack; use Ctrl+S afterwards
  if you want those resulting calculator changes saved.

Try creating a program called `TEST` with this source:

```rpl
«
2 3 +
»
```

Ctrl+P → RPL files / editor → F3 → name TEST → Enter. The editor starts with the
outer delimiters and the cursor between them. Type `2 3 +`, then F2. The top
of the calculator stack should be 5. Reopen the browser after a restart to run
`TEST.48s` again.

A program that needs arguments, such as SQDEMO, expects those numbers on the
calculator stack before you open the browser and choose Run.

## Graph tools

Press Ctrl+G. Plot 2D and Enter 2D formula use the same native function view.
Enter an expression in **X**, without quotes or `y=`; the current formula is
prefilled. Entering one formula replaces Y1 and disables Y2/Y3. Trigonometry
follows the calculator angle mode; choose Radians in setup for SIN(X).

2D window edits finite ordered numeric bounds shared by single and multi-curve
plots. Plot colour rotates the three native curve colours. For more curves,
open Multi-curve 2D. Save graph settings explicitly through Graph scenes.

In the 2D view, **Left/Right traces**, **Up/Down selects a curve**, **F1 opens a
value table**, **F4 toggles pan/trace**, and **+/- zooms**. In pan mode, arrows
move the window. **Esc or F5** returns; Esc again returns to the calculator.
Esc interrupts sampling or a long calculation. The low-level upstream RPL plot
commands and parameter objects remain available with their original semantics.

## Storage, recovery and current limits

Use the loader's FAT16/FAT32 SD card; exFAT is unsupported. All calculator paths
are rooted in `/db48x/`. Save/close errors are checked before promoting a saved
session or edited source file. The prior session is retained as `.bak`, with
startup fallback if the main file is absent or unreadable. Explicit colour
session loading makes `BeforeLoad-colour.48S` first. This is backup-based recovery,
not a guarantee against FAT metadata corruption during sudden power loss.

Without SD the calculator and graphing still work; persistent saves, files and
help are unavailable. Insert SD and restart to mount it. The HOME program list
also works without SD. Use the existing loader's USB mode to transfer files;
there is no calculator USB mass-storage mode. Full-screen colour pixmaps are
larger than the calculation arena; smaller graphical objects are supported.

Battery display uses the controller's percentage, not a measured voltage. The
software clock is session-only and starts at 2026-01-01 after cold boot. Idle
uses timed polling/backlight control rather than deep sleep. SwissMicros-specific
flash tools and screenshots remain unsupported. The native setup/program tools
replace common workflows; the legacy SwissMicros file-selection dialogs are
still unavailable.

See `BUILDING.md`, `PORTING.md`, `HARDWARE-TEST.md`, `NOTICE.md`, and
`SOURCES.lock.json`. Corresponding source and checksums accompany the package.

## 3D wireframe surfaces (port 3)

Open **Ctrl+G -> F5 (3D wireframe)**. Choose the saddle example to start, or
**F2 (Enter Z=f(X,Y))**, type `X^2-Y^2`, and press Enter. Enter only the expression,
without `Z=` or quotes. The focused field wraps and scrolls longer input.
Plot current formula shows the current expression in its menu label.

- **Arrows:** rotate left/right and tilt up/down.
- **+ / -:** zoom in/out. **F1:** reset the camera.
- **Esc or F5:** return from the viewer to the 3D tools; Esc again returns
  to graph tools. Graphs now has ten rows; use Esc or row 10 to go back.
- **Set X / Y bounds (F5 in the tools menu):** choose finite ordered bounds,
  then choose Plot. Defaults are -3..3 on both axes.
- Z scales automatically to the finite sample range shown below the plot.
  X, Y and Z labels mark the three edges from the minimum corner of the box.
  Each dimension scales independently; this is not an equal-unit 3D aspect ratio.
- **Esc cancels sampling or a long expression.** Progress shows each sampled row.
  Undefined, complex and non-finite samples become gaps; a wholly invalid
  surface reports that there are no finite real samples.

The 17x17 grid is a coarse, transparent wireframe, with no shaded surfaces or
hidden-line removal. Narrow features or poles between samples may be missed.
Rotation and zoom reuse the mesh; plotting again resamples. Sampling uses the
upstream DB48X evaluator in scoped 16-digit hardware-floating-point mode;
normal calculator precision and arithmetic settings are restored afterwards.
Trigonometry follows your selected angle mode: choose Radians in Ctrl+T for the
`SIN(X^2+Y^2)` ripple example. Stored X/Y, stack entries, EQ and 2D plot bounds
are preserved. Formulas may read other calculator variables/functions.

The 3D expression, bounds and camera remain in memory until restart. Use Graph
scenes (below) to save and restore them on SD. SVG/CSV export and on-demand Wi-Fi are available in current releases. No extra framebuffer, PSRAM or second core is used.

## Interface cleanup (port 4)

The Graphs menu is now: Plot 2D, Enter 2D formula, 2D window, Plot colour,
3D surfaces, Multi-curve 2D, Graph scenes, Calculus / formulas, Parametric / polar, Back. The first five key positions
are unchanged. The 3D submenu uses short
labels: Plot, Enter formula, Saddle example, Ripple example, X/Y bounds, Back.
The standard calculator header uses a dedicated compact font. The internal
Session-colour filename displays as Session; saved files and formats are unchanged.

## Programming tools (port 5)

Ctrl+P opens: HOME programs, RPL files / editor, CHIP-8 / assembly, Lua scripts,
Sprite editor, HOME variables, Lua console, Back. F1-F5 select the first five rows; arrows,
Enter and number keys reach all rows. Esc always goes back. Finish/cancel any
calculator entry before opening the tools. New resources are isolated under
/db48x/ and do not replace user session files or the existing bootloader.

### CHIP-8 / SUPER-CHIP

Open Ctrl+P -> CHIP-8 / assembly, select MOVE.ch8 and press F1. The original
bundled demo moves with W/A/S/D. ROMs are raw .ch8 files, 1..3584 bytes, in
/db48x/chip8/. HP48 string containers must be converted to raw ROM bytes;
Saturn machine-code programs are not executed by this VM.

The on-screen hex keypad maps as follows:

```text
CHIP keys       PicoCalc keys
1 2 3 C         1 2 3 4
4 5 6 D         Q W E R
7 8 9 E         A S D F
A 0 B F         Z X C V
```

F1 pauses/resumes, F2 restarts, F3 cycles speed (7/20/50/100 instructions per
60 Hz frame), and Esc exits to the calculator. Held keys are tracked from
keyboard press/release events. Battery/SD/LCD behaviour and keyboard rollover
still require device testing. A sound timer drives the existing buzzer.

F4 in the file browser selects CHIP-8/VIP, CHIP-48, SUPER-CHIP modern, or
SUPER-CHIP HP48-style quirks. The chosen profile is saved for the selected ROM
(or the ROM that will be compiled from the selected .asm file) as .ch8.cfg.
SUPER-CHIP is the default. Profiles cover shifts, memory-register operations,
jump offsets, logic flags, clipping, drawing wait and low-resolution scrolling;
they are not cycle-exact emulations of the original machines. XO-CHIP is not
implemented. Both 64x32 and 128x64 graphics use the existing colour framebuffer.
SUPER-CHIP's eight saved flags persist per ROM in .ch8.flags, with .bak recovery
copies retained after successful replacement. A failed write reports an error.

### Write and assemble a CHIP-8 program

In CHIP-8 / assembly, F3 creates an .asm source file; F2 edits existing source.
F1 in the source editor saves; F2 saves, assembles and runs the resulting .ch8.
The previous ROM survives a failed assembly or failed replacement. Source is
limited to less than 16 KiB and ROM output to 3584 bytes. Errors report a line.

The native assembler uses line-oriented mnemonics, labels ending in a colon,
semicolon or # comments, decimal/0x hexadecimal numbers, and DB/DW data. Octo
.8o source can be compiled on a computer and its .ch8 output copied here; the
native assembler does not compile the Octo language.

```asm
CLS
LD V0, 10
LD V1, 10
LD I, sprite
DRW V0, V1, 5
wait:
JP wait
sprite:
DB 0xF0, 0x90, 0x90, 0x90, 0xF0
```

Supported instructions include CLS, RET, EXIT, LOW, HIGH, SCR, SCL, SCD;
JP/CALL/JPOFF; SE/SNE; LD; ADD/OR/AND/XOR/SUB/SUBN/SHR/SHL; RND; DRW/DRAW;
SKP/SKNP; SAVE/LOAD and FLAGSAVE/FLAGLOAD. LD supports I, DT, ST, K, F, HF, B
and [I] forms. Registers are V0..VF. Keywords/registers ignore case; labels are
case-sensitive, up to 31 characters, with a 96-label limit. Lines are at most 255 bytes/20 tokens. Instructions must
start at even addresses; add a DB padding byte after odd-length data if needed.
JPOFF uses the Bnnn opcode and therefore follows the selected profile's register
quirk. DB is an 8-bit value and DW a big-endian 16-bit value.

The host assembler is reproducible with:

```sh
clang++ -std=c++17 -Ipico tools/assemble_chip8.cc -o build/assemble-chip8
build/assemble-chip8 resources/chip8/MOVE.asm /tmp/MOVE.ch8
```

Ctrl+P -> Sprite editor opens a 16x16 pixel grid. Arrows move, Space/Enter toggles
a pixel, F1 saves a complete .asm example into /db48x/chip8/, and Esc exits.
Use a SUPER-CHIP profile for its HIGH/16x16 drawing instructions. The saved
source remains editable in the assembly browser. The sprite grid itself is
new/blank on each visit; editing existing sprites is done in their source data.

### RPL development and debugging

The RPL editor colours comments, strings, numbers, uppercase command-like text
and delimiters. These are lexical colours, not a semantic validator. F1 checks
RPL syntax before saving. When DB48X supplies a source pointer, syntax errors
show line/column and move the editor cursor there. Runtime error dialogs include
the faulting command when available; compiled RPL does not retain source-line
mapping for every runtime instruction.

F4 in the RPL source editor saves and starts debugging. F4 in HOME programs
starts debugging the selected program with the existing calculator stack.

- F1: one instruction; F2: step over; F3: continue.
- F4: inspect HOME variables (arrows select, Enter opens a scrolling value).
- F5 or Esc: stop and clear the pending debug execution; current stack/variable
  effects remain. Completed programs also wait here until you close the debugger.
- The debugger displays the next instruction and top six stack entries.

Try Ctrl+P -> RPL files / editor -> DEBUG_DEMO.48s -> F2 Edit -> F4 Debug.
Step through 2, 3, +, 4, *; the final result is 20. The debugger uses DB48X's
native execution mechanisms, so complex commands may take time within a step;
Esc remains the calculation interrupt. Variable values are read-only previews
limited to 2047 displayed bytes, with scrolling. Save the calculator session
separately if you want resulting stack/variable changes retained across restart.

### Lua 5.4.9

Open Ctrl+P -> Lua scripts -> HELLO.lua -> F1. F3 creates a script; F2 edits.
In the editor F1 saves and F2 saves/runs; Esc closes the output viewer and
returns to the editor/browser. Scripts use /db48x/lua/ and the same source-size
limit. Standard Lua syntax is supported, with base, math, string, table, UTF-8
and coroutine libraries. File I/O, os, package/native module loading and debug
libraries are not exposed. Graphics/input, timing and sound APIs are available as described below, along
with printed output and the calculator-expression bridge.

```lua
print("Hello PicoCalc")
for i = 1, 5 do print(i, i * i) end
print(calc.eval("1/3"))
print(calc.eval("2^10"))
```

calc.eval accepts one DB48X algebraic expression without quotes, returns its
formatted result as a Lua string (including exact fractions and display
separators), and restores the stack depth for valid expressions. It follows
calculator angle/precision settings and can read calculator variables/functions.
User-defined RPL functions retain their ordinary side effects. Lua's own numbers
use its normal integer/double arithmetic, independently of DB48X precision.

Each script run starts a fresh Lua state and releases it afterward. Script
globals do not persist between runs; console globals last until the console is
closed. Source files persist immediately when saved. Lua uses a
48 KiB live-payload allocation budget inside the existing 96 KiB bounded heap,
plus normal allocator metadata. C nesting is capped at 40 levels. Memory failures
and excessive nesting return errors; Esc interrupts Lua bytecode through an
instruction-count hook. Long individual C/math-library calls finish before the
next hook. Output is limited to 2047 bytes, with a truncation notice and scrolling.
The viewer reports peak Lua payload use. HELLO and FIBONACCI examples are bundled.

## Lua graphics, input and sound (port 6)

Ctrl+P -> Lua scripts -> DRAW.lua -> F1 draws a colour example. BALL.lua is an
interactive animation: arrows move the ball, Q finishes and Esc interrupts.
TONE.lua plays four short notes. Graphics remain visible when a script finishes;
press a key for its output, then Esc to return. No additional framebuffer is used.

```lua
screen.clear("black")
screen.line(10, 10, 250, 200, "cyan")
screen.rect(20, 40, 80, 50, "orange", false)
screen.circle(160, 160, 25, "green", true)
screen.text(10, 240, "Hello PicoCalc", "white")
screen.show()
```

| Function | Behaviour |
| --- | --- |
| screen.clear(colour) | Clear the framebuffer; default black |
| screen.pixel(x,y,colour) | Draw a pixel |
| screen.line(x1,y1,x2,y2,colour) | Draw a one-pixel line |
| screen.rect(x,y,width,height,colour,filled) | Rectangle; filled defaults true |
| screen.circle(x,y,radius,colour,filled) | Circle; filled defaults false |
| screen.text(x,y,text,colour) | Small-font UTF-8 text; newline supported |
| screen.show() | Transfer the current framebuffer to the LCD |
| keyboard.poll() | Next key press, or nil |
| keyboard.wait(timeout_ms) | Wait for a press; nil on timeout; omitted/0 waits indefinitely |
| keyboard.down(key) | Whether a key is currently held |
| system.sleep(ms) | Delay while checking Esc |
| system.millis() | Milliseconds since boot, wrapping after about 49 days |
| sound.beep(hz,ms) | Play a tone; 20..16000 Hz, or 0 for silence |
| sound.stop() | Stop the tone |

screen.width and screen.height are 320. Coordinates round to integer pixels;
the origin is top-left, with Y increasing downwards. Drawing clips to the screen.
Colours accept black/white/red/green/blue/cyan/magenta/yellow/orange/gray,
#RRGGBB strings or 0xRRGGBB integers. Default drawing colour is white.
Coordinates are bounded to -4096..4096; radii/dimensions to 0..640. Text calls
accept at most 1024 bytes and replace invalid UTF-8 for display. Delays are at
most 600000 ms and tones at most 10000 ms. A C drawing/library call completes
before the next interrupt check; waits, bytecode and sleep check Esc regularly.

Key names are lowercase characters, space, enter, backspace, left/right/up/down,
and f1..f5. Esc is reserved to interrupt the script. Key polling uses press
transitions; down() tracks held keys and multiple simultaneous keys. The event
queue holds the latest 15 presses. Actual keyboard rollover/performance needs
device verification. Printed output remains bounded to 2047 bytes. Screen images
are not retained after leaving the result view; scripts should draw full frames.

### Interactive console

Ctrl+P -> Lua console (row 7). Enter `2+3` to see 5. Assign `x=10`, then enter
`x*2`; the console retains x until you exit. Expressions print up to 16 returned
values. Statements with no output return directly to the prompt. Up recalls the
previous input line; Down clears it. Use :help for guidance and :quit to close.

Functions and loops accept multiple input lines. For example enter
`function twice(a)`, then `return a*2`, then `end`, then `twice(12)` to get 24.
An incomplete chunk shows the continuation prompt. Esc cancels an unfinished
chunk; Esc at a fresh prompt exits. Chunks are limited to 4095 bytes, with
511 bytes per input line, within the same 48 KiB live Lua-payload budget.
Console errors are protected and the session can continue afterward. Globals
are not automatically saved to SD; put reusable definitions in a script file.

### Source editor improvements

RPL, assembly and Lua editors now show logical line numbers and a line/column
status. Long lines wrap at 28 characters; repeated line numbers indicate wrapped
continuations. Highlighting recognizes each language's line comments and quoted strings,
plus Lua keywords. It is lexical highlighting, not a full language parser.

| Shortcut | Action |
| --- | --- |
| Ctrl+K, then arrows | Mark/extend a selection; Ctrl+K or Esc clears it |
| Ctrl+A | Select all |
| Ctrl+C / Ctrl+X | Copy/cut selection; without selection, copy/cut the current line |
| Ctrl+V | Paste, replacing a selection |
| Ctrl+Z / Ctrl+Y | Undo/redo |
| Ctrl+F / Ctrl+R | Find / replace next or all |
| Ctrl+G | Go to logical line |
| Tab / Ctrl+U | Add two spaces / remove up to two leading spaces |
| Enter | Newline retaining current indentation |
| Ctrl+O or F3 | Editor shortcut help |

The clipboard holds 256 bytes and survives switching files until restart.
Individual replacements/selections being changed are limited to 256 bytes;
large operations report an error without changing the source. Undo keeps eight
recent edits, with contiguous typing grouped up to 256 bytes. Navigation ends a
typing group. Replace-all preflights capacity and records individual matches,
so only its last eight replacements may be undone. Undo history is per editor
visit and is not saved. Find/replace is literal and case-sensitive. Save/run/debug
keys remain F1/F2/F4 (RPL debug only), with F5 closing the editor. File saves and
program side effects are separate from text undo.

## Multi-equation plots and graph scenes

Ctrl+G -> Multi-curve 2D (row 6) edits up to three formulas in X. Empty input
turns a curve off. Plot displays them in separate colours on a Cartesian grid.
The view uses 300 samples per curve, scoped 16-digit floating-point evaluation
and the current calculator angle mode. Stored X/Y variables and calculator
precision/arithmetic settings are preserved. Other variables are read normally.
Narrow features and poles between samples can be missed; this is numerical
sampling, not an analytic continuity guarantee.

Left/Right moves the trace cursor, Up/Down selects a curve, +/- zooms, F1 opens
a nearby value table, F2 opens analysis, F4 toggles pan/trace mode, and Esc/F5 returns. In pan mode
arrows move the window. Multi-curve tools have their own bounds and colour
rotation. After selecting multi-curve mode, the main Plot 2D entry uses it;
Enter 2D formula replaces Y1 and disables Y2/Y3, using the same native Plot 2D
view, controls and bounds. It also stores EQ for compatibility with RPL programs.
The Graphs menu no longer launches the old single-EQ renderer. On first use,
a compatible legacy function EQ/window is imported into the native scene.
Non-function legacy plot types report that a native function formula is needed.
Low-level upstream RPL drawing commands remain available with their original semantics.
Root/intersection analysis is described below; image export is not implemented.

Ctrl+G -> Graph scenes (row 7) saves/loads named .scene files under /db48x/graphs/.
A scene contains the native function formulas, colours, 2D bounds, 3D formula,
3D bounds/camera and angle mode. Loading asks before applying the angle mode,
and the loaded 2D scene uses the native trace view. Open 3D surfaces -> Plot to
restore its 3D view. Legacy single function EQ/bounds are captured when possible;
other upstream plot types must use the native function tools before saving.
Unrelated calculator variables, programs and themes are not scene contents.

Save a scene explicitly before restarting; Ctrl+S saves the calculator session,
not the separate native graph scene. After restart use Load scene. EXAMPLE.scene
is bundled for trying X^2 and X with a saddle surface, in Radians mode. Scene
files carry a version and checksum. Successful replacement retains a .bak;
loading tries the backup if the primary is missing, truncated or corrupt.
No SD card means native graphing remains usable, while scene saving/loading
reports unavailable. Invalid scenes leave the current native scene intact.

## Unified native plots and Lua plotting (port 7)

All function plots in the Graphs menu, including Enter 2D formula, now use the
native Plot 2D view with its Cartesian grid, coloured curves, trace, pan/zoom
and F1 value table. 3D retains the existing wireframe renderer and camera controls.
Both views share a bounded sampling backend and accept either DB48X or Lua
function callbacks. Existing .scene files remain compatible.

Try Ctrl+P -> Lua scripts -> PLOT2D.lua or PLOT3D.lua -> F1. Esc closes the plot
and resumes the script. F5 also closes; in 3D, F1 resets the camera. Esc during
sampling or inside a long-running callback interrupts the Lua script.

```lua
plot.clear()
plot.window(-6.3, 6.3, -2, 2)
plot.function2d(function(x) return math.sin(x) end,
                {colour="cyan", label="sin(x)"})
plot.function2d(function(x) return math.cos(x) end,
                {colour="orange", label="cos(x)"})
plot.show()
```

```lua
plot.clear()
plot.surface(function(x, y) return x*x-y*y end,
             {xmin=-3, xmax=3, ymin=-3, ymax=3})
plot.show()
```

| API | Behaviour |
| --- | --- |
| plot.clear() | Release all plot functions; reset bounds, colours and camera |
| plot.window(xmin,xmax,ymin,ymax) | Set the 2D view bounds |
| plot.function2d(fn, options) | Add a curve, up to three; optional colour and label |
| plot.surface(fn, options) | Set a 3D function; optional xmin/xmax/ymin/ymax/yaw/pitch/zoom |
| plot.show() | Open the current native interactive view; return when closed |

function2d selects the 2D view; surface selects 3D. Switching views retains the
other view's functions until clear(). Repeated show() retains the Lua scene's
window/camera adjustments. Curve labels are printable ASCII up to 79 bytes.
Colours use the screen API's names, #RRGGBB or integers. Bounds must be finite,
ordered and within +/-1e100. Surface camera pitch is -1.4..1.4, zoom 0.4..2.5,
and yaw within +/-10000, in radians.

Return a number from each function; nil, NaN and infinity create domain gaps.
Callback errors and memory exhaustion return a Lua error after native cleanup.
Sampling uses 300 points per curve and 17x17 surface points. Zoom/pan and the
value table can call functions again: use functions without side effects.
Changing plots or recursively showing a plot inside a callback is rejected.

Calculator formulas continue to use DB48X with scoped 16-digit floating-point
plot evaluation and the calculator angle mode. Lua callbacks use Lua numeric
arithmetic; math.sin/cos use radians regardless of the calculator angle setting.
Lua plots have their own session state and do not replace the calculator's
formulas, graph scenes or angle mode. Lua closures are not written into .scene
files: save the script to reproduce them after restart. The Lua payload cap
remains 48 KiB; native 2D samples add a 7200 bytes of the shared workspace and 3D
uses a 17x17 stack mesh. No extra framebuffer is allocated.

## Battery reporting (port 7.1)

Battery reads now use the reference driver's 16 ms register-settle pause.
Valid controller percentages are 0..100 with bit 7 indicating charging.
Unavailable replies retry after one second; valid reads are cached for ten
seconds. A transient error can retain the last valid reading for up to thirty
seconds, after which the percentage becomes --% and the icon shows a question
mark instead of looking full. The fill is proportional to percentage.

Ctrl+T -> Battery status (row 8) shows percentage, charging, last raw I2C reply
and sample age. F1 requests a new transaction. If the fix still shows --%, report
that screen's reply on battery and USB power. Charging is the controller's
charging flag, not an independent USB-cable detector. No PMU settings or keyboard
controller firmware are changed.

Ctrl+Q saves and returns to the existing loader. Holding Up/F1/F5 is a loader
startup gesture: disconnect Pico USB, fully power off, then hold the key during
power-on. It is not a runtime exit combination. The loader remains unchanged.

## Battery transport follow-up (port 7.2)

Port 7.1 still reported an I2C transaction failure on the user's device. Its
10 ms timeout remained shorter than the reference UF2 Loader's 500 ms timeout.
Port 7.2 uses the reference 500 ms timeout for battery writes/reads plus the
16 ms pause. Keyboard polling retains its existing 10 ms timeout and no pause.
Failed battery transactions back off for five seconds; F1 in Battery status
forces an immediate retry. A failed query can briefly delay input while waiting
for the controller; no extra delay is imposed when transfers finish promptly.

Ctrl+T -> Battery status now shows Write n/1 and Read n/2. Success is Write 1/1,
Read 2/2 with a valid raw reply. -1 is an SDK bus error and -2 is a timeout;
Read 0/2 after a failed write means no read was attempted. Partial reads are
rejected. Please report both results and the failure stage if still unavailable.
This is a candidate transport fix; hardware success is not yet confirmed.

Port 7.2 also reads keyboard-controller version register 0x01 and shows the
BIOS version in Battery status. Official versions 1.0/1.1 predate the documented
battery register, so the app skips battery access and reports the 1.2 requirement.
Unknown version formats are not labelled unsupported. F1 reprobes the version
and requests a fresh battery read. No STM32 firmware is installed by this UF2.

The developer documents Alt+B as an independent coarse LED battery check.
For diagnosis, start fully powered off with all USB disconnected, power on
from batteries, then try Alt+B. See BATTERY-RESEARCH.md included with the SD bundle for primary sources,
known firmware issues and what remains unconfirmed on this device.

## Graph analysis, calculus and formula entry (port 8)

The unified native Plot 2D view now opens **F2 Analyse** for the selected curve.
This works for calculator formulas and Lua functions shown with plot.show().
Select a curve with Up/Down before opening analysis. F1 still opens the value
table; F4 still toggles pan/trace. The 3D renderer is unchanged.

| Analysis | Result |
| --- | --- |
| Roots / zeros | Up to 16 resolved zeros within an entered X interval |
| Intersections | Choose another active curve, then enter an X interval |
| Minimum / maximum | Sampled/refined interval extremum, including endpoints |
| Slope at X | Approximate derivative at an entered X |
| Definite integral | Signed integral over entered bounds; reversed bounds negate it |
| Area | Integral of absolute function value, always nonnegative |

Bounds and X values accept calculator expressions such as 1/3, 2*3 and π,
not just decimal numbers (type PI for π). Results show coordinates or approximate values,
estimated errors where available, and evaluation counts. After viewing a result,
choose whether to return or copy X, Y or the numerical value to the calculator
stack. With several roots/intersections, select its numbered point. Ctrl+S saves
copied stack results with the calculator session.

Resolved points are marked on the plot; integrals/areas shade the selected
curve against zero within the interval, clipped to the visible viewport. Changing
curve or window clears overlays. Overlays are transient; .scene files still save
formulas/window/colours/camera, and their format is unchanged.

### Formula editor

Enter 2D formula, each multi-curve formula, 3D formula and calculus formula use a
focused editor. Initial text is selected: typing replaces it; arrows begin editing.
Enter validates and accepts; Esc restores the original formula.

- F1: SIN/COS/TAN/LN/EXP/SQRT, X and π insertion.
- F2: fraction, power, square, parentheses and ABS templates; also differentiate
  or take the antiderivative of the **entire current formula** using DB48X.
- Tab: jump to the next empty parentheses slot, for example a fraction denominator.
- F3: syntax check without leaving the field.
- F4: bounded typeset math preview (fractions, powers, radicals); press a key to
  return to editing. This is a source editor with a preview, not a two-dimensional
  cursor-based equation editor.
- Ctrl+A: select all; arrows move by UTF-8 character/visual row; Backspace edits.

The formula limit remains 159 UTF-8 bytes. Templates leave the cursor inside
the first argument. A transformation that is unsupported or too long leaves
the input unchanged. Syntax checking does not establish that all X values are
real or defined. Functions can still have domain gaps while plotting.

### Symbolic and numerical calculus

Open **Ctrl+G -> Calculus / formulas** (row 8). Choose symbolic derivative,
antiderivative, numerical slope, numerical signed integral, absolute area or
symbolic definite integral.
Enter a formula in X with the same editor.

Symbolic results use DB48X's existing derivative/primitive algorithms. View the
result, then optionally copy the exact expression to the stack or plot it as Y1
in the native view. Antiderivatives show a particular primitive; the arbitrary
constant +C is implicit. Unsupported primitives report the upstream error; this
is not a promise of integration for every expression. Calculator angle mode and
upstream symbolic rules apply; Radians is the conventional choice for calculus.
Symbolic definite integration evaluates a supported primitive at the entered
bounds using DB48X arithmetic, so X^2 from 0 to 1 can return exact 1/3. It reports
an error when no primitive is supported; use numerical integration in that case.
Numerical results are approximate, and copying them to the stack is explicit.

Try X^2: its derivative is 2*X, a primitive is X^3/3, the slope at X=2 is 4, and
the signed integral from 0 to 1 is approximately 1/3. The editor's F2 transforms
make it possible to edit X^2, differentiate it, preview it and plot the result.

### Ready-made test plots

Load **Graphs -> Graph scenes -> ANALYSIS.scene**, then Plot 2D. Y1 is X^2-4 and
Y2 is X. Expected checks:

- Y1 roots on -3..3: -2 and 2.
- Intersections with Y2: approximately -1.561552813 and 2.561552813.
- Y1 minimum on -3..3: X=0, Y=-4. Maximum is 5 at an endpoint.
- Y1 signed integral on 0..1: approximately -3.666666667; absolute area: 3.666666667.

**Ctrl+P -> Lua scripts -> ANALYSE.lua -> F1** opens the same examples through
Lua. Use F2 to test the same analysis tools on those callbacks.

### Numerical limits

Analysis is bounded to 8192 function evaluations and checks Esc between samples.
Roots use a 257-point scan and residual-checked bracketing; poles/jumps are not
accepted just because the bracket shrinks. Tangent roots require a resolved zero
and can be missed, as can narrow or closely spaced features. Coincident/zero
curves do not produce a finite list of isolated roots. Only the first 16 resolved
points are returned. Extrema are sampled then locally refined, not a proof of
the global extremum for arbitrary oscillatory/discontinuous functions.

Integration uses adaptive Simpson refinement, a minimum subdivision depth and
an estimated tolerance of 1e-8 times max(1, the initial integral magnitude), up
to 24 refinement levels. It requires finite samples including endpoints: improper
integrals, poles and difficult discontinuities can fail rather than return a
misleading result. Finite sampling cannot guarantee detection of every narrow
feature. Slopes use shrinking centred/Richardson differences with a one-sided
consistency check to reject obvious cusps. Numerical errors are estimates, not
certified bounds. Use pure functions because sampling/refinement can call them
repeatedly. Lua callback errors/interruption return after native cleanup.


## Interactive graph exploration (port 9)

The user confirmed port-8 graph analysis, formula entry and calculus work on the
PicoCalc. Port 9 adds exploration to the same native view for calculator formulas
and Lua plot.show() callbacks. New port-9 controls still require device testing.

- Left/Right moves the crosshair one sampled column; Up/Down selects a curve.
  Live coordinates are shown below the plot, with a horizontal guide when visible.
- F3 opens Explore: toggle the live tangent, mark a bound, clear bounds, or go to
  a visible X. Go to X snaps to the nearest of the 300 sampled plot columns;
  displayed X is the actual snapped position.
- Show live tangent draws the local numerical tangent in pink and displays dY/dX.
  It updates as the cursor or selected curve changes. Undefined samples and
  nonconvergent slopes display Tangent unavailable. Esc interrupts sampling.
- Enter marks A, then B. A third press starts a new pair. Green vertical lines
  mark the bounds. F2 analysis offers Use marked bounds or Type bounds when a
  distinct pair is present. Bounds retain marking order for signed integrals;
  they persist across pan/zoom and curve changes until cleared or the view closes.
- F2 -> Area between curves selects another active curve and integrates
  abs(Yselected-Yother). The result is nonnegative even with reversed bounds or
  crossing curves; shading fills between the two curves. The same bounded
  numerical accuracy/domain limits as port 8 apply.
- F1 table, F4 pan/trace, +/- zoom and F5/Esc return remain available. Tangent,
  crosshair, bounds and shading are temporary view state, not saved in scenes.

Test ANALYSIS.scene: Y1=X^2-4, Y2=X. Area between them on 0..1 is 25/6,
approximately 4.166666667. Use typed bounds for exact endpoints; visual bounds
are limited to plot-column resolution. Show the tangent to Y1 near X=2:
the displayed slope should be approximately twice the displayed X.

### Interactive stack menu

With no calculator entry pending, Up opens the native interactive stack view.
Up/Down selects a stack item, Enter or Esc exits. In the unshifted plane F1
inserts the selected object into the input line, F2 shows it enlarged, F3
executes/evaluates it, and Backspace deletes it. Other soft keys rearrange items;
read their displayed labels before using them. F1-F5 select the first five
soft-menu columns, Ctrl+F5 selects column six, and Tab cycles calculator shift
planes. Shift planes change these actions. Full-screen PicoCalc menus instead
use Up/Down and Enter, number shortcuts, or F1-F5 for rows 1-5; Esc returns.


Port 9 keeps the Tab-selected interactive stack menu line active across
operations. For example, select Roll↑ and press F4 repeatedly: every press rolls
up until Tab changes the active line. Up/Down moves one stack level at a time
without changing that line. Enter/Esc exits and resets the plane; starting a
new stack interaction uses the unshifted line. Leaving through Edit also resets
the plane. Calculator shifts outside interactive stack mode remain one-shot.


## Math workbench (port 10)

Open **Ctrl+W**, or Ctrl+T -> Math workbench. The title shows the calculator
angle mode. Choose Radians in Setup for the COS(X)=X example. Graphs, Lua plots,
port-9 exploration and the sticky interactive stack menu remain available.

Native menus now display at most eight rows per page. Up/Down selects across
pages; Left/Right changes page. F1-F5 or number keys 1-8 selects a visible row.
On the second Setup page, F1 opens Math workbench, F2 Data, F3 Forms, F4 Notebooks and F5 Back.

### Single equation

Choose Edit equation and enter lhs=rhs, such as COS(X)=X, or a plain expression
interpreted as zero. Formulas retain the 159-byte limit and the existing F1
functions, F2 templates/calculus, F3 syntax check and F4 typeset preview.
The guided unknown is X. Other symbols may read stored calculator variables.

- Initial guess -> Solve near guess uses upstream DB48X numerical solving.
  In radians, COS(X)=X from 0.5 should give approximately 0.7390851332.
- Search roots on interval uses native bounded root analysis. For X^2=4 on
  -3..3, expect -2 and 2. Narrow or tangent roots may be missed; this does not
  promise every solution. Up to 16 roots and 8192 evaluations are supported.
- Symbolic isolate X is a separate action. Try 2*X+3=7 for an exact result of 2.
  Unsupported isolation reports an error, without a numerical fallback. Copy the
  exact equation to the stack, or explicitly store its right-hand expression as X.
- Plot equation residual draws lhs-rhs in the unified native view. Its zeros
  correspond to solutions; F2 analysis and F3 exploration remain available.

Numerical results display a scaled residual and offer explicit copying to the
stack or storing X. No result is automatically stored. Ctrl+S persists explicitly
stored variables and copied stack values with the calculator session.

### Equation systems

Choose two unknowns (X,Y) or three (X,Y,Z), edit the matching number of equations,
and enter guesses. This calls the upstream simultaneous solver, including its
Jacobian method where needed. It finds a local real solution; guesses matter.

Try X+Y=3 and X-Y=1: X=2, Y=1. The bundled SYSTEM3.workbench has X+Y+Z=6,
X-Y=0 and Z=2: X=Y=Z=2. Numerical results show a scaled residual for each
equation. Copy or store one selected result explicitly. Inconsistent, singular,
non-real, undefined or nonconvergent cases report failure rather than an
unverified answer. Symbolic system solving is not part of this guided interface.

### Parameter plots

Parameter plot starts with A*SIN(B*X+C), A=1, B=1, C=0. Configure each parameter's
minimum, maximum, positive step and current value. Plot window edits this
workbench's bounds independently of ordinary Graph scenes.

Open Plot with parameters, then **F3 -> Parameters**:

- Up/Down chooses A, B or C.
- Left/Right changes its value by the configured step and redraws the plot.
- Enter edits the current value numerically; values must be within the bounds.
- Esc returns to ordinary graph tracing. Esc during sampling interrupts the plot.

The slider footer shows the current parameter, value and step. Values clamp at
the configured limits. Tangents and analysis reuse the same evaluator. Parameter
changes clear previous analysis overlays. Use pure formulas/functions, since
sampling may evaluate them many times. Parameter values are workbench-local;
Store a parameter explicitly copies A, B or C into the current calculator
directory. Slider changes do not overwrite your existing calculator variables.

### Save, load and examples

Saved workbenches -> Save stores a named .workbench under /db48x/workbenches/.
It includes the single equation, system equations and guesses, interval bounds,
parameter definitions, angle mode and native graph scene. Results and transient
trace/tangent overlays are not saved. Ctrl+S saves the calculator session;
workbenches are saved separately. Nothing automatically reloads after restart.

Load asks before applying the saved angle mode. Invalid files leave the current
workbench intact. Loading can recover a valid .bak if the primary file is missing
or corrupt. Replacement saves retain the previous file as .bak. Without SD,
solving and plotting work, while saving/loading reports unavailable.

Included examples: SOLVE.workbench (COS(X)=X), SYSTEM3.workbench (three-variable
system), WAVES.workbench (A*SIN(B*X+C)); all use radians when loaded.

Numerical solving temporarily uses 16-digit precision, a total allowance of 200 upstream iterations
and one shuffle allowance per solve. Systems divide the iteration allowance
among their unknowns (100 each for two, 66 each for three). Interval search
reserves 32 of its 8192 evaluations for final residual checks. Returned numerical solutions must
pass a scaled residual check of 1e-8. This is a numerical check, not a proof of
uniqueness or completeness. Solver settings, variable bindings, stack and
cancellation state are restored after calculations. A temporary child directory
holds trial values and is removed on exit; no application flash writes occur.

Port 10 builds and host tests are documented in reports/VALIDATION.md. New
workbench controls, performance and a 30-minute session need physical testing.

## Release sequence

Port 11: data tables, CSV import/export, scatter plots, regression and statistics.
Port 12: reusable formula forms with named inputs and saved values.
Port 13: calculation notebooks with expressions, results, notes and explicit
rerun controls. These stages are now included together in the cumulative port-13 build.


## Data tables and statistics (port 11)

Ctrl+D, or Setup -> Data / statistics, opens a bounded X,Y table. It holds up to
128 finite real pairs, independent of calculator stack and statistics variables.
The six-row demo is Y=2X+1. Model memory is allocated from the bounded heap only
when first used. Unsaved changes remain in RAM while using other tools, but do
not survive restart; use CSV files to save them.

Edit table: arrows select row/column; Enter edits a value (calculator expressions
such as 1/3 are accepted). F1 adds a pair, F2 confirms deleting a row, F3 shows
statistics, F4 plots, F5/Esc returns. Summary includes min/max/mean/median/sum,
population variance and sample variance/standard deviation for each column;
sample measures are unavailable below two rows.

Regression supports linear, logarithmic, exponential and power fits using
upstream DB48X statistics. Results show coefficients a,b and correlation/R²
(transformed coordinates for nonlinear fits), with explicit coefficient copying.
Logarithmic/power fits need positive X; exponential/power fits need positive Y.
Constant X, insufficient data, undefined or out-of-range results report failure.

Scatter + fitted curve uses the unified native plotting engine. Orange crosses
show observations, the blue curve shows the fit, and F2 analysis/F3 exploration
retain their existing controls. If a fit is unavailable, the scatter alone is
shown and the function trace is undefined. Residual scatter plots Y-observed
minus Y-fitted, against the zero baseline. Plot bounds are fitted to the data.

CSV files are stored under /db48x/datasets/. Save/export writes X,Y plus decimal
pairs at double round-trip precision. Import accepts optional X,Y or x,y header,
LF/CRLF, UTF-8 BOM, quoted numeric cells and scientific notation. It requires
exactly two finite numeric fields per nonblank row, at most 128 rows and 16 KiB;
malformed/ragged/nonfinite input leaves the current table intact. There is no
Excel workbook or arbitrary-column import. Previous saves are retained as .bak,
and loading tries the backup if primary data is invalid or missing. File names
are local to the calculator; root SD content and other apps are not modified.
LINE.csv gives slope 2/intercept 1; POWER.csv gives a=3,b=2 with the power model.

Port 11 also fixes the SD multi-sector read defect behind port-10 workbench load
failures: reads now validate each 512-byte sector and leave the card out of
streaming mode. Workbench load errors include path, size and transport/decoder
status, and hidden companion files are excluded. See reports/SD-WORKBENCH-FIX.md.
Host checks pass; the new controls, transport fix and plotting performance still
require device confirmation.


## Reusable formula forms (port 12)

Ctrl+F, or Setup -> Formula forms, opens a named calculation form. A form has a
title, one expression (159 UTF-8 bytes) and up to six named real inputs. Variable
names use letters/digits/underscore, begin with a letter, and are at most 12
characters. Duplicate names (case-insensitive) and reserved calculator commands
or constants are rejected. Input expressions are at most 63 bytes; later inputs
may refer to earlier ones. Other symbols may read existing calculator variables.

Edit the title/formula/inputs, then choose Calculate explicitly. User precision,
angle mode and arithmetic flags apply; exact fractions and complex outputs are
preserved where the calculator mode permits them. Inputs must be finite real
scalars within +/-1e100. Names are scoped: calculations leave the original
calculator variables and stack intact. The result viewer offers an explicit copy
to the stack; nothing calculates automatically when you edit or load a form.

Inputs menu: select a field to change its expression, rename or delete it; Add
creates another field. Cancelled edits keep the original value. A star marks
unsaved changes. Form files under /db48x/forms/ use a bounded version/checksum,
replace with .bak recovery and load only after validation. Current input values
are part of the form file; results are not saved. Ctrl+S continues to save the
calculator session separately. Without SD, forms remain usable in RAM.

Load OHM.form, then Calculate: VOLTAGE=12 divided by RESISTANCE=4 gives 3.
CIRCLE.form computes PI*RADIUS^2 with RADIUS=3. Use the Examples menu for the same
presets; replacing unsaved changes asks first. Physical form-button/save/reload
verification is still pending, alongside the port-11 data tools.


## Calculation notebooks (port 13)

Port 13 includes ports 11 and 12 and the SD transport fix in one calculator
firmware. Ctrl+N, or Setup -> Calculation notebooks, opens a bounded notebook
with up to 12 algebraic expressions and notes. The model is allocated lazily on
the bounded heap, independently of the calculator stack and HOME variables.
This is a sequential calculation notebook, not a spreadsheet or a new language;
RPL and Lua remain the programming tools.

Cells / expressions / notes lets you choose a cell, edit its expression or note,
inspect its cached preview, run through it, move it up, or delete it. Add cell
appends a new expression. Expressions are limited to 159 UTF-8 bytes, notes to
95, and the notebook title to 47. Editing/moving/deleting an expression invalidates
that cell and later cached results. Notes do not trigger calculation. A star
marks unsaved source or note changes.

Calculation is explicit: Run all cells or Run through this cell starts at cell
one and evaluates in order. Completed results become R1..R12 for later cells
within that run. Earlier-cell dependencies are recomputed, not taken from cached
previews. Self/future result references are rejected; an error or Esc stops at
the failing cell and later cells remain unrun. Other symbols can read calculator
variables, but notebook Rn bindings do not overwrite existing variables. Use pure
algebraic functions; repeated explicit runs can call those functions again.

Calculator precision, angle mode, exact/complex flags and function settings
apply. The last full result can be viewed and explicitly copied to the stack.
Cached previews are bounded and may be shortened; they are never used as inputs
and are not saved. Running/editing does not automatically change the stack.

Files: save/load stores versioned checksummed .notebook files under
/db48x/notebooks/. Saved files contain sources and notes, not results. Backups,
validation-before-replacement, unsaved-change confirmation and missing-card
behavior follow the other native tools. Ctrl+S saves the calculator session
separately. A restart requires explicitly loading the notebook and running it.

Load CHAIN.notebook -> Run all: 2+3 gives R1=5, R1^2 gives R2=25, R2/3 gives
R3=25/3 in exact mode. FRACTIONS.notebook gives 1/2, 1/4 and 4 in exact mode.
Try changing the first expression, then Run through cell 2: its earlier
inputs are recalculated and cell 3 remains unrun. Set cell 2 to 1/0 to check the
error stop, then repair it and rerun. Physical-device controls, save/reload and a
30-minute mixed-use session remain pending; builds alone are not a device test.

All three later ports are archived locally for reproducibility. The final SD
bundle contains only the cumulative port-13 firmware and its example resources;
no separate intermediate installation or testing is required.


## Connected graphs, parametric and polar plots (port 14)

### Forms to graphs

Ctrl+F -> Graph an input sweep chooses one named input for the horizontal axis.
Enter input (X) and result (Y) bounds, then the existing native view plots the
form result. Input expressions are evaluated in their displayed order; inputs
that depend on the swept input update with it. The original input strings and
calculator variables remain unchanged. Other calculator variables remain live.
Plots use scoped 16-digit numeric evaluation and pure functions. Complex or
undefined results are gaps. F1 table, F2 analysis, F3 exploration/tangent, F4 pan,
zoom and cancellation work as for ordinary function plots. Here X means the
chosen input, named in the curve label. Sweep bounds are temporary; save the
original form separately.

Try OHM.form: choose RESISTANCE, X=1..10, Y=0..15. With VOLTAGE=12 the curve
passes through (2,6) and (4,3). The saved RESISTANCE input remains 4.

### Data to the calculator and graph scenes

Ctrl+D -> Summary statistics now offers Copy statistic to stack after displaying
the summary. Select X or Y and the metric; sample variance/deviation require
at least two rows. Nothing is copied automatically.

Regression model / result -> Use fitted curve offers coefficients, the fitted
expression, correlation and R squared for copying. Unavailable correlation is
reported instead of copying zero. Use as Y1 replaces the active native 2D
curves; open Ctrl+G -> Plot 2D. Save curve as graph scene writes a normal .scene
under /db48x/graphs without replacing your active plot. Load it through Graph
scenes. Coefficients are frozen into the expression, independent of subsequent
CSV/table changes. The scene contains the fitted function and window, not the
raw data points. All four fit models are supported. Copied expressions are
calculator expression objects; copying does not evaluate stored X.

Try LINE.csv -> Linear regression: copy the expression, or use/save it in Graphs.
It is equivalent to 1+2*X. Graph roots, slopes and integrals then use the normal
native function analysis engine. Ctrl+S persists copied calculator stack values;
graph scenes and CSV files have their own explicit saves.

### Parametric and polar paths

Ctrl+G -> Parametric / polar (second page, F1) opens path tools. Choose:

- Parametric: enter X(T) and Y(T), without X= or Y=.
- Polar: enter r(T). T is the angle, in the current calculator angle mode.
  Negative radii are valid and reflect the point through the origin.

The editor inserts T through F1 and uses T for its F2 derivative/primitive
transforms. Trigonometric symbolic rules remain those of upstream DB48X;
radians are the conventional calculus setting. Numeric path sampling always
honors the current angle mode. A new path defaults to one full turn of T.
Examples ask to use radians and replace the current path:

- CIRCLE: X=COS(T), Y=SIN(T), T=0..2*PI.
- ELLIPSE: X=2*COS(T), Y=SIN(T), T=0..2*PI.
- ROSE: r=2*COS(3*T), T=0..2*PI.
- SPIRAL: r=T/(2*PI), T=0..4*PI.

Both modes use the same Cartesian grid, clipping, colours and native renderer
as function/data/form plots. There is one active path, sampled at 300 T values;
no extra framebuffer is allocated. This is bounded sampling, not a continuity
or completeness guarantee. Undefined/non-real points become gaps; narrow
features, loops or discontinuities between samples can be missed.

- Left/Right traces T, with live T,X,Y readouts and a crosshair at the point.
- F1 shows a T,X,Y table. F2 edits the T interval and resamples.
- F3 toggles a numerical tangent, goes to a sampled T, or gives equal axis units.
  Vertical tangents are drawn when resolved; tiny/stationary or cusp derivatives
  may be unavailable. Go to T snaps to the nearest of 300 sampled values.
- F4 toggles pan/trace; +/- zooms. Pan/zoom reuses the path samples.
- F5/Esc returns; Esc during sampling interrupts.

Cartesian roots/intersections/areas and visual X-bound selection are not applied
to paths: a loop may have several Y values at the same X. Enter explains that
F2 edits parameter bounds. Existing Cartesian analysis is unchanged.

Save/load paths uses versioned, checksummed .path files under /db48x/paths.
Loading asks before applying its angle mode. Files contain all three formula
fields, mode, T interval and viewing window; .scene/.workbench/.form formats
are unchanged. Backup recovery and replacement-save protection follow the other
tools. Without SD, plotting works in RAM. T and calculator settings/variables
are restored after the view closes. Loading/editing a path does not modify EQ,
ordinary graph scenes, forms, datasets or the calculator stack.

Port 14 host/build verification is recorded in reports/VALIDATION.md. Physical
verification of the new controls, rendering and performance is still pending.

## Differential equations, units and graph export (port 15)

**Ctrl+G → Differential equations** solves a real first-order initial-value
problem `dy/dX=f(X,Y)`. Enter the slope expression (for example `Y`), set X0/Y0
and a view window, then Plot solution / direction field. Examples and checked
`.ode` saves are available under `/db48x/odes/`.

The native graph displays a direction field and an approximate solution through
the initial condition. Left/Right traces; F1 shows values; F2 reports numerical
work and partial solutions; F3 → Explore / controls offers tangents, initial
conditions and field/solution visibility; F4 pans; +/- zooms. Esc interrupts.
Adaptive RK4 step doubling uses a local tolerance of 1e-7 and an 8192-evaluation
solution budget, plus 315 field evaluations. Sampling has 300 points. Stiff
problems, singularities and finite budgets can stop integration. This release
supports scalar first-order ODEs, not coupled or higher-order systems. X/Y and
calculator settings are scoped and restored.

**Ctrl+F → Inputs / values → select an input → Input units** attaches an optional
unit declaration, such as `V`, `ohm`, `m` or `m/s`. A plain number acquires that
unit; an input quantity such as `100_cm` is converted to the declared unit.
**Result units** optionally requests a compatible output unit. Incompatible
dimensions produce a calculator error. The new UNITOHM example returns `3_A`
from 12 V / 4 ohm. Existing version-1 unitless forms load unchanged; new saves
use a checksummed version-2 format. Forms preserve exact arithmetic and use
upstream DB48X unit conversions. Unit-aware sweep axes show magnitudes in the
input and result units identified before plotting; sweep bounds use those units.

**In a native 2D graph, F3 → Export graph SVG / Export samples CSV** writes to
`/db48x/exports/`. SVG shares the renderer's clipped line geometry, including
curves, grid, tangent, shaded bounds, scatter markers and direction fields.
SVG text uses a standard monospace font; it is a vector graph, not a framebuffer
screenshot. CSV records the existing samples without rerunning the expression:
Cartesian X/Y1/Y2/Y3, parametric/polar T/X/Y, or ODE X/Y. Undefined samples are
empty cells. Scatter points are appended in a separately labeled block.
**In a 3D surface, F2 exports the current projected wireframe as SVG**. Enter an
export name using letters, digits, underscore or dash; replacement asks first.
Writes use a temporary file and backup replacement, keeping the previous valid
file if writing fails or is cancelled. Calculation works without SD; exports
and saves clearly report unavailable media. No application flash writes occur.

Port 15 includes port 14's connected form sweeps, data graph scenes, notebook
plots, parametric/polar graphs and equal-axis controls. New controls and SD
round trips still need physical testing; firmware build verification is recorded
separately in `reports/VALIDATION.md`. Deployment is a separate action.


## Screen readability pass (port 15.1)

Native screens now fit long text to measured font widths, switching to the
13-pixel compact font where necessary and marking elided labels with `...`.
Graph formula, values, status and controls occupy separate rows; neither
parameter sliders nor tangent/bound hints overwrite the footer. The plot
viewport remains 300×210. Three-dimensional bounds and controls use compact
text. Menu pagination hints, data-table hints, formula feedback, source-editor
status/footer and the About screen have bounded text regions. Long notices
and confirmation questions are available in the scrolling text viewer.
Calculator expression/result fonts are unchanged.

Representative previews use the actual DB48X fonts and current colour drawing code;
font metrics, UTF-8 elision, row geometry and raster clipping are tested.
Physical readability and the full device walkthrough remain pending. Nothing
has been copied to the SD card as part of this review.


## Reliability and memory release (port 16)

Port 16 addresses the production review's four findings: input-sized scratch
arrays no longer consume the C stack, failed saves preserve validated recovery
files, SD initialization has an elapsed deadline, and Lua runs compile directly
from SD after releasing editor buffers. The no-exception allocation cleanup
also removes unused exception/demangler support.

Lua **Run** saves first, temporarily releases editor memory, then reopens the
saved source afterwards. The cursor is retained and undo history starts fresh.
If SD disappears while loading, compilation fails without executing a partial
script. The Lua console still works without SD. A .rollback file is an
interrupted-save recovery generation; loaders validate it before .bak.
Keep recovery files when recovering a card. Existing document/session formats
remain compatible. The current-only bundle includes all port 14–15 features.

Build, sanitizer, Arm allocation and simulator comparison evidence is provided
in reports/PORT16-REMEDIATION.md and reports/VALIDATION.md. Hardware cold boot,
controls, save/reload, loader return and mixed-use endurance remain pending.


## Conversions and Wi-Fi (port 17)

Press **Ctrl+U**, or **Ctrl+T → Conversions**. Choose **Unit conversion**,
enter the value, select From and To, then Convert. The categories cover length,
mass, absolute temperature, area, volume, speed, time, pressure, energy, power
and angles. Custom units accept expressions such as `kg*m/s^2`. Fractions remain
exact where the upstream engine supports them. Celsius/Fahrenheit offsets and
US liquid versus imperial gallons use DB48X's existing definitions. Results
are previews until **Copy to stack**. Use pure expressions; programs with side
effects are unsupported.

For currency, choose **Cached currency rates**. The supplied reference table
loads when no saved cache is available. Select From/To and Convert. Every result
shows the observation dates, provider and cached status; fees and spreads are
excluded. These are daily reference observations, not intraday quotations.

For on-device updates:

1. Open **Wi-Fi setup** from Conversions or Setup. Enter the 2.4 GHz SSID and
   WPA2-AES personal password. Open networks require the explicit open-network
   action; WPA3-only and enterprise authentication are unsupported.
2. Optionally choose **Save credentials on SD**. Credentials remain readable
   in `/db48x/network/WIFI.cfg` and its recovery copies. Without saving they
   last only while this menu session remains open. **Forget** removes all copies.
3. Open **Cached currency rates → Refresh over Wi-Fi / HTTPS** (second menu
   page; Right moves pages). It synchronizes UTC from NTP automatically before
   checking TLS and downloading rates. The displayed clock is updated in your
   selected timezone, defaulting to Toronto. For manual UTC or historical rates,
   open **Update options / date**. Supported current UTC input: 2020–2037.
4. Wait for the validated table. **Esc cancels**. The radio is turned off
   after success, failure or cancellation. A successful refresh updates RAM
   and attempts an atomic SD-cache save; a failed save retains the previous
   valid SD cache and reports that the new rates are available only in RAM.

HTTPS uses Mbed TLS 3.6.7, TLS 1.2 ECDHE/ECDSA, the GTS Root R4 trust anchor,
provider-hostname verification and certificate-date checks. Standard NTP supplies
the date/time by default in port 17.3; it is not authenticated. TLS verification
is never disabled. Changes to the provider's
certificate chain can require a future firmware update. Wi-Fi is on demand;
there is no background networking. With occupied heap space, a connection can
fail gracefully for insufficient memory; calculator memory reservations stay fixed.

Manual dated rates work without SD or Wi-Fi. **Import SD rate table** accepts
checksummed `.rates` files under `/db48x/currency/`; **Save current cache** makes
an imported/manual table persistent with recovery copies. The computer updater
is included in that directory and in the source archive:

```sh
python3 tools/update_currency_rates.py --output LATEST.rates
```

Copy the output into `/db48x/currency/`, import it and save the current cache.
The script uses the computer's trusted HTTPS certificate store. It never chooses
or deploys to a mounted SD card automatically. [Rate provider](https://frankfurter.dev/).

Build/host/Arm-emulation checks are documented in `reports/PORT17.md`.
Physical PicoCalc Wi-Fi, controls, persistence, loader return and mixed-use
acceptance remain pending. The release has not been copied to your SD card.


### Radio status (port 17.1)

**Wi-Fi setup → Radio / connection status** shows just the current radio and
connection state. F1 refreshes the display; Esc returns. Opening this view does
not turn the radio on or attempt a connection. With on-demand networking idle,
it correctly shows Radio: Off and Connection: Disconnected. Instructions are
available separately under **Wi-Fi setup → Help**.


### Wi-Fi join retries and diagnostics (port 17.2)

A temporary driver `NONET` (no matching SSID found yet) now retries asynchronous
joining, following the Pico SDK's connection helper. Retry requests are limited
to once per second within the existing 35-second join budget. Esc still cancels;
authentication rejection and a terminal driver failure remain immediate errors.
Waiting for an IP address does not count as a failure until the join budget expires.

The result screen separates network discovery timeout, authentication rejection,
association failure and DHCP timeout. It also shows raw Wi-Fi status, elapsed
join time and join-request count, without exposing passwords. Status 3 means
connected with an IP address, -2 means network not found, -3 means authentication
failure, and 2 means associated but no IP address. Old cached rates are retained
on failure, and the radio turns off on every return path.


## Automatic time, timezone and historical rates (port 17.3)

**Refresh over Wi-Fi / HTTPS** now performs Wi-Fi join → NTP synchronization →
verified HTTPS download, without asking you to type the current date/time.
The display clock adopts the synchronized time when the operation returns,
including when time sync succeeds but the later rate download fails. The radio
still turns off on every return. Nothing downloads automatically at cold boot.

Choose **Cached currency rates → Update options / date** for:

- **Clock:** Automatic NTP (default), or a manual **actual current UTC** override
  if your network blocks UDP/123. A manual override advances while this menu
  session remains open. A failed NTP sync stops the download; it never silently
  uses the unset boot clock or disables certificate checks.
- **Rates date:** latest available observations, or a historical `YYYY-MM-DD`.
  This selects the rate table; it never changes the clock or backdates TLS.
  Returned observation dates may precede the requested date on holidays or when
  a provider lacks newer data. Future requests and tables newer than the chosen
  date are rejected; incomplete/unavailable tables keep the old cache.
- **Timezone:** Toronto / Eastern by default, including its DST transitions.
  Other named presets and custom signed `+HH:MM` / `-HH:MM` fixed offsets are
  available. Named rules are bundled projections, not a full global IANA database;
  custom fixed offsets do not automatically change for DST. Vancouver accounts
  for B.C.'s 2026 permanent UTC−7 change.

Timezone is saved separately under `/db48x/network/TIMEZONE.cfg` with checksum,
atomic replacement and recovery copies. Without SD, it lasts for the current
boot. **Wi-Fi setup → Clock timezone** opens the same setting. The clock itself
has no battery-backed persistence: it starts unset after reboot until a sync or
manual clock setting. Choosing a timezone immediately changes an already
synchronized display clock; a legacy manually-set local clock stays local until
it is synchronized again.

NTP queries `time.cloudflare.com`, falling back to `time.google.com`. Replies
are checked for endpoint, matching random originate cookie, packet size,
mode/version, leap/stratum validity and supported timestamp range, including the
2036 era rollover. Each server gets a bounded query; Esc cancels. Standard NTP
is **not authenticated**: these checks reject malformed/unrelated replies but
cannot defeat an observer who can alter the time exchange. TLS still checks its
CA chain, hostname and certificate dates against the chosen clock.

The computer updater also accepts `--date 2024-01-02` for a historical table.
Successful historical downloads replace the active cached table just like a
latest-rate download; its actual dates remain visible. Update options are
transient; downloaded tables and saved timezone settings persist on SD.

See `reports/PORT173.md` for build, protocol, timezone and memory evidence.
Physical PicoCalc acceptance remains pending; no port-17.3 SD deployment is claimed.
