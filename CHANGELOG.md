# Changelog

## Unreleased

## 0.10.0-rc.3 (2026-11-06)

- Complete calculator Fourier, probability, real-matrix and bounded symbolic commands; connect working menu actions and hide unavailable legacy interfaces.
- Add one-time local-clock reminders with visual alerts, acknowledgement/deletion and saved-session persistence.
- Restore parent menu pages and selected rows when returning from a submenu.
- Preserve literal INPUT text and error dismissal, restore failed inline powers without extra operands, and improve large real/complex Gamma accuracy.
- Correct decimal alignment, constant-name lookup, constant-cache arithmetic and desktop tick overflow found by runtime sanitizers.
- Refresh calculator help, command insertion, manuals and release evidence. The patched simulator passes 8,507 tests; normal and sanitized calculator contracts pass 135 checks each.

- Make the first minus key subtract immediately when two stack values are ready.

- Support complex Gamma and factorial, including native imaginary-unit entry.
- Add exact Termial sums through `?`, `Termial`, and the Probability menu.
- Make numeric factorial/Termial and RPN power keys calculate immediately, with cursor-safe editing.
- Evaluate parenthesized calculator entries and retain original formulas after errors.
- Use partial redraws for formula cursor movement and add Ctrl+O for the Math menu.

- Add an API & Assembly companion reference for Lua and CHIP-8/SUPER-CHIP, with downloadable and host-tested routines.

## 0.10.0-rc.2

- Add illustrated User and Programming Guides, tested language examples and PDF build tools.
- Expand public installation and development instructions.
- Correct help wording for direct integer base conversion, CSV storage and calculus navigation.
- Complete font credits and package component notices under `/db48x/licenses/`.
- Retain the original evaluator, mathematics, memory reservations and document formats.

## 0.10.0-rc.1

- Prepare a standalone LumaRPL source repository from port 30.5.
- Capture all 21 upstream source changes in a checksummed patch.
- Pin dependency and submodule revisions and verify patched source hashes.
- Provide current build, contribution, release and packaging instructions.
- Retain the original evaluator, calculator memory reservations and saved formats.

Earlier port notes are archived in docs/history. Candidate verification and
physical acceptance are recorded in docs/RELEASE-NEXT.md and the versioned evidence under docs/review/.
