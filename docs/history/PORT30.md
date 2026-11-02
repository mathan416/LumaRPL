# LumaRPL port 30 — consolidated Ports 25–30

This release combines the memory/performance foundation, implicit graphs and optimization, advanced linear algebra, calculus and series, statistics and correlated uncertainty, signals/control systems, and connected studies. It builds on port 24 and preserves the calculator arena, bounded heap, reserved stack, RGB332 framebuffer and shared-workspace sizes. Deployment is a separate action.

Open **Ctrl+T → Math & graphs → More: complex / applied math → Advanced studies**. Implicit plots are also under **Math & graphs → Graphs → Implicit / contour plots**. Arrows/Enter or visible 1–8 shortcuts select items; F5/Esc returns one level. F4 Details explains the selected action. Results enter the calculator stack only through Copy.

## One saved study

The six new areas share a study containing equations, two independent 128-row X,Y tables, each table's unit declarations, a numerical matrix, transfer coefficients, measurements, settings, title and notes. **Connected study / data** copies observations from the existing Data / statistics workbench into A or B after confirmation. Sending a study table back also asks before replacement. These are snapshots: changing the external table does not silently change the study.

**Save / open / name / notes** stores an 8192-byte versioned, checksummed `.study` under `/db48x/studies/`. `DEMO.study` contains the initial examples. Loading validates before replacement and never evaluates formulas. Results are transient; select a calculation to rerun it. Saving uses temporary-file replacement and backup recovery, and a failed save retains both the previous valid file and unsaved inputs. Loader exit includes unsaved study protection. PSRAM is volatile; save before power-off. Without SD, calculations remain available.

## Implicit graphs and optimization

The initial equation `X^2+Y^2=25`, residual level 0 and window -8..8 draws a circle of radius 5. Alternatively enter the field `X^2+Y^2` and levels 1,4,9 for three circles. Equations are reduced to left-minus-right before applying levels.

This is a contour layer in the existing native renderer. Arrows probe X/Y; Enter shows the field and selected-level residual. The crosshair does not snap to a curve. F1 copies X then Y, F2 chooses the highlighted level, F3 exports SVG, F4 toggles Pan and +/- zoom. Viewport changes remain part of the study.

An 81×81 grid, two working scan rows, at most 8192 evaluations and 1536 cached segments keep memory bounded. Undefined cells and failed continuity probes leave gaps. Features smaller than the grid can be missed; this is not an exact geometric solver. Complexity, cancellation and storage failures reject incomplete plots.

For optimization, use `(X-1)^2+(Y+2)^2`, a box of -5..5 on each axis and initial (0,0). Local minimum should return approximately [1,-2,0]. This two-variable pattern search uses rectangular bounds, at most 400 iterations/4096 evaluations and an explicit step-resolution stopping test. It does not establish a global optimum. Arbitrary nonlinear constraints are outside this release.

## Advanced linear algebra

The study begins with `[[3,1],[1,3]]`. It has singular values 4,2, condition 2, symmetric eigenvalues 2,4 and inverse `[[0.375,-0.125],[-0.125,0.375]]`. Eigenvector columns correspond to the listed values; their signs can differ.

QR displays Q and R with A=QR. SVD displays U, singular values and V with A=U·diag(s)·Vᵀ. Pseudoinverse reports numerical rank and condition; singular values ≤ largest×1e-12 are discarded. Large condition numbers warn of sensitivity. Least squares uses the first N Y values of study B as b and reports the residual norm.

Edit a matrix in the existing Matrix workbench, then explicitly import A into the study. This numerical tool retains the current 1–6 square-matrix limit and real, dimensionless entries. Symmetric eigenvectors are supported; general nonsymmetric eigensystems are not. Existing exact and unit-bearing matrix operations remain available separately. Result screens can explicitly send a matrix back to Matrix A after dirty-document confirmation.

## Limits, series and improper integrals

- `SIN(X)/X`, point 0, both-sided numerical limit: approximately 1. `1/X` there should be unresolved.
- `EXP(X)`, point 0, Taylor degree 4: ascending coefficients `[1,1,0.5,1/6,1/24]`. Supported symbolic derivatives are evaluated numerically; unsupported derivatives fail explicitly. The local polynomial plot spans X0±1 and does not imply a remainder bound.
- `X^2`, first=1,last=3: numerical sum 14 and product 36. Indices are inclusive integers within ±1e6, with up to 1000 terms.
- `EXP(-X)`, first bound 0, interval to +infinity: approximately 1. Selected infinite intervals use transformed midpoint refinement, at most 4064 evaluations. Divergent, singular or oscillatory functions may fail or evade sampling; refinement change is not a rigorous bound.

These tools use radians without changing calculator settings or stored variables.

