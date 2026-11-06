# Next release-candidate readiness

Review date: 2026-11-05. VERSION remains 0.10.0-rc.2; these changes are Unreleased. No new tag, GitHub release or candidate bundle is created by committing this update.

## Changes under review

Complex Gamma/factorial; exact Termial; native constants and signed entry; Math-menu shortcut; immediate postfix and RPN power operations; grouped formula evaluation; partial cursor redraw; cursor-safe operators; and original-formula error recovery. The API & Assembly companion adds a reviewed 71-page PDF and nine maintained examples.

## Available evidence

- Calculator keyboard/controller checks pass in all eight themes, including address and undefined-behaviour sanitizers. Grouped expressions, classic complex literals, operator entry, error repair and pixel-equivalent partial redraw are covered.
- Target firmware builds and passes RP2350 UF2/loader/partition validation. See `docs/review/calculator-update.json` for the tested firmware hash and scope.
- Existing command IDs and saved formats are retained; Termial is appended as a new opcode.
- Generated help/reference/insertion freshness, upstream patch hashes and PDF checksums are checked before commit.
- All 135 manual examples pass: 29 RPL, 55 Lua and 51 CHIP-8, plus production assembler/VM routine contracts. Physical execution of the new examples remains separate. See `docs/manuals/API-AUDIT.md`.

## Before tagging the next candidate

1. Require passing CI on the pushed commit and run the complete simulator suite from a clean patched checkout. Prior full-suite results and the corrected catalogue expectation are recorded in `docs/review/calculator-update.json`.
2. Regenerate and visually review the User Guide PDF: its Markdown source now describes calculator-entry changes newer than the checked-in rc.2 PDF.
3. Choose the next candidate version, rebuild from its exact commit, and produce a checksummed candidate bundle with corresponding source.
4. Cold boot that exact firmware and complete the device and mixed-use checks in `docs/RELEASE-CHECKLIST.md`, including cursor editing, error repair and save/restart.

The implementation is suitable for next-candidate testing. Final release acceptance is still pending; SD copying does not establish device acceptance.
