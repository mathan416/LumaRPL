# 0.10.0-rc.3 release-candidate readiness

Review date: 2026-11-06. VERSION is 0.10.0-rc.3. This is a prerelease candidate, not a stable release. Candidate build provenance and checksums are included with its GitHub assets.

## Changes under review

Complex Gamma/factorial; exact Termial; native constants and signed entry; Math-menu shortcut; immediate postfix and RPN power operations; grouped formula evaluation; partial cursor redraw; cursor-safe operators; and original-formula error recovery. The API & Assembly companion adds a reviewed 71-page PDF and nine maintained examples. The calculator completion pass fixes the seven review failures, preserves submenu rows, adds the supported Fourier/probability/matrix/symbolic commands, and retires inherited empty menu entries. The Time/Date Alarms menu now supports 32 one-time local-clock reminders, visual alerts, acknowledgement/deletion and saved-session restoration; it does not wake powered-off hardware. See `docs/CALCULATOR-CAPABILITIES.md` for exact scope and limits.

## Available evidence

- After the verified SD deployment, the user confirmed that alarms and time work on the device (2026-11-05). This confirms those functions; the user subsequently confirmed a 30-minute mixed-use session without issues and successful save/restart with alarm restoration. Other individual checklist items were not separately asserted.

- The complete patched simulator suite passes all 8,507 tests with zero failures or engine assertions. The 135 numerical/contract checks pass normally and with address/undefined-behaviour sanitizers; 39 bounded host executables, Lua, 310 shipped-help examples and 135 manual examples also pass.

- Calculator keyboard/controller checks pass in all eight themes, including address and undefined-behaviour sanitizers. Grouped expressions, classic complex literals, operator entry, error repair and pixel-equivalent partial redraw are covered. Reminder alerts preserve the stack, existing errors and queued keys, and pending/acknowledged alarms survive explicit session save/load.
- Target firmware builds and passes RP2350 UF2/loader/partition validation. See `docs/review/calculator-completion.json` for the tested firmware hash and scope.
- Existing command opcode values are retained; new commands are appended. The normal binary-file version checks still apply. FiniteDifference avoids reserving DELTA, and bare variables such as qr remain valid in formulas.
- Generated help/reference/insertion freshness, upstream patch hashes and PDF checksums are checked before commit.
- All 135 manual examples pass: 29 RPL, 55 Lua and 51 CHIP-8, plus production assembler/VM routine contracts. Physical execution of the new examples remains separate. See `docs/manuals/API-AUDIT.md`.

## Candidate publication checks

The calculator runtime remains identical to commit `06f36d7`; this candidate
updates version metadata, manuals and release packaging/verification tools.
Current source hashes, test evidence, the fresh pristine-baseline comparison and
scoped device confirmations are recorded in `docs/review/release-evidence.json`.
The release tooling rejects mismatched firmware/source evidence and packages
versioned defaults only, excluding local saves and private resource files.

Publication requires a clean committed source tree, passing release verification,
checksummed assets, green CI on the release commit and a verified annotated tag.
The User Guide, Programming Guide and API reference contain 63, 84 and 71 pages,
respectively; all 218 pages were checked for version-only text changes and layout.


The completion pass is tracked in `docs/review/calculator-completion.json`. Hardware confirmations apply to the tested predecessor firmware hash. The rc.3 build updates version metadata and packaging; cold boot of that exact newly versioned image remains a candidate acceptance check.
