# LumaRPL port 19.2 — calculator menu recovery

Corrects the blank calculator and leftover native footer after dismissing an error or returning from a native screen. Calculator handoff resets clipping and redraws all components; periodic menu animation cannot paint through the error page. Held input is suppressed during the handoff.

Single-row calculator menus now display the Tab-selected row. Changing menu height, hiding a menu or opening reference help removes obsolete menu and shortcut-hint pixels. Calculator F1–F5 select columns 1–5; Ctrl+F5 selects column 6. Native action bars are unchanged.

FixedDisplay (Fix) requires a digit count from the stack. An empty stack correctly produces “Too few arguments”. Enter `3`, press Enter, then choose Fix to select three decimal places. Beep is a toggle and needs no stack argument.

## Focused device checks

1. Open Advanced RPL settings. Tab to the row containing Beep, press F3 twice. Beep should toggle and the selected row should stay active.
2. Open the command catalog, Tab to the row containing `<`, then F1. Confirm the selected command is inserted.
3. Open display modes with an empty stack and choose Fix. Dismiss the error with Esc. The complete stack, menus and header should return, with no blue native footer or isolated label fragments.
4. Repeat Fix with `3` on the stack. It should succeed. Restore your preferred display mode afterward.
5. Close calculator menus. The Tab/Ctrl+F5 hint should disappear. When menus are visible, the hint is intentional.
6. Repeat with single-row menus and the other themes; check Tab, F1–F5 and Ctrl+F5. Return through Home and verify no held key activates the next screen.

The production frame composer is shared with regression tests, which exercise non-forced error dismissal, restrictive clipping, menu removal and single-row settings in three themes and square/round styles. Target firmware font assets are compiled explicitly for host captures.

Memory reservations and saved formats are unchanged: calculator arena 208 KiB, heap 96 KiB, stack 32 KiB, RGB332 framebuffer 100 KiB and shared workspace 16 KiB. Flash image: 1,890,096 bytes; unallocated internal RAM gap: 5,376 bytes.

Host testing is separate from physical acceptance. No device acceptance or SD deployment is claimed. The prior port 19.1 full desktop baseline is retained; the full suite was not rerun for this Pico UI hotfix. Current targeted engine, controller, sanitizer, Lua, allocation and loader checks are reported separately.
