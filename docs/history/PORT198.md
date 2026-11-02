# LumaRPL port 19.8 — return-frame and verification fixes

The calculator firmware is `LumaRPL-Pico-2-W.uf2`. Saved formats, mathematical commands, UI and Help content are preserved. Deployment is a separate action.

## Firmware fixes

Empty conditional frames encode an absent branch as a null pointer with an all-bits-one endpoint. The upstream code created and tested that sentinel through undefined pointer arithmetic; optimized builds reported an assertion even for `« CASE 1 THEN 42 END 99 END » EVAL`, although the result was 42. The port now encodes/tests the endpoint as an integer and handles local-count/empty frames before object-pointer arithmetic. The invariant remains checked; it is not disabled.

`run_push` now propagates a failed frame allocation instead of reporting success. Regression checks exhaust the recursion budget inside plain and conditional calls, catch the error, then calculate 2+3 successfully.

Targeted pointer instrumentation also exposed two related issues: clearing the render cache formed a range from a null pointer, and parsing locals rewound a source pointer through an unsigned negative offset. Cache invalidation now compares integer address ranges; the parser subtracts within the source buffer directly.

## Host verification fixes

The 15 historical desktop Help failures came from testing the DB48X manual against colour screenshot fixtures for the DB50X manual. With the matching pinned upstream manual and a byte-exact generated index, all 32 Help checks pass using the old executable and unchanged expected images. This was a test-fixture mismatch; the calculator's PicoCalc command reference and other Help content have not been changed.

The suite runner now streams and inspects both output channels. Any assertion fails verification even when the simulator returns exit code zero. A regression demonstrates this using the actual previous engine; unit checks also cover clean success, nonzero exits and index offsets for both manuals.

Real-engine return-frame tests cover first/second/default/no-match/nested CASE branches, the original DoSubs example, local-variable cleanup, empty IFT, IFTE, counted loops and recovery from exhausted frames. Production Run tests exercise scalar CASE/local results and recovery in eight themes. Full-suite results, memory measurements and artifact verification are supplied in the accompanying reports. Physical acceptance remains separate.

## Device checks after deployment

1. Run `« CASE 1 THEN 42 END 99 END » EVAL`: expect 42.
2. Run `« CASE 0 THEN 42 END 99 END » EVAL`: expect 99.
3. Run `{ 1 2 3 4 5 } « → a b « CASE 'NSUB=1' THEN a END 'NSUB=ENDSUB' THEN b END 'a+b' EVAL END » » DOSUBS`: expect `{ 1 5 7 5 }`.
4. Run an existing saved RPL program, then use normal arithmetic and the stack menus.
5. Check Advanced RPL settings Tab/Beep/Ctrl+F5 and the command reference as before. Confirm normal screen restoration and loader return.

No SD deployment is performed by building this release.

## Memory measurement

Flash image: 1,948,056 of 4,186,112 application-partition bytes (8 bytes more than port 19.7). Static RAM before heap remains 387,536 bytes. Arena 208 KiB, heap 96 KiB, stack 32 KiB, framebuffer 100 KiB and workspace 16 KiB are unchanged; the unallocated gap remains 5,680 bytes.
