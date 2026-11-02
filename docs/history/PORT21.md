# LumaRPL port 21 — complex numbers, geometry and extended finance

One colour release adds these tools and matrix improvements. Searchable calculation history is deliberately excluded. The canonical firmware remains `pico2-apps/LumaRPL-Pico-2-W.uf2`, compatible with UF2 Loader v2.4.1. Deployment is a separate requested action; user data and other applications must be preserved and the SD card left mounted.

## Complex workbench

Ctrl+T > Math & graphs > Complex workbench opens two operands, Z and W. Edit each as rectangular (real, imaginary) or polar (magnitude, phase), using bounded 31-byte scalar expressions. Z conversions / roots provides rectangular and polar output, magnitude, phase, conjugate and all n-th roots for integer n from 2 through 16. Z,W arithmetic provides addition, subtraction, multiplication and division. Zero has no defined phase and division by zero reports an error.

The phase unit is local to this workbench. Changing degrees/radians converts existing polar phase fields to preserve their values; conversions that exceed the field length use a 17-significant-digit numerical representation. Results use the upstream DB48X engine. Viewing and cancelling preserve the calculator stack and settings; F1 Copy explicitly pushes a result. Files saves/loads checksummed `.complex` files under `/db48x/complex/`. BASICS contains Z=3+4i and W=1-2i.

## Interactive geometry

Ctrl+T > Math & graphs > Graphs > Interactive geometry uses the existing native plotting engine, RGB332 framebuffer, panning, zoom, grid and export. The bounded construction contains six labelled points: infinite lines AB and CD, and a circle centered at E through F. Layers toggles each construction. This is a numerical geometry tool, not a general symbolic construction system.

Choose Explore / move points. Arrows select a point during normal tracing; Enter starts movement. While moving, arrows adjust by 1% of the current window span and Enter edits the point numerically. Esc or F5 returns to tracing, then returns to the parent. F2 Measure shows distances, circle radius, line angle, line intersection and AB/circle intersections. Parallel, coincident, degenerate and tangent cases are identified with relative numerical tolerance 1e-12. The circle display uses 96 segments. Graph actions retain native navigation/export controls; SVG includes the geometry, while the existing CSV format includes the six point samples alongside plot samples.

Files saves/loads checksummed `.geometry` files under `/db48x/geometry/`; LINES is the initial example. Unsaved geometry and complex documents remain in the existing checked document cache and participate in replacement/loader-exit protection. Coordinates are finite scalars within +/-1e50. The initial window is -6 to 6 on both axes.

## Matrix additions

Ctrl+T > Math & graphs > Matrix workbench retains A/B arithmetic and adds Named registers and Powers / row reduction. Registers enumerate 2D matrices in HOME, allow explicit Store A/Store B with overwrite confirmation, and import selected values into A or B without consuming the stack. Save the calculator session to persist HOME registers across restart. Existing `.matrix` files remain unchanged.

Integer powers accept -32 through 32; negative powers require an invertible square matrix. REF, RREF and rank use the upstream engine. Classify augmented system interprets the final column as the right-hand side and compares coefficient and augmented ranks to report unique, infinite or inconsistent solutions. Exact fractions are recommended: numerical conditioning affects floating-point rank decisions. Results offer explicit Copy and, where compatible, Use in A.

## Financial expansion

Ctrl+T > Math & graphs > Financial calculator > Growth / rate / term provides compound future value, regular-deposit savings, annual rate and payment-count solving. Inputs include the repayment used for rate/term and the savings deposit. Payments/deposits use the existing beginning/end timing and frequency. Rates are nominal annual percentages; payment counts may be fractional. Invalid, non-amortizing, indeterminate and non-convergent cases are reported without replacing valid inputs.

Cash flows > Find IRR searches the configured percentage interval for per-period IRRs. DOUBLEIRR has flows -100, 230, -132 and yields approximately 10% and 20%. IRR bounds must be greater than -100% and at most 10000%. The bounded search uses at most 8192 evaluations, checks scaled residuals and permits cancellation. It can miss repeated or closely spaced roots and never claims all roots were found. Annual-rate solving uses at most 200 iterations and bounds -99.999999% to 10000%.

Successful results remain on screen; Copy is explicit. Finance files now use version 2 to retain payment, deposit and IRR bounds. Existing version-1 finance files still load with defaults for new fields. Saves retain the existing temporary-file/backup recovery workflow. Formula conventions were checked against the primary [NumPy financial term documentation](https://numpy.org/numpy-financial/latest/_api_stubs/numpy_financial.nper.html) and [IRR documentation](https://numpy.org/numpy-financial/latest/_api_stubs/numpy_financial.irr.html); no additional runtime dependency was added.

## Verification and device testing

Host tests exercise real DB48X matrix powers, REF/RREF classification, complex arithmetic and root branches, local angle conversion, stack/settings preservation and production controls in eight themes. Independent numeric tests cover finance, geometry, corrupt documents and old finance files. Firmware memory, fixed reservations and UF2 partition/address checks are recorded in the accompanying reports. Visual coverage is explicitly scoped to inspected changed screens.

The instrumented numeric adapter and standalone numeric/codec tests use ASan/UBSan. Rendering is excluded from this sanitizer run because of the previously recorded packed-font alignment diagnostic. The unchanged DB48X mathematical core/fonts inherit the 8447-test desktop baseline from port 19.8; this is not a fresh full desktop suite. Device acceptance remains pending.

On the device, test BASICS complex arithmetic (Z+W=4+2i, Z*W=11-2i), square roots of 3+4i (2+i and -2-i), and polar unit changes. Move geometry points, check intersections, zoom and export. Save/reload all three new example types and test cancellation with missing SD. Store and reload a named matrix, check A^0 and A^-1, then reduce SOLVE.matrix. Compare loan rate/term with 5% and 60 payments; run DOUBLEIRR. Finally test editor recovery, loader return, cold restart and a 30-minute mixed-use session. Host verification does not establish these physical results.

## Measured firmware memory

| Resource | Port 21 | Change from port 20.1 |
|---|---:|---:|
| Flash image | 2,022,152 bytes | +34,632 bytes |
| Static SRAM before heap | 387,728 bytes | +32 bytes |
| Calculator arena | 212,992 bytes | +0 bytes |
| Bounded heap | 98,304 bytes | +0 bytes |
| Reserved stack | 32,768 bytes | +0 bytes |
| RGB332 framebuffer | 102,400 bytes | +0 bytes |
| Shared workspace | 16,384 bytes | +0 bytes |
| Unallocated SRAM gap | 5,488 bytes | -32 bytes |

The new complex and geometry documents occupy 132 and 136 bytes when resident, using the existing bounded heap/PSRAM cache. The finance document grows by 32 bytes to 216 bytes. These payloads do not reduce the calculator arena or increase any memory reservation. The UF2 application partition is 4,186,112 bytes; map and partition checks pass. No second framebuffer or unbounded history cache is added.
