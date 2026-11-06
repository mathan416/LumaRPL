# Calculator completion review

These changes are local and unreleased. This document distinguishes implemented
calculator commands from inherited interfaces that remain unavailable. Numerical
and controller evidence is recorded in `docs/review/calculator-completion.json`.

## Implemented calculator commands

| Area | Commands | Contract |
| --- | --- | --- |
| Fourier transforms | `FFT`, `InvFFT` | Real or complex vectors; power-of-two lengths 1–1024, subject to memory. Forward uses the negative exponential without scaling; inverse divides by N. No automatic window or demeaning. |
| Probability | `Normal`, `Student`, `ChiSquare`, `FDistribution`, and their `Inv` commands | Lower-tail probabilities and finite quantiles. Inverse probabilities must be strictly between 0 and 1. Normal takes mean/variance/x; Student and chi-square take df/x; Fisher takes numerator-df/denominator-df/x. Arguments must be real, or remain symbolic. Small tails are evaluated directly during inversion. |
| Matrix factorizations | `MatrixQR`, `LQ`, `Cholesky`, `SVD`, `Schur` | Real matrices, dimensions up to 32 subject to memory. QR returns Q R; LQ returns L Q; Cholesky returns L; thin SVD returns U, singular-value vector, V; real Schur returns Q T. Real Schur eigenvalues give triangular blocks; complex conjugate pairs may give 2×2 blocks. |
| Matrix analysis | `SVL`, `CondNum`, `SpecRad`, `ToDiagonal`, `FromDiagonal` | Singular values, spectral condition number, spectral radius and diagonal conversions. Singular condition numbers report divide by zero. |
| Modular matrices | `RREFMOD`, `CASModulus` | Exact integer arithmetic over a prime field; default modulus 13. Negative and arbitrary-size integer entries are supported. Composite moduli are rejected. |
| Calculus | `DerivX`, `IntVX`, `Taylor`, `FiniteDifference`, `Limit` | DerivX/IntVX use the current algebra variable. Taylor takes expression/variable/point/order; FiniteDifference takes expression/variable/step. Taylor and analytic limits use radians while preserving the user's angle setting. |
| Finite limits | `Limit` | Finite analytic expressions and removable zero-over-zero quotients, with up to 64 symbolic differentiations. Discontinuous operators, infinite endpoints and general piecewise limits are rejected. |
| Trig rewrites | `HalfTan`, `TanToSinCos`, `TanSquaredToSinCos`, `TExpand`, `TLin`, `SinCos` | Half-angle, tangent, sum/difference/double-angle, product/square and complex exponential identities. These are bounded transformations, not a complete symbolic trigonometry system. |
| Exp/log rewrites | `ExpLn`, `Lin`, `LnCollect` | Expand/collect exponentials. Logarithm collection is limited to positive integer factors, preserving complex logarithm branches. |
| Lists | `Find` | First one-based matching list/vector index, or zero if absent. |
| Complex functions | `Gamma`, factorial, `Erf`, `Erfc`, `LnGamma` | Decimal evaluation with guard precision. Complex LnGamma is the principal logarithm of Gamma. Poles and unsupported precision demands report errors. |

The QR menu item runs `MatrixQR`, preserving existing `qr` variable names in RPL.
The ∆ menu item inserts `FiniteDifference`. The longer name preserves existing
`DELTA` directory and variable names. `Serie` is an alias for `Taylor`.
Existing command opcode values are unchanged; 42 commands and one setting are
appended. Normal binary-file compatibility checks still apply.

New named functions require parentheses inside algebraic expressions, for
example `'Normal(0;1;0)'`. A bare `qr` in an expression remains a variable,
including in unit-bearing heat-transfer equations.

## Clock reminders

The Alarms menu now schedules, lists, recalls, finds, acknowledges and deletes
one-time clock reminders. It uses the local clock, supports 32 reminders and
saves them in the calculator session. Alerts appear at idle boundaries;
powered-off devices do not wake for alarms.

## Existing commands now connected to menu entries

| Menu labels | Implementation |
| --- | --- |
| FRoots / FCoef | Existing `PRoot` / `PCoef`, also available by the new aliases. |
| Eqs / Indeps | Existing equation recall and variable extraction. |
| Rules | Replaced by the existing downward matching action. |
| IBF | Replaced by the existing partial-fraction action. |

## Retired empty menu entries

The PicoCalc menu builder omits nonempty `Unimplemented` actions. No hardware integration has been added.

| Unavailable interface | Supported alternative or status |
| --- | --- |
| IR printing, Print/Disk placeholders | Existing screen capture/export and file workflows. No IR transmitter integration. |
| Clock adjustment placeholder | Existing Time/Date/clock settings remain. |
| Legacy Save/Load/Open/Close/Read/Write/Seek/Dir menu placeholders | Existing calculator-session save/load, file browsers, RPL file storage and supported Lua APIs remain. No new handle-based RPL I/O API is provided. |
| Label/Goto/Gosub/Return placeholders | Existing structured RPL control flow and subprogram evaluation remain. No legacy branch-label language is added. |
| Arc placeholder | Existing line/circle/ellipse and native plotting tools remain. A standalone Arc command is not implemented. |
| Interval ↔ standard-deviation conversion | Deliberately rejected: bounds alone do not define a probability distribution, and standard deviation alone does not define a finite interval. |

## Regression fixes

The seven earlier failed checks are fixed by four changes: INPUT validators
receive literal text, printable keys respect error dismissal, failed inline
powers retain their source without leaving operands, and large-argument Gamma
retains the selected numerical accuracy.

Submenu Back also restores the parent's page and selected row. Tests cover the
keyboard path across all eight themes. The runtime sanitizer pass additionally
found and fixed decimal-digit alignment, desktop clock signed overflow,
short-constant-name overreads, and unsigned constant-cache pointer arithmetic.
The controller runner now treats engine assertions as failures even when the
process exits successfully. The older optional widget harness builds and runs
again with the production fonts and current caret geometry.

## Limits of release evidence

Host tests, a firmware build and UF2 validation do not establish a device cold
boot or SD-card acceptance. The tested firmware, help and manuals have since been copied and hash-verified
on the SD card at the user’s request; see `docs/review/alarms.json`. Source-control publication is tracked by Git; this verification and deployment
snapshot precedes the commit. No candidate tag or release has been created. The release-candidate version has not changed.
