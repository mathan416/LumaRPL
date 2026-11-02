# LumaRPL port 30.2 — implicit graph and optimization performance

This release reduces repeated work while retaining the same DB48X arithmetic, precision, grid resolution, search budgets, stopping tolerances and result formats. It also includes port 30.1's direct Bin/Oct/Dec/Hex conversion and Ctrl+Y Bases shortcut.

Pure numerical expressions in X/Y reuse rooted coordinate objects and the upstream plotting bindings. A conservative whitelist excludes named parameters, user programs, units, random values and unfamiliar operations; those retain the original interpreter and directory-binding path. Arithmetic still runs through DB48X in the original order. A small exact-coordinate cache uses the existing shared workspace, never approximates nearby points and checks cancellation before returning a cached result. Cache hits still consume the original numerical request budget.

Contour storage writes only the segments produced, rather than initializing all 1,536 possible PSRAM records. Drawing reads checked records without rewriting them. SRAM fallback and existing editor/ODE record behavior remain supported. No additional framebuffer or permanent cache is allocated.

The 208 KiB calculator arena, 96 KiB heap, 32 KiB stack, 100 KiB framebuffer and 16 KiB shared workspace are unchanged. Static RAM remains 390,276 bytes, with the same 2,940-byte unallocated gap. Flash grows by 1,016 bytes to 2,273,784 bytes, within the 4,186,112-byte loader application partition.

Host comparisons against the archived port 30.1 engine check bitwise field values, contour endpoints and optimizer results, alongside cancellation, forced garbage collection, allocation failure and caller-state preservation. Host timings are indicative; they do not establish PicoCalc speedups. See the accompanying release report for measured cases and limitations.

## Device checks after installation

1. Open Settings → device diagnostics → Numerical memory / performance. Run the same benchmark used on port 30. Your previous photo showed 1,183,319 microseconds for 100 expression evaluations, a 10,128-byte stack high-water mark and an 8,192-byte shared-workspace peak. Record the new values. Stack/workspace high-water marks include earlier activity, so compare fresh boots.
2. In implicit graphs, plot the same circle and contour levels as before. Check the shape, tracing, zoom/pan, redraw and Esc interruption. Measure the initial plot and redraw separately; PSRAM traffic savings may be most visible on redraws.
3. Repeat the optimization example `(X-1)^2+(Y+2)^2`. The minimum should remain X=1, Y=-2, F=0. Also try a bounded maximum and an interrupted search. Time the same bounds and starting point on both releases.
4. Try an expression containing a stored parameter, such as `A*X+Y`, after assigning A. This deliberately uses the original evaluation path and is not promised the same speedup.
5. Reopen an editor document, use undo/redo, run a Lua plot, save/reload and return to the loader. These check existing users of shared storage/workspace.

The PSRAM diagnostic's 4 KiB write/read timing is unchanged by design: this update reduces contour transactions rather than changing PSRAM bus speed. No SD deployment or physical acceptance is implied by host checks.

Baseline correction: older notes transcribed the original photo as 118,319 µs. It reads 1,183,319 µs. Port 30.5 restores the old evaluator; the speed comparison proposed in this historical guide is not a claim of a new regression.
