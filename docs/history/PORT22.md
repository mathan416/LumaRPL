# LumaRPL port 22 — math workbenches

Port 22 adds five workbenches. Open Home with **Ctrl+T**. Use arrows and Enter, or the numbered visible rows. F5 / Esc goes back. Results stay separate from the calculator stack until you choose **Copy**. Opening or cancelling a tool preserves calculator variables, settings and existing stack values.

| Workbench | Home path |
|---|---|
| Vectors | Math & graphs → Matrices & vectors → Vectors |
| Probability | Math & graphs → Probability, or Data & notebooks → Probability |
| Inequalities | Math & graphs → Graphs → Inequalities |
| Multivariable calculus | Math & graphs → Calculus & algebra → Multivariable calculus |
| Symbolic algebra | Math & graphs → Calculus & algebra → Symbolic algebra |

## Vectors

Enter two real vectors, A and B, with two or three components. Components accept exact expressions such as `1/2` and `SQRT(2)`. Products / geometry offers dot and cross products, magnitude, endpoint distance, projection of A onto B and the angle in degrees. Arithmetic / scale offers addition, subtraction, multiplication/division by a real scalar and normalization.

Try A = [1,0,0], B = [0,1,0]: dot is 0, cross is [0,0,1], and the angle is 90°. In two dimensions, cross returns the signed Z component. A zero vector has no direction; projection onto a zero vector and division by zero report an error. Plot vectors draws both from the origin through the native path engine. Three-dimensional inputs offer XY, XZ and YZ projections rather than a perspective vector view.

## Probability

Choose normal, binomial or Poisson, then Parameters. Set Point X, Interval and Percentile p before opening Calculate. Normal uses mean and standard deviation; binomial uses integer trial count n and success probability; Poisson uses mean lambda.

For a normal distribution with mean 0 and standard deviation 1, P(X ≤ 0) is 0.5. The interval -1 to 1 is approximately 0.6826894921; percentile 0.95 is approximately 1.644853627. Density at a point is not a point probability. Discrete intervals include both endpoints and count integer outcomes only. Inverse CDF returns the smallest integer meeting the requested cumulative probability.

Plot with shaded interval uses the native graph engine. Normal shows a density; discrete distributions show unit-width probability-mass steps. Trace, zoom and graph export remain available. Numeric limits: binomial n ≤ 2000, Poisson lambda ≤ 1000; percentile must be strictly between 0 and 1. These are floating-point calculations, not symbolic probabilities.

## Inequalities

Configure up to three boundaries of the form Y compared with f(X), select ≥, >, ≤ or <, and Plot intersection. A disabled boundary does not constrain the shaded region. Strict boundaries are dashed. Undefined samples leave gaps instead of shading across them.

Try Y ≥ X² and Y ≤ X+2. The common region lies between the curves from X=-1 to X=2. Change the plot window to inspect it, trace the boundaries or export the plot. This release supports intersections of Y-versus-f(X) regions, not arbitrary implicit inequalities in two variables.

## Multivariable calculus

Enter F(X,Y), optionally G(X,Y), and a point. Calculate offers symbolic partial derivatives, gradient, the Jacobian of [F,G], the Hessian of F, numerical point evaluation and double integration. Jacobian rows correspond to F,G; columns to X,Y. Symbolic results retain their symbols; Approx in a result uses any stored calculator variables, rather than the workbench point.

Try F=X²+Y² and G=X*Y. Gradient is [2*X,2*Y], Jacobian is [[2*X,2*Y],[Y,X]], and Hessian is [[2,0],[0,2]]. At (1,1), F=2. Plot F and Plot tangent plane share the native 3D renderer. Tangent slopes use numerical differences and reject undefined points or inconsistent nearby slopes.

Integral / surface bounds keeps the integration rectangle separate from the surface X/Y window. Integrating X²+Y² over [0,1]×[0,1] gives approximately 2/3. Integration uses bounded composite Simpson refinement, at most 5,603 samples. The reported refinement difference is an estimate for smooth integrands, not a rigorous error bound. Singular, discontinuous or highly oscillatory functions may need smaller rectangles or a different method. Esc interrupts sampling.

## Symbolic algebra

Enter an expression, then choose Simplify, Expand, Collect / factor supported forms, Partial fractions or Substitute. Choose a substitution variable and replacement expression separately. Expand `(X+1)^2`, or substitute X=2 to obtain 9. Exact expressions use the upstream DB48X engine. Collect applies its supported rules; it does not promise general polynomial factorization.

Use result explicitly replaces the workbench expression after confirmation. Copy sends the result to the calculator stack. Results too large for the 159-byte formula editor remain copyable. Back leaves the expression alone.

## Saving and recovery

Each workbench has **Files / help**. Save writes one named `.mathlab` document containing inputs for all five workbenches, their angle mode and plotting/integration windows. Results are transient. Load asks before replacing dirty inputs. The workbench angle mode is independent of calculator settings.

Files live under `/db48x/mathlabs/`. `DEMO.mathlab` contains the examples above. Saves use temporary-file replacement and backup recovery with a versioned, bounded CRC32-checked format. A failed save preserves the previous valid file and leaves inputs dirty. Without SD, calculations and plots still work. Loader exit includes dirty math-workbench inputs in its save protection. PSRAM backing is volatile; save to SD before power-off.

## Guided device checks after deployment

1. Open each path above and return with Esc. Check menu labels, F4 Details, Help and all themes you use.
2. Run the examples in each section. Copy a result with existing stack values present; check those values remain below the new result.
3. Edit a vector component to `1/2`; check exact results. Try zero-vector normalization and projection onto a zero vector: both should explain the failure.
4. Check normal, binomial and Poisson plots, strict/inclusive inequality boundaries, tracing, zoom, export and Esc interruption. Check surfaces and tangent planes.
5. Save as TEST, change an input, then Load TEST. Cancel replacement once, then accept; the saved input should return. Save again, restart and reload. Confirm existing sessions and graph documents still open.
6. Recheck editor command insertion/error jumps from port 21.7, Lua/CHIP-8 demos, Wi-Fi/NTP/rates, loader return and a 30-minute mixed-use session.

Implementation, host tests and screenshot review are separate from physical acceptance. The release is built without writing to the SD card; deployment is a separate action.
