# LumaRPL port 20.1 — matrix arithmetic

The matrix workbench now supports two independently editable matrices, A and B. It retains the port 20 matrix file format and all prior calculator features. SD deployment is a separate action.

Open Ctrl+T > Math & graphs > Matrix workbench. The main menu offers Edit matrix A, Matrix arithmetic (A, B), Matrix operations, Matrix A files and Help. Determinant, inverse, transpose, augmented solve and 2x2 eigenvalues are under Matrix operations.

## Arithmetic and controls

In Matrix arithmetic, Edit matrix B uses the same grid editor as A. Matrix B files saves, loads or imports B from the stack. Both operands support up to 6x6 cells with 31-byte scalar expressions; fractions such as 1/3 retain exact arithmetic.

- A+B and A-B require matching dimensions.
- A*B is the matrix product, with columns(A)=rows(B).
- A/B explicitly means A*inverse(B), not inverse(B)*A. B must be square and nonsingular, with compatible dimensions.
- Scalar arithmetic offers k*A, A/k and negation. k may be an exact real expression; division by zero is rejected.
- Swap A and B exchanges both complete grids without discarding either and marks both unsaved.

Viewing a result preserves A, B, calculator variables/settings and the stack. F1 Copy explicitly pushes the result. Matrix results additionally offer F3 Use in A, allowing the next calculation to use that result. Unsaved A replacement requires confirmation. Results exceeding the editor's dimensions or cell-string limits remain copyable to the calculator stack; a failed import keeps A and the result available. Scalar/eigenvalue-list results do not offer Use in A.

A and B save independently as .matrix documents under /db48x/matrices; choose distinct filenames, such as MATRIXA and MATRIXB. Existing files and bundled examples remain compatible. Unsaved B uses the checked PSRAM cache (bounded-heap fallback if necessary) and is included in loader-exit saving/cancellation protection.

## Verification

Real-engine tests assert addition, subtraction, multiplication, right division and exact scalar values; rectangular products; noncommuting division order; invalid dimensions; singular division; zero/invalid scalars; and stack, directory, precision and angle preservation. Production-controller cases check B editing/files, explicit copying, Use in A, cancelled replacement, swapping, B save/reload and exit protection in all eight themes. A targeted sanitizer build instruments the production matrix adapter while linking the actual DB48X engine; it is not a claim that every upstream engine translation unit was rebuilt with sanitizers. The clean sanitizer run exercises numeric operations without rendering. An initial rendering-enabled probe reported an inherited packed-font unaligned word read at db48x/src/blitter.h:1557; that unchanged renderer diagnostic is retained separately. Whole-UI sanitizer cleanliness is not claimed.

The calculator arena, heap, reserved stack, framebuffer and shared workspace reservations remain unchanged. Matrix B adds one bounded 1,164-byte document while resident, using existing PSRAM backing when inactive. The final image uses 1,987,520 bytes of the 4,186,112-byte application partition. Static RAM is 387,696 bytes, 16 bytes more than port 20; the unallocated gap is 5,520 bytes. Measured memory, loader-fit and stack-frame reports accompany the artifacts. Mathematical engine/fonts remain unchanged from port 20; the full 8,447-test desktop baseline is inherited, with fresh targeted matrix and UI verification reported separately.

Bundled MATRIXA.matrix and MATRIXB.matrix provide the following test operands. Load A through Matrix A files; load B through Matrix arithmetic > Matrix B files.

Physical acceptance remains pending. Test A=[[1,2],[3,4]], B=[[2,0],[0,3]]. Expected A+B=[[3,2],[3,7]], A-B=[[-1,2],[3,1]], A*B=[[2,6],[6,12]], and A/B=[[1/2,2/3],[3/2,4/3]]. Check k=1/3 scaling, Use in A chaining, cancelled replacement, invalid dimensions and zero division. Save/reload both matrices and restart, then check loader return and mixed use.
