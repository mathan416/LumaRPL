# LumaRPL port 19 — controls and acceptance guide

## Home and the calculator

Boot opens the calculator stack. Ctrl+T opens Home:

| Category | Tools |
|---|---|
| Calculator | Return to the stack |
| Math & graphs | Graphs, equation solving, calculus, formula forms |
| Data & notebooks | Data/statistics, calculation notebooks |
| Programming | HOME RPL, SD RPL, Lua scripts/console, CHIP-8/assembly, sprites, variables |
| Conversions | Units, cached currency rates, Wi-Fi setup |
| Files & session | Sessions and the typed document save/load browsers; return to loader |
| Settings | Display/theme, calculation settings, clock/timezone, network, diagnostics |
| Help & about | Reference, command catalog, shortcuts, About, credits/licenses |

Native lists use Up/Down, Enter or F1 to open the selected row. Number keys 1–8
open the corresponding visible row; Left/Right changes pages. Returning from an
action retains the list selection. F4 Details shows an elided row in full. F5 or
Esc returns to the parent. Function keys invoke their displayed actions, rather
than selecting list rows. Empty lists hide unavailable actions.

The calculator retains F1–F5 for its RPL soft menus, Ctrl+F5 for entry six and
Tab for the active stack-menu line. Its configurable mathematical fonts remain.
Finish calculator input with Enter or cancel with Esc before opening a tool
that evaluates expressions. Home and read-only settings remain accessible.

| Shortcut | Tool |
|---|---|
| Ctrl+G | Graphs |
| Ctrl+W | Equation workbench |
| Ctrl+D | Data/statistics |
| Ctrl+F | Formula forms |
| Ctrl+N | Notebooks |
| Ctrl+P | Programming |
| Ctrl+U | Conversions |
| Ctrl+S / Ctrl+L | Save / load calculator session |
| Ctrl+Q | Save session and return to loader |

## Files, source and unsaved work

Source browsers show F1 Open, F2 Run, F3 New, F4 Actions and F5 Back. Binary RPL
and CHIP-8 files open a details view with Run; edit their text source to change
them. Typed document load browsers offer Open and Details, with their save/new
workflow in the parent tool. CHIP-8 compatibility is under F4 Actions.

Editors use F1 Save, F2 Save+run (assembly builds before running), F4 Tools and
F5 Back. Tools includes Find, Replace, Go to line, Undo, Redo, Select all,
Shortcuts, File details and RPL Debug. The status line shows Ln/Col. Ordinary
cursor movement changes only caret and Ln/Col; selection, edits and scrolling
redraw the affected content, retaining static chrome. Menus, readers, prompts and results retain
their title and action bar while navigating; no-op menu/reader keys send no pixels.
Ordinary 2D tracing redraws 261 rows, and 3D rotation 223 rows, retaining static
controls. These are host/model counts, not physical latency measurements.

A leading * indicates unsaved source. Back offers F1 Save, F3 Discard and F5
Cancel; Enter defaults to Cancel. Failed saves retain the editor and previous
saved file. Save+run parks Lua source in checked PSRAM and restores the editor
after execution; syntax/runtime errors remain recoverable. Sprite editing also
marks changed drawings and offers Save/Discard/Cancel before closing. RAM-backed forms,
notebooks, workbenches and graph documents remain available when moving between
tools. Replacing unsaved work asks first.

Ctrl+Q identifies unsaved tool documents. F1 Save all opens the existing named
save dialogs; F3 Leave requires explicit confirmation; F5 Cancel stays in LumaRPL.
Cancelling a save or encountering an error prevents loader exit. Calculator
session saving is separate from saving the tool documents. Save to SD before a
cold restart; PSRAM retains nothing across power loss.

## Math, results and graphs

Graphs has one route each for 2D functions, parametric/polar, 3D surfaces,
differential equations and saved graphs. Use the 2D function list to edit Y1–Y3,
then Plot/trace curves. Formula entry offers Functions, Templates, Check and
Preview. Esc cancels without applying the unfinished formula.

