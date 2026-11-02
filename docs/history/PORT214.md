# LumaRPL port 21.4 — examples and language references

Help & about now includes separate Lua reference and CHIP-8 reference entries. Both use the same native subject lists, Find, paging and scrolling reader as the RPL command reference. Up/Down selects or scrolls; Enter/F1 opens; F5/Esc returns. List shortcuts 1–8 and labelled Previous/Next retain their existing behavior. Reference pages only explain examples; opening a page never executes it.

The Lua reference has 33 pages. It covers the supported language/library subset and every calculator-specific function: calculator expressions, canvas drawing, fonts/measurement, colour sprites, native 2D/3D plotting, keyboard input, timers and sound. Every page has a runnable example and describes the output or visible result. It explicitly explains missing desktop file/module APIs, radians in Lua math and temporary console variables.

The CHIP-8 reference has 40 pages, with examples for every accepted assembler mnemonic, labels/data, registers, keypad mapping, profiles, fonts and timers. Examples distinguish classic CHIP-8 from Super-CHIP extensions. Register-only examples explain that they do not draw anything; a complete digit example waits for a key before returning.

RPL examples now include 269 worked command/setting entries with expected results and relevant state-change notes, plus existing examples in the guide and reference. Stack examples show remaining values in order. Matrix deletion shows both outputs; regression examples calculate the fit before reading its slope. Other menu/settings entries include control or preference examples. There are 693 Try it blocks among the 1,042 built-in reference topics; this is not a claim that every advanced/internal command has a tested numerical example. Mathematical implementations and saved formats are unchanged. Some additional executable examples are adapted from upstream DB48X documentation and checked against its stated result; attribution/license notices remain included.

The two new books are SD-backed bounded indexes and text files under /db48x/help. Their category tables are in flash; the existing eight-record page cache and single 4,097-byte temporary topic allocation are reused. No extra framebuffer or unbounded cache is introduced. Flash use is 2,011,648 bytes (800 more than 21.3). Static internal SRAM remains 387,628 bytes; the arena (208 KiB), heap (96 KiB), stack (32 KiB), framebuffer (100 KiB), workspace (16 KiB) and unallocated gap (5,588 bytes) are unchanged.

## Verification scope

Reports under reports/port214-* separate automated checks, visual samples and physical acceptance. Lua hardware/bridge callbacks are host fixtures; CHIP-8 examples use the production assembler/VM. Production controllers use real engine/widgets/fonts at PicoCalc dimensions with host device/SD boundaries. Reference file cleanup is checked under ASan/UBSan. This does not claim a whole-engine leak-free result or a fresh full desktop simulator suite. Physical testing and SD deployment remain separate.

## Device checks after deployment

1. Ctrl+T > Help & about > Lua reference. Open Drawing, then screen.text or screen.rect. Scroll, page and return; the selected list position should remain.
2. Try the screen.text example in a saved Lua script. F2 Save & run should display the greeting. Try the plot.function2d example and return from its native graph.
3. Open CHIP-8 reference. Read Keypad and Profiles. Create the Draw a digit example under Programming > CHIP-8 / assembly. Select SUPER-CHIP modern for EXIT support. Run it: a 5 should appear; press and release a mapped keypad key to return.
4. From the calculator, Ctrl+K finds RPL commands. Read DUP, RowSwap, Slope and StoreDiv. Try a simple example, checking the expected result and any note about variables/statistics/settings before running it.
5. Check text readability in your theme, restart after saving work, and confirm loader return.

Firmware keeps pico2-apps/LumaRPL-Pico-2-W.uf2. Matching source, resources, checksums, notices and UI captures accompany the release. No SD deployment occurs until requested.
