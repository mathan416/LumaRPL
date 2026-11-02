# LumaRPL port 23 — mathematical tools and predictable Back

This release adds guided polynomial solving, coupled/second-order differential equations, an engineering workbench and inference from data. It also improves source-command insertion and keeps calculation submenus open when you close a result.

Open Home with Ctrl+T. Use arrows/Enter or visible row numbers. Esc/F5 goes back one level. For example: Angle result → Vector calculations → Vector workbench. The selected calculation stays selected. The same behavior applies to probability, multivariable, algebra, complex, matrix and extended finance operation menus. Value selectors still close when you make a choice; a graph control such as Show tangent returns to the graph so you can see the change.

## Command insertion

In a source editor, choose F4 Tools → Insert command. Lua calls include their parentheses, quotes and arguments. The first argument is selected for replacement; an empty string or empty call puts the caret inside its delimiters. CHIP-8 inserts a complete instruction line and selects its first operand. Type to replace the selection. Ctrl+Z undoes insertion without removing earlier undo history; Redo restores the insertion and its argument cursor position.

RPL source commands retain their stack syntax: `SIN` consumes its argument from the stack, so it is not changed into a Lua-style call. The formula editor already provides `SIN()` and other algebraic templates with the caret inside the parentheses. No command runs just because you insert it.

## Guided polynomials

Home → Math & graphs → Calculus & algebra → Guided polynomials.

Enter degree 1–8, followed by real coefficients from highest power down to constant. For A=X²−1, enter degree 2 and coefficients 1, 0, -1. For B=X−1, enter degree 1 and 1, -1. A leading zero is rejected; cancelling entry keeps the previous polynomial.

Calculate / solve offers the expression, real/complex roots, real-point evaluation, multiplication, quotient and remainder. This example has roots -1 and 1, A(3)=8, quotient X+1 and remainder 0. For X²+1, roots are imaginary. Repeated entries represent multiplicity. Exact inputs use DB48X arithmetic; general roots may be numerical or fail to converge. Plot A shares native tracing, analysis and export; expressions over the existing 159-byte graph limit cannot be plotted.

## Expanded differential equations

Home → Math & graphs → Graphs → Differential equations → Systems / second order.

Coupled mode accepts Y'=F(X,Y,Z), Z'=G(X,Y,Z). Second-order mode accepts Y''=G(X,Y,Z), with Z representing Y'; the solver supplies Y'=Z. Trigonometric expressions use radians without changing calculator angle settings.

Try Y'=Z and Z'=-Y, with initial X=0, Y=1, Z=0, final X=6.283185307179586. Y follows COS(X) and Z follows -SIN(X). The phase portrait should be a circle. Second-order mode with G=-Y gives the same solution.

Plots use 201 numerical solution samples; the table shows every tenth sample. Native tracing, zoom and export are available. Adaptive RK4 step doubling uses a 1e-7 absolute/relative scale and at most 8192 right-hand-side evaluations. This is intended for non-stiff initial-value problems. Singular/stiff cases can exhaust the bound; a failed solve is not presented as a complete solution. Esc interrupts sampling. Initial/final X must be distinct and within +/-1e6; backward integration is supported.

## Engineering

Home → Math & graphs → Finance / engineering → Engineering: RLC / RC filters.

Enter R in ohms, L in henries, C in farads and f in hertz. Use 1e-6 for one microfarad. Series and parallel RLC calculations show complex impedance. Components are ideal; a zero resistor or zero inductor shorts the parallel network.

RC responses use R and C, with low-pass output across C and high-pass output across R. Inductance is not used. For R=1000, C=1e-6 and f=159.1549431, both gains are approximately 0.70710678 (-3.0103 dB); low-pass phase is -45°, high-pass +45°. RC responses require positive R.

Bode magnitude and phase plots share the native renderer. Their horizontal coordinate is log10(frequency in Hz): X=3 means 1000 Hz. Sweep limits are entered in hertz. This is an ideal circuit workbench, not a general circuit simulator.

## Probability and statistics

Home → Data & notebooks → Inference / distributions. Also available from Probability → Calculate probability / quantile → Data inference / fit distribution.

Enter or import a table in Data / statistics first. Select X, Y or paired differences X-Y. Mean inference requires at least two observations and nonzero sample variance. Confidence is a fraction such as 0.95; set null mean/proportion separately.

For X=1,2,3,4,5, sample mean=3 and SD=SQRT(2.5). The 95% Student t interval is approximately [1.03676,4.96324]. With null mean 3, the two-sided t statistic is 0 and p=1. Paired differences give a paired t test.

Mean inference assumes independent observations and an approximately normal population, or an adequate large-sample approximation. Paired mode assumes independent pairs and approximately normal differences. A p-value is not the probability that the null hypothesis is true.

Fit normal explicitly updates the Probability workbench with the sample mean and sample SD. Return to Probability and use its density/CDF/interval/quantile calculations and shaded plot immediately. Fitting does not establish normality. The original table is preserved.

Proportion inference accepts successes and trials. It gives a Wilson score interval and an approximate two-sided normal score test against the null proportion. The test needs adequate expected successes/failures; small samples need an exact method, which this release does not add.

## Saving and device testing

Files / help in any advanced tool saves a single checked `.advanced` file under `/db48x/advanced/`, containing all four input sets and inference settings. The data table is saved separately as CSV. Results and ODE samples are transient. Temporary-file replacement and backup recovery preserve the previous valid file after a failed save. Unsaved advanced inputs are included in loader-exit protection. Existing saved formats remain compatible.

After deployment, test:

1. Insert a Lua call and a CHIP-8 instruction; type over the selected argument, undo twice and redo. Check formula templates put the caret inside parentheses. Confirm ordinary RPL commands still run with stack arguments.
2. Open Vector calculations → Angle, close the result with F5, and check the same row remains selected. Repeat in Probability calculations, complex, matrix, algebra and extended finance operations.
3. Run the polynomial, oscillator, RC and Student t examples above. Check both graph modes, tables, trace and export.
4. Fit a normal distribution from the table and use it immediately in Probability. Check paired differences and a proportion interval; inspect assumptions in Help.
5. Save advanced inputs as TEST, change an input, load TEST, then restart and reload. Check cancellation and loader-exit save protection.
6. Recheck existing RPL/Lua/CHIP-8 programs, Wi-Fi/NTP/currency, and a 30-minute mixed-use session.

Host testing and visual review are reported separately from physical acceptance. Deployment is a separate requested action.
