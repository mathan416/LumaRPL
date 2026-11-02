# LumaRPL port 20 — linked graphs, matrices, paired results and finance

Four additions share the native UI and existing calculator engine. Port 19.14 Wi-Fi recovery remains included. Calculator arena, heap, reserved stack, RGB332 framebuffer and shared workspace reservations are unchanged. No SD deployment has been performed by this build.

## Linked graphs and tables

In a native 2D, parametric/polar or ODE graph, F1 Table opens the same sampled points used to draw the graph. Up/Down selects a row; Left/Right pages. Enter or F1 Trace highlights that exact sampled point in the graph. F2 changes function where applicable. F3 copies X then Y explicitly (Y becomes stack level 1). F4 Details shows complete coordinates and the path parameter when applicable. Esc cancels table navigation without changing the graph's selection.

For scatter plots, the table contains the actual data rows rather than fitted sample values. In Data / statistics > Edit X,Y table, select a row and press F4 Plot: the graph highlights that row. Choose another data point through its F1 table and return to the editor to keep that row selected. Normal graph tracing and analysis remain available. Function sampling still uses 300 points; this feature does not claim extra numerical accuracy or allocate another sample array/framebuffer.

## Matrix workbench

Open Ctrl+T > Math & graphs > Matrix workbench. Arrows select a cell; Enter or F1 edits its complete expression, up to 31 UTF-8 bytes. A page shows three columns; moving across columns reveals the next group. F2 Size replaces the grid only after confirmation. Supported dimensions are 1–6 rows and columns. Fractions such as 1/3 remain exact where DB48X supports them; units and unresolved symbols are rejected.

The main menu offers determinant, inverse, transpose, augmented-system solving and 2x2 eigenvalues. Determinant/inverse require square matrices; singular inverse/solve reports an error. Solve uses N rows and N+1 columns, with the final column b: up to five unknowns. Eigenvalues use the characteristic roots for exactly 2x2; general eigenvectors and larger eigenvalue problems are outside this release. Input grids remain unchanged by operations.

Files saves/loads .matrix documents under /db48x/matrices, or imports the matrix at stack level 1 without consuming it. Documents contain dimensions, bounded cell strings, a version and checksum. Loads validate before applying; transactional saves retain recovery copies. Dirty grids survive tool navigation through the existing checked PSRAM cache, with bounded-heap fallback. Loader exit offers saving.

Bundled examples: FRACTION.matrix has determinant 2/3; SOLVE.matrix solves X+Y=3, X-Y=1 (X=2, Y=1); EIGEN.matrix has eigenvalues 3 and 1.

## Original value and numerical approximation

Native result screens retain the original typeset value and show a compact approximation when DB48X can produce one. F1 Copy copies the original by default. F4 Approx changes Copy and F2 Details to the approximation; F4 Original switches back. F2 exposes the complete selected value, with an explicit shortening notice if the reader's bounded text limit is reached. An original floating-point result is not labelled exact. An unsupported symbolic approximation leaves the original available.

Ctrl+T > Math & graphs > Value / approximation inspects the current stack top. Merely viewing or cancelling preserves the stack and settings. For an exact demonstration, enter 2 3 / at the calculator and view the stack value; copy the original fraction or its decimal approximation explicitly. This does not replace the calculator's configurable stack font or rewrite its main stack display.

## Financial calculator

Open Ctrl+T > Math & graphs > Financial calculator. Finance inputs set principal, nominal annual percentage, whole-number periods, periods per year and beginning/end payment timing. Monthly uses 12 periods per year; periods is the total payment count, not years.

Loan payment reports a positive payment magnitude that amortizes the principal to zero. Compound future value assumes no added deposits. Amortization reports remaining balance, principal repaid and interest after a selected whole payment count; Esc cancels the bounded calculation. Fees, taxes, lender-specific day counts and intermediate cent rounding are excluded. The annual rate is divided by the payment/compounding frequency; it is not an effective annual rate or a fee-inclusive APR.

Cash flows / NPV uses CF0 now and equally spaced later flows, up to 17 total. Negative flows are outflows and positive flows inflows. Its discount is a separate percentage per cash-flow period. These conventions match the documented [periodic payment equation](https://numpy.org/numpy-financial/latest/_api_stubs/numpy_financial.pmt.html) and [NPV interface](https://numpy.org/numpy-financial/dev/api.html); no NumPy runtime is included in firmware.

Save/load uses versioned, checksummed .finance documents under /db48x/finance, with transactional replacement and backup recovery. LOAN.finance supplies a 10,000 principal, 5% nominal rate, 60 monthly payments and example cash flows. Inputs remain in RAM/PSRAM while navigating; save before power-off. Finance does not overwrite RPL TVM variables. These guided tools do not add IRR, dated cash flows or a lender-specific mortgage mode.

## Verification and device acceptance

The final Arm image uses 1,983,216 bytes of the 4,186,112-byte application partition. Static RAM increases by 32 bytes; 5,536 bytes remain unallocated between heap and reserved stack. The matrix document is 1,164 bytes and finance inputs/cash flows 184 bytes while resident; inactive documents use the existing checked PSRAM cache. All fixed calculator memory reservations remain unchanged.

Host checks exercise actual DB48X matrix operations and paired-result copying, grid and finance controllers, linked sample/scatter selection, every theme, saved-document restoration and failed saves. Independent sanitizer tests check known payment/growth/NPV results, zero/tiny rates, beginning/end amortization, overflow, cancellation and every-byte corruption rejection for both document formats. Radio/storage boundaries in the UI harness remain mocked. Arm linker, fixed-memory, allocation, stack-frame and UF2 Loader partition checks accompany the release.

The mathematical core, fonts and desktop engine are compared with the port 19.14 source archive. The clean 8,447-test desktop baseline is inherited from port 19.8; it is not a fresh full-suite run. UI coverage and screenshot review are reported separately from device acceptance. No physical-device acceptance is claimed.

After separate deployment, test linked selection in functions, parametric/polar, ODE and scatter views; matrix example results and save/reopen/restart; original versus approximate copying; zero-interest loan payments, the bundled finance examples and NPV. Check missing SD and cancelled/failed saves keep documents, then loader return and a 30-minute mixed-use session. Recheck currency refresh and Wi-Fi/time recovery as regressions.