2D plots show the formula, cursor coordinates and a separate status line above
the action bar: F1 Table, F2 Analyse (or mode-specific information), F3 Explore,
F4 Pan/Trace and F5 Back. Left/Right traces; +/- zooms. Enter marks Cartesian
analysis bounds. F3 contains exports and mode-specific exploration, including
tangents, parameter sliders, path controls and ODE layers. Esc interrupts sampling.
3D arrows rotate; +/- zoom; F1 Reset, F2 Export and F5 Back. The grid/mesh stays
inside the plot region, separate from the labels and action bar.

Numerical qualifications, rate dates and actual truncation notices remain
visible. Result screens show the typeset value first, context second, with
explicit Copy, Details and any applicable Store/Plot action. Viewing or cancelling
does not copy a value or store a variable. Solver and analysis methods retain
their existing numerical limits; bounded root searches do not promise all roots.

## Lua text and graphics

Printed Lua output uses compact proportional text. F3 Columns toggles compact
fixed cells for aligned output; F3 Normal restores prose spacing. A graphical
script finishes with F1 Output and F5 Back. The running screen offers F5 Cancel.

Canvas text preserves the existing pixel coordinates and `screen.text` arguments:

```lua
assert(screen.font() == "compact") -- every new session starts here
local previous = screen.font("mono")
local width, height = screen.measure("X   Y\n1   2")
screen.text(8, 40, "X   Y\n1   2", "white")
screen.font(previous)
screen.show()
```

`compact` is the 13-pixel proportional face with 16-pixel line spacing. `mono`
uses the same face in fixed compact cells. `legacy` reproduces the previous
`Settings.header_font()` metrics and line spacing, for existing coordinate-based
scripts: call `screen.font("legacy")` before drawing. The setter returns the
previous profile; the getter returns the current one. Selection persists across
console commands until that Lua session closes and does not change calculator
settings. `screen.measure(text)` returns width and height using the exact drawing
layout, including newlines and UTF-8 handling. Existing bounded string limits
apply. Empty text measures 0×0. CHIP-8 display pixels are unchanged.

## One device acceptance session

Record the firmware hash and keyboard BIOS version. Physical acceptance is
pending until these checks run on the PicoCalc:

1. Cold boot; open Home and every category. Check all three themes. Hold a number
   or Enter while changing screens: it must not activate the next screen.
2. Calculate `2 ENTER 3 +`, `1 ENTER 3 /`, a complex value and a matrix. Exercise
   all six soft-menu columns and repeated Roll Up/Down with sticky Tab selection.
3. View help, a long Details page, an error and a confirmation. Enter on a
   confirmation must cancel. Check that Back returns to the selected list row.
4. Open HELLO.lua, run it, toggle Columns/Normal. Run DRAW.lua and BALL.lua, then
   the native plot demos. Test compact/mono/legacy and `screen.measure` in console.
5. Create RECOVERY.lua containing `print("RECOVERY TEST")` and `print(42)`. Move
   the cursor, select, undo/redo and scroll. Save+run, return to the editor, save,
   reopen and restart. Reopen the same source and run it again. Introduce a Lua
   error, recover, and verify dirty-source Save/Discard/Cancel.
6. Trace/tangent/mark bounds, change parameters, view a path and an ODE field,
   rotate 3D, interrupt sampling and export SVG/CSV. Check bottom labels/action bar.
7. Refresh Wi-Fi/NTP/rates, inspect timezone and rate date. Restart and convert
   from cached rates with Wi-Fi unavailable. Check Memory/PSRAM display timings;
   host row counts are models, not physical latency measurements.
8. With dirty documents, Ctrl+Q → Cancel, then Save all. Confirm a failed/cancelled
   save keeps LumaRPL open. Return to loader, relaunch and complete 30 minutes of
   mixed calculation, editing, Lua, plotting and saving without a crash.

Report implementation, host verification, screenshot review and physical
acceptance separately. Host captures use mocked SD/device boundaries and do not
prove electrical keyboard behavior, Wi-Fi interoperability or power-loss safety.

The design follows [Nielsen's usability heuristics](https://www.nngroup.com/articles/ten-usability-heuristics/)
and targets [4.5:1 text contrast](https://www.w3.org/WAI/WCAG22/Understanding/contrast-minimum)
after RGB332 quantization. This is a firmware readability target, not a claim of
WCAG conformance for the hardware.