## Statistics and correlated uncertainty

Additional distributions provides uniform, exponential, Student t and chi-square CDFs, percentiles and plots. Degrees of freedom are capped at 10000. Student t with df=5 has CDF(0)=0.5. An exact two-sided probability-ordering binomial test with k=0,n=10,p=0.5 gives p=0.001953125. Trial count is at most 200; the test definition is exact but evaluation uses floating point.

Weighted line/quadratic fitting uses study A. For observations (0,1),(1,3),(2,5),(3,7), equal error scales give intercept 1 and slope 2. Relative error scales vary linearly by row, weights are 1/scale², and common variance is estimated from weighted residuals. Student t coefficient intervals assume independent Gaussian residuals and a correct model. The Welch test compares independent A/B Y samples, converting compatible B units into A units; unequal variances are allowed.

For uncertainty, use F=A*B, A=2_m±0.1_m, B=3_m±0.2_m and C=0±0. Correlation AB=0.5, AC=BC=0 gives nominal 6 m² and first-order standard uncertainty about 0.608276253 m². Correlations must form a positive-definite matrix; perfect correlation is outside this implementation. Monte Carlo uses 128–2048 correlated Gaussian draws and a saved nonzero 32-bit seed. Repeating the settings/seed repeats the draws. The central 95% percentile interval has simulation error and is not a fitted-parameter confidence interval. Invalid-domain draws stop the run rather than being dropped.

## Signals and control systems

Study A initially contains a unit-amplitude 16 Hz sine sampled at 128 Hz; B is a three-sample averaging kernel. Convolution computes the discrete sum without a sample-time multiplier. Cross-correlation computes sum A[i]·B[i+lag], without normalization. The complete output length A+B−1 must fit 128 rows. X coordinates use the declared rate; irregular observations are not resampled.

Welch PSD uses 32/64-sample Hann segments, mean removal and 50% overlap. Its one-sided density units are Y²/Hz, unlike the port-24 peak-amplitude FFT. The UI reports unused rows from a final incomplete segment. PSD can be plotted or exported as CSV.

The default continuous-time transfer function is H(s)=1/(s+1): numerator [1], denominator [1,1], highest power first. Pole=-1 s⁻¹; there are no finite zeros. At 1/(2π) Hz, gain is approximately -3.0103 dB and phase -45°. Bode X is log10(Hz). Principal phase wraps leave gaps instead of connecting across a jump.

The unit step reaches about 0.993262053 at 5 seconds; the impulse response is EXP(-t). Transfer degrees are at most three. Time responses require strictly proper functions and compare 1000/2000-step RK4 outputs; discrepancy above 1e-5×max(1, magnitude) rejects the result. This is an estimate, not a stability proof or stiff-system solver. Right-half-plane poles indicate instability.

## Units and resource foundation

Complex real/imaginary inputs accept compatible physical quantities: 3_m and 400_cm give magnitude 5_m. Bare components inherit a unit within that input. Products/quotients combine dimensions; roots take corresponding root units. Polar magnitudes can carry units and phases can be explicit angle quantities. Polar input, phase and roots use a scoped 16-digit numerical profile; exact rectangular arithmetic retains the calculator precision. Affine temperatures are unsupported; use kelvin. The existing `.complex` format is unchanged.

Geometry now saves one coordinate unit. Assign m to existing magnitudes after confirmation; entering 100_cm then means 1 m. Changing m to cm rescales points and window; removing units asks before retaining the bare magnitudes. Older geometry files load as dimensionless. Coordinate copies and distance context retain the unit.

**Memory / performance** reports current heap allocation, peak committed heap, stack watermark and shared-workspace peak, then benchmarks 100 expression evaluations and a verified 4 KiB PSRAM write/read. Committed heap includes allocator free blocks and is not peak live allocation. Device measurements include interrupt stack activity; host/model timings are not device measurements. The benchmark owns a temporary PSRAM slot and does not overwrite documents.

Numerical work uses exclusive bounded scratch leases. Busy foreground networking and insufficient heap headroom deny admission cleanly; silent background clock activity pauses and resumes around numerical ownership. Calculator arena, heap, stack, framebuffer and workspace reservations stay unchanged. No second framebuffer, multicore execution or new math runtime is introduced.

## Device acceptance after separate deployment

Test cold boot and both entry routes; the examples above; Copy with existing stack values; contour probing/pan/zoom/export/cancellation; matrix result transfer; seeded uncertainty repeatability; study capture/save/reopen/restart and failed/missing-SD paths; complex/geometry units; Lua/editor recovery; Wi-Fi/NTP/rates and cached conversion; loader return; and a 30-minute mixed-use session. Record Memory / performance measurements before and after the session. Host verification, visual review and physical acceptance are reported separately.
