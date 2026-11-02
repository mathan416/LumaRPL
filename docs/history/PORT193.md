# LumaRPL port 19.3 — PicoCalc reference, Details and themes

Ctrl+T > Help & about > Calculator reference now opens a built-in PicoCalc guide. Eleven topics cover keys/menus, the RPL stack, graphs/calculus, equation solving, data/forms/notebooks, programming/sprites, conversions, files/recovery, networking and settings. It works without SD. The final entry retains the upstream DB48X command reference for mathematical command semantics; its SwissMicros hardware notes are separate from PicoCalc controls.

The same guide ships as `/db48x/help/LumaRPL.md` for reading on a computer. Authoritative topic and menu text lives in `docs/ui/`; `tools/generate_ui_help.py` generates the flash-resident firmware text and SD copy. `--check` rejects missing registered-menu contexts, stale output and explanations that exceed the bounded reader buffer.

F4 Details now shows the full selected label, what its action does and relevant context. All 77 registered native menus have explanations, including 282 specific action descriptions. Currency Details explains Amount/From/To, page navigation and immediate rate availability. Solver Details distinguishes local solving, interval searches and symbolic isolation. Details does not execute the action and returns to the same menu row/page. Unregistered shared lists do not display an unhelpful Details button. File-browser F4 Actions and calculator soft-menu F4 retain their existing meaning.

## Eight themes

Choose Home > Settings > Display / theme. The original Classic light, Midnight blue and Amber terminal are joined by:

- Forest: deep green, pale mint text and green controls.
- Paper: warm ivory, dark brown ink and burgundy controls.
- Ocean: deep teal, pale cyan text and turquoise controls.
- Graphite: black with neutral grey controls and white text.
- Plum: deep purple, soft lilac text and violet controls.

Headers, native action bars, selection, graph overlays, editor chrome and calculator colors follow the palette. Text is corrected for contrast after RGB332 quantization. Native curve and mesh strokes adapt their display contrast while retaining readable requested colours; graph exports follow the displayed theme. Saved curve colours are unchanged. Existing mathematical font choices are retained. Theme colors use existing session settings: Ctrl+S saves them for restart; no saved format changes are introduced. Lua/CHIP-8 drawing and sprite pixels are not recolored.

## Device acceptance

1. Open Calculator reference. Read Start here, Keys & menus, Units & currency and Files, saving & recovery. Use Up/Down to scroll, Left/Right to page, F5/Esc to return. The topic menu itself spans two pages. Check that opening/closing the guide preserves the stack.
2. In Conversions > Cached currency rates, select From and press F4. It should explain source currency selection. Return with F5: From stays highlighted. Repeat on Amount, To and Refresh. Left returns to the conversion controls after a download; refreshed rates work immediately.
3. Check Details in Math workbench, graph exploration, Programming/editor Tools and Settings. Reading should neither run the action nor modify documents.
4. Try all eight themes in Settings > Display / theme. Inspect a selected row, reference text, the main stack/date/battery header, formula/result, Lua output, editor syntax and graph controls. Original user-colored sprite/Lua/CHIP-8 pixels remain their original colors.
5. Save the session with Ctrl+S, restart and confirm the selected theme colors remain. Also check loader return and normal calculation.

## Verification scope

Current host tests execute production controllers at 320x320 with real fonts and RGB332 drawing across all eight themes. They check Details focus/state preservation, reference navigation without SD, palette distinction and text contrast. Existing error-dismissal, sticky rows, RPL Run, editor recovery, Lua, sprites and immediate-rate tests remain in the controller run. Sanitizer, real-engine/shared-widget, calculation, Lua, rendering-audit, memory/stack and UF2-loader checks are recorded separately.

There is no new framebuffer or UI cache. Internal reservations remain: 208 KiB calculator arena, 96 KiB heap, 32 KiB stack, 100 KiB framebuffer and 16 KiB workspace. Metadata and guide text reside in flash. The temporary Details buffer is confined to a separate non-inlined reader function, avoiding a large buffer in every nested menu frame.

Visual review covers the changed theme, reference and Details screen samples, separately from the full generated capture inventory. The upstream calculator source is unchanged from port 19.2; no fresh full desktop suite is claimed for this platform UI release. Physical verification of port 19.3 remains pending. SD deployment is a separate action.
