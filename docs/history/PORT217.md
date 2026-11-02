# LumaRPL port 21.7 — editor assistance

All three source editors now offer **F4 Tools → Insert command**, after Language reference and Open example. Browse with arrows or numbered rows; F2/F3 and Left/Right change pages, and F4 Find searches names. Choose a command, inspect its template, then press F1 Insert. It replaces selected text or inserts at the cursor, never executes it. Ctrl+Z undoes the insertion; earlier edits stay in undo history. Back from the picker or preview changes nothing. A full source buffer or a selection larger than 256 bytes reports why insertion cannot proceed.

The bounded picker uses flash metadata: 1,026 RPL commands/settings/menu commands, 24 Lua API calls and 32 CHIP-8 instructions/directives. It also works without SD. Arguments are examples to edit, not automatic variable bindings. CHIP-8 label references such as start and routine need definitions; SUPER-CHIP instructions need the matching profile.

## Jump from errors

RPL syntax errors offer Edit at line at the engine's source position. RPL runtime errors do not include a source map: Find is offered only when the failing command has one matching occurrence outside strings/comments. It recognizes command aliases. Repeated or absent commands show no jump action; no exact runtime line is claimed.

Lua syntax and runtime errors offer Edit at line when a source position is available. Runtime locations are captured from Lua frames before unwinding; the error and line are retained separately from the output buffer. An error after a large print can therefore still be located. Cancellation and errors without a source location do not offer a jump.

CHIP-8 assembler errors offer Edit at line. Runtime errors offer Edit source, mapping the failing instruction address through the assembler back to its source line.

Back from an error leaves the cursor unchanged. A jump moves the caret without editing text or clearing undo history. Save & run still explicitly saves first; RPL syntax failures before saving leave unsaved source intact. Lua/RPL run recovery retains undo with PSRAM, with a bounded heap snapshot when PSRAM is unavailable. If a safe snapshot cannot be retained, Run is not started. Existing files, formats, calculator mathematics and static memory reservations are unchanged.

## Device checks after deployment

1. In each editor, make an unsaved edit. Insert a command, undo it, then undo your earlier edit. Repeat with a selection and with Back from the preview.
2. RPL: enter `« 7 DUP * »`, then introduce an unfinished string on another line. Save or run, choose Edit at line, and check the caret and undo. For runtime search, try `« 1 0 / »`; Find should locate the division. Repeated divisions must not produce a guessed jump.
3. Lua: run `print(1)` followed by `error("test")` on line 2. Choose Edit at line, correct the error, save and run again. Check undo and save/reopen.
4. CHIP-8: build `CLS` followed by `LD V0, 999`. Edit at line should select line 2. For a runtime error, `CLS` followed by `RET` has no matching call; Edit source should return to RET.
5. Recheck Open example with unsaved source, loader return, restart persistence and mixed use.

Host verification and device acceptance are reported separately. No SD deployment occurs as part of this build.
