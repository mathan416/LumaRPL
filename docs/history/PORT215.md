# LumaRPL port 21.5 — compact source editors

RPL source files, Lua scripts and CHIP-8 assembly now share a smaller fixed-cell code font. The source view shows 40 columns and 16 visual rows, up from 28 columns and 10 rows. Character cells are 7x12 pixels with 14-pixel line spacing. Titles, menus, the action bar and calculator stack fonts retain their existing sizes.

Syntax colours, line numbers, selection, UTF-8 cursor movement, wrapping and scrolling use the new geometry. Both cursor paths (normal movement and movement while selecting/scrolling) use the same column count. Cursor-only movement still restores the old caret, draws the new caret and updates Ln/Col; the tools hint and action bar stay in place. No-op cursor keys do not refresh the display.

The generated font has 932 glyphs, including the RPL quotation and mathematical characters already supported by the compact UI. It uses Pixel Operator Mono with resized FogSans mathematical supplements; corresponding font notices and full license texts are included. It lives in flash and adds no framebuffer or static SRAM allocation. Measured firmware flash use is 2,023,960 bytes; static SRAM remains 387,628 bytes. Calculator arena, bounded heap, reserved stack and workspace sizes remain unchanged. This changes the SD source editor; the formula editor and mathematical stack rendering are separate.

## Checks

Production renderer/controller tests cover RPL, Lua and assembly in all eight themes, selected text, the 40-column wrap boundary, scrolling beyond 16 rows, no-op keys and pixel-for-pixel equality of cursor-only redraw versus a full redraw. Existing source save/run and recovery workflows are exercised by the same controller suite. The UTF-8 editor model runs with AddressSanitizer and UndefinedBehaviorSanitizer. Rendering audit, generated-font freshness, memory layout and UF2-loader partition checks are included. Mathematics and saved formats are unchanged. A fresh full desktop simulator suite is not claimed.

Visual review covers the changed source editors and selections. Physical readability, responsiveness and SD behavior still need device testing; host checks do not establish those.

## Device test after deployment

1. Open an existing RPL source file. Check that more text fits while quotation marks and mathematical symbols remain readable.
2. Move through a line longer than 40 characters and through a file longer than 16 lines. Check cursor position, scrolling and Ln/Col. At the start of a file, Left/Up should do nothing visibly.
3. Mark a selection with Ctrl+K and extend it across a wrapped line. Try copy/paste and undo/redo, then cancel or save deliberately.
4. Open a Lua script and a CHIP-8 assembly source. Check syntax colours, indentation and the unchanged F1/F2/F4/F5 actions. Save/reopen and run one familiar demo of each type.
5. Check your preferred theme on the physical screen and confirm loader return after saving work.

Firmware keeps pico2-apps/LumaRPL-Pico-2-W.uf2. The release includes matching resources, source, checksums, notices and review captures. SD deployment is separate and has not been performed.
