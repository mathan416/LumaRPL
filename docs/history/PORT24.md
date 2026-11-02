# LumaRPL port 24

Port 24 adds units and dimensions, uncertainty propagation, interpolation and nonlinear fitting, discrete mathematics, and signal analysis. It builds on port 23. This release is not automatically copied to an SD card.

Open **Ctrl+T → Math & graphs → More: complex / applied math**. Interpolation and signal analysis are also under **Data & notebooks**. Arrows/Enter or 1–8 select a visible row; F5 or Esc returns to the immediate parent. Copy is always explicit.

## Units and dimensions

Choose **Units and dimensions → Evaluate / convert**. The initial expression `(12_V)/(4_ohm)` with target `A` gives 3 A. Parenthesize individual quantities in expressions, for example `(2_m)+(30_cm)`. DB48X checks dimensions; seconds cannot be converted into metres.

Quantity inputs now work in vector components, matrix cells and scalars, polynomial coefficients, declared ODE coordinates, normal-distribution measurements, engineering component values and statistical mean bounds. Engineering prompts accept values such as `10_kOhm` and `100_nF` and normalize them to SI. Vector components with bare numbers inherit the vector's declared/inferred unit. Incompatible components are rejected.

Polynomial **Units** declares X and each polynomial's output unit; coefficient magnitudes correspond to output/X^power. ODE **Units** declares X, Y and Z; derivatives must have compatible Y/X and Z/X dimensions. Bare numerical entries use the selected unit. Normal measurement units are under the distribution picker. Formula forms retain their existing named, unit-aware inputs.

Data units label the numerical X,Y columns. Set them explicitly after importing a new CSV; changing a label does not rescale the data. Curve-fit models and parameters use numerical magnitudes in those units. Algorithms requiring dimensionless inputs (probabilities, recurrence indices, combinatorics, transcendental arguments) still require them. The complex-number and geometry editors retain numerical coordinates; engineering impedance results carry ohms. This does not make every symbolic or advanced matrix operation valid for arbitrary mixed dimensions. Use kelvin for numerical temperature analysis; affine °C/°F is rejected there.

## Measurement uncertainty

Open **Measurement uncertainty**. The initial `A*B`, A=`2_m` ± `0.1_m`, B=`3_m` ± `0.2_m`, gives **6 m² ± 0.5 m²**. Choose Propagate, then F1 Copy values to put `[nominal, standard uncertainty]` on the stack. Output units `cm^2` give 60000 ± 5000 cm².

These are independent standard uncertainties, combined by first-order numerical derivatives. Contributions are shown separately. Correlated errors, confidence intervals and large nonlinear uncertainty are outside this method. Domain and detected nonsmooth cases report failure. A, B and C calculator variables are preserved.

## Interpolation and curve fitting

Create the exponential demo (confirm replacement of the current table), then Plot interpolation. Toggle linear/shape-preserving cubic interpolation and evaluate a point inside the sampled range. X must be strictly increasing and distinct; interpolation does not extrapolate.

Fit the default `A*EXP(B*X)+C`. The demo gives A=2, B=0.5, C=1. Inspect/copy coefficients, plot the fitted curve, then plot residuals. Initial guesses only change through the explicit Use coefficients action. This is unweighted local least squares with three parameters, bounded iterations and evaluations; a converged fit is not a guarantee of a globally best model. Singular models and invalid domains report failure.

## Discrete mathematics

The default A=12, B=18 gives GCD=6 and LCM=36. Integer calculations also include modulo, modular powers, prime factors, combinations and permutations. Inputs are unsigned decimal integers, not rounded measurements. General integer inputs are at most 1e12; factoring is limited to 1e9 and combinatorics to n≤1000. Esc interrupts factoring.

Generate the initial recurrence `U(N+1)=2*U+1`, U(0)=1: 1, 3, 7, 15… View/plot it, export CSV, or explicitly replace the current table. Up to 128 rows are supported; replacement asks first.

## Signal analysis

Create the sine demo (confirm table replacement). It has 32 samples at 128 Hz with a 16 Hz, unit-amplitude sine. With a rectangular window, the amplitude spectrum peaks at 16 Hz with amplitude 1. Phase at that bin is −90°. Hann/Hamming windows and optional mean removal are available; amplitudes compensate for window gain.

FFT uses 8, 16, 32, 64 or 128 Y samples, treated as uniformly sampled at the specified rate. X is not resampled. Infer sample rate checks increasing uniform X; declare seconds or a compatible time unit first. Frequency is Hz, amplitude uses the input Y unit, and weak-bin phases are omitted. This is a one-sided peak-amplitude spectrum, not a power spectral density. Do not interpret irregular observations as uniform samples.

Filters provide a centred moving average (shortened/renormalized at the ends), a one-pole low-pass, and input-minus-low-pass high-pass. The first low-pass value equals the first sample; the first high-pass value is zero. Set a positive cutoff below half the sample rate. Plot/export results before choosing Replace current table.

## Save, reopen and recovery

Files / help saves applied-math inputs as checksummed `.science` documents under `/db48x/science/`. The bundled DEMO.science contains initial examples. Save the current data table separately as CSV; the `.science` file stores settings and unit labels, not observations or transient results. Existing `.mathlab` and `.advanced` files remain readable and acquire optional unit fields when saved again. Saving uses temporary-file replacement and backup recovery. Without SD, calculation and plotting remain available.

## Device acceptance

Host verification and visual review are recorded separately in the release reports. On the PicoCalc, test the examples above; copy results, check prior stack items and variables, cancel calculations, save/reopen inputs and tables, and restart. Check cold boot, editor recovery, Wi-Fi/NTP, cached currency conversion, loader return and a 30-minute mixed-use session. Hardware display, responsiveness and SD behavior require device testing.
