# LumaRPL port 19.9 — readable calculator soft keys

Calculator soft menus now use the 13-pixel proportional compact face, with a mathematical-glyph fallback where needed. Text is centred using actual glyph bounds and the visible button interior; setting markers and submenu corners receive their own space. `%Total` remains complete and stationary in the command catalog. Labels genuinely wider than a cell retain upstream scrolling. Row geometry, Tab selection, F1–F5 and Ctrl+F5 mappings are unchanged. Native five-slot action bars and Help content are unchanged. Port 19.8's runtime fixes are included.

## Access and controls

- Ctrl+T > Help & about > Command catalog. Alternatively type `CATALOG` at the calculator prompt and press Enter.
- Ctrl+T > Settings > Calculation > Advanced RPL settings. Alternatively type `ModesMenu` and press Enter.
- Tab selects the next calculator soft-menu row and keeps it selected. F1–F5 activate columns 1–5; Ctrl+F5 activates column six, including page arrows where displayed.
- Individual commands can be typed by name and entered, with their required arguments on the stack. Opening the catalog inserts commands; it does not supply their arguments.
- Ctrl+K searches the command reference. There is currently no separate dedicated Ctrl shortcut for the catalog or Advanced RPL settings. Physical Shift/Caps Lock controls QWERTY typing.

## Verification

The production drawing observer checks 31,648 soft-key placements across the full catalog and representative calculator menus. It rejects stationary text extending beyond its measured interior, and asserts `%Total` fits in every theme and button style. Real production controller checks cover all 173 registered screen paths, Tab/Beep/catalog selection, sixth-entry controls, ordinary Enter, editor recovery, RPL execution and the existing native workflows. All 176 focused menu captures were visually inspected across eight themes and round/square styles.

The desktop menu function is preprocessed and compared with port 19.8, allowing only a local font-pointer alias; its behavior and all other reader code are unchanged outside Pico-specific branches. The clean port 19.8 full desktop suite (8,447 tests, zero failures and assertions) is reused, not reported as a fresh port 19.9 full-suite run. Mathematical core and Help assets remain unchanged. Fresh target memory, allocation-layout and loader checks accompany the release.

Flash: 1,948,376 bytes (+320 from port 19.8). Static RAM remains 387,536 bytes before heap. Arena 208 KiB, heap 96 KiB, stack 32 KiB, framebuffer 100 KiB, workspace 16 KiB and the 5,680-byte unallocated gap are unchanged. No new font asset, framebuffer or persistent label cache is allocated.

## Physical checks after deployment

1. Open Catalog; find `%Total`. Confirm the final `l` is visible and the label is stationary.
2. Open Advanced RPL settings. Check Grads, Angles, Beep, Flash, Display, Seps, →Prog, Reset and System.
3. Tab once, press F3 twice: Beep toggles twice and the row stays selected. Ctrl+F5 opens Seps from that row.
4. Check Display, UI and Stack pages, submenu corners and setting indicators. Very long labels may scroll inside their own button.
5. Check both round/square styles if used, then normal calculations and loader return.

Firmware: `LumaRPL-Pico-2-W.uf2`. Deployment is separate; device acceptance remains pending.
