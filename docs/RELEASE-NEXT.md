# 0.10.0-rc.4 release-candidate readiness

Review date: 2026-11-17. VERSION is 0.10.0-rc.4. This is a prerelease candidate, not a stable release. It replaces rc.3 and fixes the findings from the full code review.

## Changes since rc.3

- Calculator entry: a typed power keeps a leading minus with its number
  (`-2^2` is 4), and operator keys no longer edit a hidden entry while help,
  graphics or stack browsing is shown.
- Alarms: Enter acknowledges; Esc/Back or a one-minute timeout snoozes; idle
  checks never raise errors and back off after a failed draw or save.
- Lua: F5 is an ordinary key; Esc interrupts pattern searches; `load` refuses
  bytecode and `string.dump` is removed.
- Editors and tools: Replace all is fully undoable up to 8 matches and asks
  first beyond that; CHIP-8 assembler numbers with leading zeros are decimal;
  ODE examples ask before replacing unsaved work.
- Storage and devices: failed saves remove their temporary files; startup
  reports a recovered session; SD v1 detection, large-card sector counts,
  PSRAM transfer timing, key-queue pairing and alarm timing statistics fixed.
- Release tooling: committed-content packaging, verification of the `db48x`
  tree against the patch, explicit checks under optimized Python, and isolated
  simulator test runs.
- Manuals: rebuilt with these changes. The User Guide is now 64 pages; the
  Programming Guide (84) and API & Assembly Reference (71) keep their counts.
  All 219 pages pass the out-of-page and sparse-page checks.

## Verification

All checks ran on candidate commit `d0f753f` (firmware SHA-256 `16122123ced26c31423fa3a3a43655e48f51471a08ace4857bb1f2f4d7b4fad5`) and are recorded in `docs/review/release-evidence.json` with logs in `docs/review/rc4/`.

- The patched full simulator suite passes all 8,507 tests with zero failures and zero engine assertions. The pristine-upstream baseline is the rc.3 run of the same pinned upstream commit, reused because the pin and toolchain are unchanged.
- The 135 calculator contract checks pass normally and under address/undefined-behaviour sanitizers.
- Production controllers pass in all eight themes, normally and under sanitizers: 325 screen IDs, 29,583 keys and 32,256 frames.
- 41 bounded host test programs, Lua, Arm allocation and Arm TLS, 310 help examples and 135 manual examples (29 RPL, 55 Lua, 51 CHIP-8) pass.
- All 219 manual pages pass layout checks; the release-tool tests pass.

Hardware testing of this exact rc.4 image has not been done yet. Earlier device confirmations apply to the rc.3-era image they were recorded for.

## Before publishing

- Install the exact rc.4 image and confirm on hardware: cold boot, stack and soft menus, alarms (Enter acknowledges, Esc/Back snoozes), Lua keys (F5 readable, Esc interrupts), save/restart, Wi-Fi/NTP and a 30-minute mixed-use session. Record the result and firmware hash in `device_acceptance`.
- Publication requires a clean committed tree, passing `tools/verify_release.py`, checksummed assets from `tools/package_release.py`, green CI on the release commit and a verified annotated tag. Packaging never deploys to an SD card.

## rc.3 history

The rc.3 readiness record (review date 2026-11-06) and its evidence remain in `docs/releases/0.10.0-rc.3.md` and `docs/review/rc3/`.
