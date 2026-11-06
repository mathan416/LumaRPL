# Next release-candidate readiness

Review date: 2026-11-05. VERSION remains 0.10.0-rc.2; these changes are Unreleased. No new tag, GitHub release or candidate bundle is created by committing this update.

## Changes under review

Complex Gamma/factorial; exact Termial; native constants and signed entry; Math-menu shortcut; immediate postfix and RPN power operations; grouped formula evaluation; partial cursor redraw; cursor-safe operators; and original-formula error recovery. The API & Assembly companion adds a reviewed 71-page PDF and nine maintained examples. The calculator completion pass fixes the seven review failures, preserves submenu rows, adds the supported Fourier/probability/matrix/symbolic commands, and retires inherited empty menu entries. The Time/Date Alarms menu now supports 32 one-time local-clock reminders, visual alerts, acknowledgement/deletion and saved-session restoration; it does not wake powered-off hardware. See `docs/CALCULATOR-CAPABILITIES.md` for exact scope and limits.

## Available evidence

- The complete patched simulator suite passes all 8,507 tests with zero failures or engine assertions. The 135 numerical/contract checks pass normally and with address/undefined-behaviour sanitizers; 39 bounded host executables, Lua, 310 shipped-help examples and 135 manual examples also pass.

- Calculator keyboard/controller checks pass in all eight themes, including address and undefined-behaviour sanitizers. Grouped expressions, classic complex literals, operator entry, error repair and pixel-equivalent partial redraw are covered. Reminder alerts preserve the stack, existing errors and queued keys, and pending/acknowledged alarms survive explicit session save/load.
- Target firmware builds and passes RP2350 UF2/loader/partition validation. See `docs/review/calculator-completion.json` for the tested firmware hash and scope.
- Existing command opcode values are retained; new commands are appended. The normal binary-file version checks still apply. FiniteDifference avoids reserving DELTA, and bare variables such as qr remain valid in formulas.
- Generated help/reference/insertion freshness, upstream patch hashes and PDF checksums are checked before commit.
- All 135 manual examples pass: 29 RPL, 55 Lua and 51 CHIP-8, plus production assembler/VM routine contracts. Physical execution of the new examples remains separate. See `docs/manuals/API-AUDIT.md`.

## Before tagging the next candidate

1. Require passing CI on the pushed commit and run the complete simulator suite from a clean patched checkout. The current full-suite and firmware evidence is recorded in `docs/review/calculator-completion.json`; the catalogue expectation accounts for the new Ack command.
2. Review the regenerated User and Programming Guide PDFs together with the capability table. Keep manuals, help and the tested firmware synchronized.
3. Choose the next candidate version, rebuild from its exact commit, and produce a checksummed candidate bundle with corresponding source.
4. Cold boot that exact firmware and complete the device and mixed-use checks in `docs/RELEASE-CHECKLIST.md`, including cursor editing, error repair and save/restart.

The completion pass is tracked in `docs/review/calculator-completion.json`. Final release acceptance is still pending; SD copying does not establish device acceptance.
