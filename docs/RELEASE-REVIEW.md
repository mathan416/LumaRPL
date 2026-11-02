# LumaRPL 0.10.0-rc.1 release review

Review date: 2026-11-01. Baseline: port 30.5. This candidate retains the original
implicit-graph/optimization evaluator. It introduces no calculator features or
saved-format changes; the firmware version identifies the candidate.

## Findings resolved

1. The development source patch omitted upstream changes. The new patch captures
   all 21 changed files, including `src/scoped_array.h`; bootstrap checks its SHA-256
   and each resulting file hash against the pinned DB48X commit.
2. Build and validation depended on old release folders and prebuilt objects.
   Fresh pinned checkouts now build firmware and simulator objects. Legacy document
   fixtures are checked in with provenance hashes. Controllers have a stable entry
   point; the help console links the production PicoCalc settings implementation.
3. Build instructions and release naming were stale. VERSION, current instructions,
   contribution/security guidance, release checklist, packaging and a host CI
   workflow now live in a separate local Git repository. Historical port notes are
   explicitly separated from current instructions.

## Host verification

- Clean pinned dependency checkout and authenticated Lua archive: passed.
- Fresh Pico 2 W Arm firmware build and actual UF2 Loader v2.4.1 partition check:
  passed. See review/loader.json. No device boot is inferred from this.
- Host address/undefined-behaviour sanitizer tests: passed.
- Production controllers, normal and sanitized: passed; 325 screen IDs,
  29,557 scripted keys and 32,230 frames each. Sanitizers cover the production
  controller/adapters; this is not a claim that every Qt/upstream object is instrumented.
- Real-engine graph/calculus/Lua bridge probe: passed.
- Lua bounds, interruption, streamed compilation and repeated cleanup: passed.
- Arm newlib allocation, combined memory-budget and Lua cycles: passed.
- Arm real TLS setup/verification/free and exhaustion cycles: passed.
- Legacy geometry/math/science document compatibility and corruption tests: passed.
- Generated help, command reference, font and insertion assets: freshness checks passed.
- All 309 shipped RPL help examples: passed against the production-linked console.
- UI rendering audit: 325 registered call sites and 83 reviewed direct rendering
  sites; no unregistered rendering path detected.

Fresh unchanged-upstream and patched full suites each ran **8,443 tests with
zero failures and zero assertions**. Their test counts match; 1,376 golden-file
comparisons confirm expected images were unchanged. See
review/upstream-comparison.json, provenance and compressed full logs.
The GitHub Actions workflow has been prepared but has not run on GitHub.

## Memory and loader

Flash image: 2,272,816 bytes of the 4,186,112-byte application partition.
Static internal RAM before heap: 390,276 bytes. Calculator arena: 212,992 bytes;
bounded heap: 98,304; reserved stack: 32,768; RGB332 framebuffer: 102,400;
shared workspace: 16,384. Remaining unallocated internal gap: 2,940 bytes.
Arena/framebuffer/workspace are included in static RAM, not additional allocations.
No allocation regions overlap; all reservations match port 30.5.

## Visual review

48 captures covering diagnostics, implicit contour loading/plot/probe and bounded
optimization menu/results were inspected across all eight themes. Decoded pixels
match port 30.5 exactly. See review/sheet-01.png through sheet-03.png and
review/visual-review.json. This is a focused regression review, not a fresh visual
acceptance of every reachable screen.

## Remaining acceptance

This remains a release candidate. Cold boot, controls, graph interruption,
editor/save/restart recovery, network/rates, loader return and a 30-minute mixed-use
session must be checked on the candidate firmware. Prior port observations do not
substitute for that session. A dedicated security audit has not been performed.

No SD card was modified. No remote repository was created and nothing was pushed.
The local repository is ready for continued review; publishing/tagging a final
release remains separate from this candidate preparation.
