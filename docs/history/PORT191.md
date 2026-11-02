# LumaRPL port 19.1 — fixes and colour sprites

Port 19 controls remain documented in [PORT19.md](PORT19.md).
This release keeps the 208 KiB calculator arena, 96 KiB heap, 32 KiB stack,
100 KiB RGB332 framebuffer and 16 KiB workspace. Deployment is separate.

## Programs and calculator menus

HOME programs F2, SD RPL Run and source-editor F2 Save & run execute the selected
program body, then return through Programming and Home to show the calculator
stack. A nested quoted program stays a program: Run does not repeatedly evaluate
objects returned by your program. Errors report the failing operation; source
remains recoverable when saving or execution fails. Esc interrupts execution.

Tab chooses an RPL soft-menu row. F1–F5 use that row; Ctrl+F5 selects its sixth
entry. The row stays selected after an operation while the same menu remains
open. Opening a different menu uses its initial row. This applies to settings,
the command catalog and interactive stack menus. Native action bars still have
five labelled slots, with F5 Back/Cancel. Esc closes a selected RPL menu
without invoking a shifted shortcut; a further Esc cancels an active entry.

The compact font now retains complete negative-bearing glyph bitmaps, including
the right stroke of 5. Source editor and configurable mathematical fonts remain.

## Currency rates

Refresh validates the downloaded table, makes it immediately available and saves
it transactionally. Convert immediately; no restart is needed. Currency choices
are preserved by code when present in the new table. Reopening Conversions keeps
the active table, even if SD saving failed. A failed download keeps previous rates.
A failed save keeps new session rates and the previous disk cache; save again to
retain them across restart. Rate observation dates remain visible.

## Colour sprite editor

Home > Programming > Sprite editor edits 16×16 RGB332 images with transparency.

- F1 Save writes a `.sprite` document under `/db48x/sprites/`.
- F2 Colour opens all 256 colours. Arrows choose; Enter/F1 selects paint;
  F3 selects erase; F5 cancels.
- Arrows move the pixel cursor. Space/Enter paints; Backspace erases.
- F4 Tools opens saved sprites, starts a new drawing, changes mode, clears,
  exports a CHIP-8 mask or opens Help.
- F5 Back offers Save, Discard and Cancel for an unsaved drawing. Cancel is the
  default. Dirty sprites are also listed before returning to the loader.

CHIP-8 mode uses a one-bit mask and a preview colour. Switching a populated colour
sprite to CHIP-8 confirms replacing pixel colours. Save stores a colour document;
Open restores it in colour mode. Export CHIP-8 mask writes assembly under
`/db48x/chip8/`: opaque pixels are set, transparent pixels clear. Colour export
asks first; colours are not part of CHIP-8 assembly. Export is separate from Save.

Lua draws sprites with `screen.sprite("DEMO.sprite", x, y)`. Integer coordinates
clip at screen edges; transparent pixels preserve the canvas. It returns `true`
or `nil, error`. Supply a plain `.sprite` filename, without directory components.
Call `screen.show()` to display changes. The bundled `SPRITE.lua` demonstrates
colour, transparency and edge clipping. Existing CHIP-8 ROM pixels are unchanged.
The 304-byte versioned format and CRC32 are documented in `/db48x/sprites/README.txt`.

## Guided device acceptance — pending

1. Cold boot; confirm port 19.1 in About. Open Programming > HOME programs >
   Equation, press F2, and check its expected result on the stack. Repeat with an
   SD RPL program and editor Save & run. Test `« 2 3 + »` for 5; test a program
   needing stack input, a runtime error, and Esc during a long loop.
2. Open Settings > Advanced RPL settings. Tab to the Beep row; press F3 twice.
   Beep must toggle each time and the row must stay selected. Test Ctrl+F5 for
   the sixth entry. In the command catalog, Tab to the row containing `<`,
   press its F-key, and confirm the correct command and retained row.
3. Inspect `0123456789`, `55555` and `(5) 5.5` in small status/help text and Lua
   output. Check all themes and that the sixth-key hint does not overlap the stack.
4. Refresh currency rates, convert immediately, leave and reopen Conversions,
   convert again, then restart and confirm the saved rates. Test offline cached use.
5. Paint multiple sprite colours and transparency; save as TEST, reopen it,
   and run `assert(screen.sprite("TEST.sprite", 30, 40)); screen.show()` in Lua.
   Run SPRITE.lua. Check Cancel on unsaved work, reopen after restart, CHIP-8 mask
   export/build/run, and a missing or invalid sprite error.
6. Check editor save/recovery, graph exploration/export/interruption and loader
   return. Complete 30 minutes of mixed use without a crash.

Host tests and screenshots cannot establish LCD readability, electrical key-repeat
behaviour, live network reliability or physical-device acceptance.
