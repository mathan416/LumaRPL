# LumaRPL port 30.4 — evaluator diagnostic

Historical release guide: the temporary Compare diagnostic was removed in port 30.5 when the old evaluator was restored.

This release investigates the device slowdown reported after port 30.3: 956,432–976,733 microseconds for the 100-evaluation benchmark, compared with a previous value incorrectly transcribed as 118,319 microseconds (the original photo reads 1,183,319). PSRAM transfer time remained about 9.3 milliseconds. No cause or performance fix is established yet.

## Run the comparison

1. From the stack press Ctrl+T → Math & graphs → More: complex / applied math → Advanced studies → Memory / performance.
2. The existing single benchmark still runs on entry. Press F1 **Compare** on its result page.
3. Let all 12 batches finish. This may take several seconds. Esc cancels, retaining calculator state.
4. Photograph the **Evaluator comparison** summary. It shows old/new medians in normal and quiet modes, exact-result agreement and the measured CPU clock.
5. Press F2 **Details**. Open Old / normal, New / normal, Old / quiet and New / quiet in turn. Photograph their counters; scroll to see Other time and arena values. F5 returns to the summary. Return to Memory / performance and press F1 to repeat if needed.

The old evaluator is the archived port 30.1 implementation (the same evaluation path used in port 30). The new evaluator is the current production implementation. Both execute `SIN(X)*COS(Y)+X^2` for X=i*0.01, i=0…99 and Y=0.5, with scoped 16-digit precision, radians and the same hardware-floating-point setting. No memo/cache is attached to either evaluator, matching the original 100-evaluation benchmark.

There are three trials for each engine/mode, alternating which evaluator runs first. Each batch parses once, performs one untimed warm-up, and requests garbage collection before timing. Progress-screen drawing is outside the timed interval. The 600 old/new result pairs must agree bit for bit; mismatches are reported rather than hidden. These steady-batch measurements are not directly interchangeable with the original single-run number.

**Normal** retains the saved busy-animation interval. **Quiet** suppresses periodic busy animation for that batch. It does not disable keyboard polling, battery checks or Esc interruption. The saved interval is restored after success, cancellation and errors.

## Read the results

- New much slower than old in both modes points toward an evaluator-specific problem.
- Both improve markedly in quiet mode points toward busy rendering/transfer overhead.
- Keyboard polling includes its I²C bus time. The separately shown keyboard-bus counter is a subset, not another amount to subtract.
- LCD transfer measures conversion/transfer work, not all glyph drawing. Background and battery polling have separate counters.
- Other time is elapsed time minus the four non-overlapping I/O categories. It includes engine work, allocation/GC, drawing CPU work and instrumentation; it is not a pure arithmetic measurement.
- Details counters come from the actual median batch; the three trial durations are shown separately. Host runs use mocked I/O and cannot establish device speed or the cause of the regression.

No files are automatically saved. Send the summary and relevant Details photos. All calculator reservations remain: 208 KiB arena, 96 KiB heap, 32 KiB stack, 100 KiB framebuffer and 16 KiB shared workspace. The diagnostic uses the existing 8 KiB workspace lease and a four-byte profiling pointer; no additional framebuffer or unbounded cache is allocated.

Firmware/source/bundle verification and physical-device diagnosis are separate. Deployment requires a separate request. Port 30.3's direct decimal/binary/octal/hex conversions and port 30.2's plotting changes remain included.
