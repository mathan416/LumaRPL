# LumaRPL port 21.2 — PicoCalc help

The built-in guide and contextual tool help now start with tasks and examples. The command reference puts purpose before usage notes, adds individual descriptions for value settings and flag states, and replaces generic editor disclaimers with command-specific behavior. It retains upstream mathematical programs and stack descriptions.

The review covers 11 built-in guide topics, 17 tool-help pages, 103 native menu contexts and 410 action explanations, plus all 1,026 parseable command/setting entries and their accepted spellings. Entries not needing changes remain intact. All 132 value settings and 196 flag states have explicit purpose metadata. New console examples show input and expected output and are checked by `tools/test_help_examples.py`.

Controls distinguish physical Press, on-screen Choose and source Type. The help explains Tab row selection, Ctrl+F5, numeric settings, source-editor shortcuts, explicit result copying, immediate currency updates and separate session/document saves. Notebook files preserve expressions and notes; results require an explicit rerun after loading. The RPL square-program example uses SQDEMO because SQUARE is a reserved command name.

Source and tests

Edit the shared sources under docs/ui and docs/command-reference, then run both help generators. Authoring rules and research links are in docs/ui/HELP-AUTHORING.md. Generated outputs include resources/help/LumaRPL.md, db48x.md, db48x.idx and pico/ui/help_data.h. Upstream db48x/help remains unchanged.

Validation reports are under reports/port212-*. The new worked examples are individually executed; the reference's existing advanced programs are preserved rather than claimed individually re-executed. Mathematical engine code is unchanged from port 21.1, whose full 8,447-test suite passed. This help update uses targeted reference, engine-example, reader/controller, font, firmware-memory and loader checks. Device acceptance remains separate.

Device checks after a separately requested deployment

1. Open Help & about > Calculator reference. Read a topic, scroll and return; repeat through a direct tool's Help.
2. Open the command reference and search UIAlpha, EditorReplace, StackFont, BeepOn and Root. Check that purpose, arguments and context are clear.
3. Find PicoCalc worked examples. Close Help and try the saved SQDEMO program, angle examples and unit conversion. Save important state first because examples can change it.
4. Check Tab/F-key row selection and Esc navigation in Advanced RPL settings. Check source-editor Help without confusing its shortcuts with calculator commands.
5. Inspect help in your preferred theme on the physical display. Save and return to the loader normally.

This release does not change saved formats, mathematical operations or memory reservations. The canonical firmware remains pico2-apps/LumaRPL-Pico-2-W.uf2. Deployment is separate and the SD card has not been modified.

Measured firmware flash: 2021712 bytes. Static SRAM remains 387728 bytes; the 208 KiB arena, 96 KiB heap, 32 KiB stack, 100 KiB framebuffer and 16 KiB workspace reservations are unchanged. Host/visual checks passed; physical acceptance remains pending.
