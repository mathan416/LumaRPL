# LumaRPL port 21.1 — release-review fixes

This release fixes both issues from the port 21 release review. It retains the complex, geometry, finance, matrix and other tools documented in PORT21.md. The canonical firmware name remains `pico2-apps/LumaRPL-Pico-2-W.uf2`; SD deployment is a separate requested action.

## Fixes

Packed font/pixel sources in the indexed renderer are now read byte by byte for the requested pixel. The renderer no longer dereferences a potentially unaligned 32-bit source word, or reads an unnecessary word beyond an exact-length bitmap. Byte-packed destinations also use exact pixel writes. The aligned word path now bounds its lookahead reads, and overlapping moves use the safe pixel path. The same masks, orientation and RGB332 conversion are retained. This supersedes the older release notes' unresolved packed-font alignment diagnostic.

Geometry now compares distances with relative tolerances in the same units, without the previous unit-sized floor. It compensates both cross-product terms to preserve collinearity, avoids squaring tiny or large radii and projects intersection points from the circle center so distant line anchors do not erase small offsets. Point circles and degenerate lines are handled explicitly. Parallel-line coincidence checks normalize the local displacement instead of using an absolute tolerance floor.

Regression examples: radius 1e-8 and line y=1e-7 give no intersection; radius 1e-13 and a center-crossing line give two intersections. Distinct parallel lines y=0 and y=1e-13 remain distinct. Numerical geometry still uses relative tolerance 1e-12; constructions extremely close to tangency/parallelism remain numerically qualified.

## Verification and acceptance

The new packed-pixel regression covers all five source pixel modes, byte alignments 0–3, exact allocation ends, partial bytes/rows and clipping under ASan/UBSan. Geometry tests cover disjoint, tangent, crossing, point and degenerate constructions over scales 1e-200 through 1e200, translated coordinates and distant anchors. This helper-test range exceeds the UI document's existing +/-1e50 coordinate limit.

The release reports distinguish rendering-enabled production-controller sanitizer checks, standalone tests, simulator before/after comparison, visual comparison and physical acceptance. Linked simulator-core objects are not all sanitizer-instrumented; passing controller checks must not be described as a fully instrumented entire engine. Mathematical settings, saved formats, file locations, calculator memory reservations and the existing single framebuffer are retained.

Fresh before/after RGB332 desktop suites each passed all 8,447 tests with zero failures and zero assertions. Expected images were unchanged. Rendering-enabled controller sanitizer checks and the dedicated packed-pixel/geometry regressions passed. Scoped Lua allocation accounting ended at zero live blocks; repeated PSRAM/cache ownership tests showed no growing retained allocations. These exercised paths do not establish that every application path is leak-free.

Physical acceptance remains pending. On the PicoCalc, check compact digits and mathematical results, scroll help and menus, run Lua text/canvas demos, then test geometry at normal and small scales. Save/reload geometry and other documents, verify loader return and complete a 30-minute mixed-use session. Host checks do not establish physical stack high-water or SDK heap behavior.

## Memory and review scope

The measured flash image is 2,024,232 bytes, 2,080 bytes above port 21 and below the 4,186,112-byte application partition. Static SRAM remains 387,728 bytes including the arena/framebuffer. The 208 KiB arena, 96 KiB heap, 32 KiB stack, 100 KiB framebuffer and 16 KiB workspace are unchanged; 5,488 bytes remain unallocated between heap and reserved stack. Neither fix introduces an allocation or additional buffer.

The broader rendering check also corrected a host-test fixture that supplied 31 bytes for a 27-byte literal. It now passes the actual literal length. This was a test-fixture overread, not an application text-reader change. Host build rules now include the renderer header as a dependency and keep sanitizer objects separate from ordinary objects, preventing stale inline rendering code from hiding regressions.

## Guided device checks

1. Open Ctrl+R and Ctrl+A. Use Tab, F1–F5 and Ctrl+F5; confirm commands still select the displayed row. Check digits, menu labels and math results while opening/closing help and switching themes. Existing long soft-menu labels remain constrained by their six-column layout.
2. Open Math & graphs > Graphs > Interactive geometry > Point coordinates. Set A=(-1e-6,1e-7), B=(1e-6,1e-7), E=(0,0) and F=(1e-8,0). Distances / intersections must say AB / circle: none. The tiny circle will not be visible in the default window; use the measurements to check this case.
3. Set A=(-1e-6,0), B=(1e-6,0), F=(1e-13,0). Measurements must report two intersections near (+/-1e-13,0). Set F=(0,0) to test a point circle: one intersection at the center.
4. Save/reload the small geometry, then use Reset demo to restore a normal visible construction. Check point movement, zoom and export. Run a complex roots result and the Lua canvas/text demos, then verify loader return.

The prior review's findings describe the archived port 21 build. Port 21.1's sanitizer and scale-regression reports record their host-verified resolution; device acceptance is still a separate gate.
