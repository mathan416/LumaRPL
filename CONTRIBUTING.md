# Contributing

Keep calculator mathematics and saved formats compatible with DB48X. Keep the
calculator arena, bounded heap, reserved stack, framebuffer and shared workspace
at their documented sizes. Never hide a memory reduction behind a UI feature.

Changes belong in the PicoCalc platform layer where possible. Upstream changes
must also update patches/db48x-picocalc.patch and its file-hash manifest. Use
bounded buffers, checked allocations and scoped cleanup. Review DB48X GC root
requirements before holding calculator objects across allocations.

Run the relevant production-controller, sanitizer, Lua and allocation checks,
then memory and UF2 validation. Document test commands and limitations. Inspect
changed screens using real fonts and RGB332 rendering, in all affected themes.
Physical results must be reported separately from host results.

Do not commit Wi-Fi credentials, SD contents, user documents, session files,
local paths or generated builds. Do not update goldens to conceal regressions.
Include a Signed-off-by trailer using your own identity. Preserve upstream
copyright and license notices.
