# LumaRPL port 19.6 — Advanced RPL settings

Tab now selects calculator soft-menu rows without shifting ordinary Enter, editing or arithmetic. Enter duplicates the top stack value normally; it does not invoke Show or toggle alpha merely because a different menu row was selected. F1–F5 use the selected row; Ctrl+F5 selects its sixth entry. Esc dismisses errors without changing the selected row; closing the menu clears the row selection. Interactive stack navigation retains its previous controls.

The former **Modes** action is labelled **→Prog** to distinguish it from a submenu. This upstream command places a program of nondefault settings and the selected menu on the stack. A long program is expected, especially with a colour theme. Run it with `EVAL` to apply the settings it contains. Settings omitted because they were at their defaults are not explicitly restored by that program. Viewing the program is optional. Full-screen Show may reject a long settings program as too large; dismiss that error with Esc. It must leave a clean calculator view. **Reset** restores factory RPL settings, including display settings; **System** opens PicoCalc Settings.

Full-screen Show now resets its drawing clip while copying the image. Returning to the calculator through a forced redraw clears the full previous view before drawing the stack and menus, preventing stale graphics and hints.

PicoCalc does not expose the upstream minimum-voltage setting in its advanced menu: the keyboard controller reports battery percentage, and the port uses that reading for its low-battery check. The underlying compatibility setting and saved formats are retained. Beep controls sound; Flash controls the visual flash.

Numeric settings consume a stack argument. For example, enter `3`, press Enter, then choose Display > Fix for three decimal places. Precision consumes its desired digit count; font settings consume a valid upstream font ID. Missing or invalid arguments report an error and leave the setting unchanged. Beep and other marked flags toggle without arguments.

## Verification

The production controller audit enumerates 148 slots in Modes, Display, Separators, UI (including all pages), Math, User and Angles. It checks 102 setting outcomes, duplicate key releases, numeric missing-argument errors, submenu entry, Reset, settings-program execution and PicoCalc System routing. Auxiliary commands receive missing-argument/cancellation checks; valid Header, Assign/Recall/Delete/Store/KeyMap and representative angle operations are checked separately. The full desktop suite supplies broader mathematical-command coverage. This does not establish exhaustive correctness for every input to every upstream command.

All advanced menu pages have native-resolution captures in eight themes, alongside settings-program and Show-return captures (112 reviewed samples). Host tests use the actual engine, fonts, RGB332 renderer and controllers, with device/storage boundaries adapted. Firmware memory and loader fit, sanitizer, real-engine, Lua and calculation checks are separate. See the accompanying reports for fresh full-suite comparison and known baseline failures. Hardware acceptance remains pending.

Measured firmware image: 1,940,256 bytes in the 4,186,112-byte loader partition, 944 bytes more than port 19.5. Static internal RAM remains 387,852 bytes before the bounded heap; arena 208 KiB, heap 96 KiB, stack 32 KiB, framebuffer 100 KiB and workspace 16 KiB are unchanged, with a 5,364-byte unallocated gap.

## Device checks after deployment

1. Home > Settings > Calculation > Advanced RPL settings. Tab to the row containing →Prog, Reset and System. F1 should put a settings program on the stack. Enter should duplicate it without corrupting the screen or enabling alpha.
2. Use Esc to close menus. Invoke `Show` on the program; if it reports a size limit, dismiss with Esc. Esc from a successful Show should also restore a clean stack view. Reopen Advanced RPL settings.
3. On the middle row, F3 toggles Beep; press/release it twice. The same row must stay selected. Use Ctrl+F5 to open Separators from that row.
4. Open Display. Put `3` on the stack before choosing Fix. Try Fix with an empty stack: dismiss the missing-argument error with Esc and confirm the menus redraw cleanly.
5. Open UI and visit every page. Check the sixth-key navigation and Back behavior. Use System to visit PicoCalc Settings and return.
6. Reset changes factory settings: save a session first if you want to preserve your current choices. Confirm the calculator remains usable after Reset, and restore your desired theme/settings afterward.

No SD deployment is performed by building this release. Existing saved formats and currency-pair persistence are unchanged.
