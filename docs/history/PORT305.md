# LumaRPL port 30.5 — old evaluator restored

Implicit graphs, contours and bounded optimization now use the original port 30 evaluator again. The newer variable-binding shortcut and numerical memo cache are removed. Calculator precision, algorithms, bounds and iteration limits remain unchanged.

Unified plotting, graph analysis, Lua plotting, all workbenches, saved formats, direct decimal/binary/octal/hex conversion and the checked PSRAM contour-storage improvements are retained. The temporary F1 Compare diagnostic and profiling hooks are removed. Memory / performance returns to its original single-benchmark page.

Use the calculator normally. For a quick check, plot `X^2+Y^2=25`, then minimize `(X-1)^2+(Y+2)^2` within X/Y bounds -10…10 from X=0, Y=0. The minimum should remain approximately X=1, Y=-2, F=0. Check Esc cancellation and return to the stack.

Memory reservations remain 208 KiB for the calculator arena, 96 KiB heap, 32 KiB stack, 100 KiB framebuffer and 16 KiB shared workspace. No calculator-memory reduction or new performance tuning is included. Build verification and physical acceptance are separate, and deployment is a separate requested action.

Correction to older testing notes: the original benchmark photo reads **1,183,319 microseconds**, not 118,319. The apparent eightfold regression resulted from that transcription error. This release follows the requested choice of evaluator; it does not claim an eightfold speed improvement.
