#!/usr/bin/env python3
"""Exercise complex Gamma in the real engine with the 208 KiB device arena."""
import cmath
import json
import subprocess
from pathlib import Path

root = Path(__file__).resolve().parents[1]
binary = root / 'build/simulator-rgb332/db48x.app/Contents/MacOS/db48x'
# Independent double-precision Lanczos reference; production uses decimal Spouge.
coefficients = (676.5203681218851, -1259.1392167224028, 771.32342877765313,
                -176.61502916214059, 12.507343278686905, -.13857109526572012,
                9.9843695780195716e-6, 1.5056327351493116e-7)
def gamma(z):
    if z.real < .5:
        return cmath.pi / (cmath.sin(cmath.pi*z)*gamma(1-z))
    z -= 1
    total = .99999999999980993
    for k, c in enumerate(coefficients, 1):
        total += c / (z+k)
    t = z+7.5
    return cmath.sqrt(2*cmath.pi)*t**(z+.5)*cmath.exp(-t)*total

cases = []
def check(name, source, expected='True', error=None):
    result = subprocess.run([str(binary), '-H', '-Z', '-m208', '-E', source],
                            cwd=root/'db48x', capture_output=True, text=True, timeout=60)
    passed = result.stdout.strip() == expected and not result.stderr.strip()
    if error:
        passed = error in result.stderr
    cases.append(dict(name=name, source=source, stdout=result.stdout.strip(),
                      stderr=result.stderr.strip(), passed=passed))
    print(('PASS' if passed else 'FAIL'), name, flush=True)

for z in (1j, 1+1j, .5+.5j, -2+3j, -2-3j, -10+.01j, 12+15j, .1+40j):
    expected = gamma(z)
    literal = f'({z.real};{z.imag})'
    check(f'Gamma {z}', f'24 Precision {literal} Gamma ({expected.real:.16g};{expected.imag:.16g}) - ABS {abs(expected)*2e-12:.16g} <')
check('Imaginary factorial ASCII entry', '(0;1)! (0.498015668118356042713691117462;-0.154949828301810685124955130484) - ABS 1e-23 <')
check('Imaginary unit factorial', 'ⅈ! (0;1)! - ABS 1e-23 <')
check('Imaginary unit named factorial', 'ⅈ Factorial (0;1)! - ABS 1e-23 <')
check('Assigned i variable still resolves', "3 'i' STO i Factorial", '6')
check('Local i loop variable still resolves', '0 1 3 FOR i i + NEXT', '6')
check('Repeated Gamma survives collection', '1 30 START ⅈ! DROP NEXT 1', '1')
check('Commands containing i still parse', '90 SIN', '1')
check('Named factorial entry', '(0;1) Factorial (1;1) Gamma - ABS 1e-23 <')
for precision in (16, 34, 50):
    tolerance = f'1e-{precision-3}'
    prefix = f'Rad {precision} Precision '
    check(f'{precision} digit Gamma recurrence', prefix+f'(2.3;4.1) Gamma (1.3;4.1) Gamma (1.3;4.1) * / 1 - ABS {tolerance} <')
    # Independent analytic identity |Gamma(1+i)|^2 = pi/sinh(pi).
    check(f'{precision} digit analytic modulus', prefix+f'(0;1)! ABS SQ π ToDecimal DUP SINH / - ABS {tolerance} <')
check('Angle mode and precision preserved', 'Deg 24 Precision (0;1)! DROP 90 SIN 1 - ABS 1e-20 <')
check('Polar argument', 'Rad (0;1) ToPolar Gamma (0;1) Gamma - ABS 1e-22 <')
check('Conjugate symmetry', '(-2;3) Gamma CONJ (-2;-3) Gamma - ABS 1e-23 <')
check('Tiny imaginary offset near a pole', '24 Precision (-1;1e-50) Gamma (0;1e50) / 1 - ABS 1e-20 <')
for n in (0, -1, -5):
    check(f'Pole {n}', f'({n};0) Gamma', error='Argument outside domain')
check('Ordinary real factorial unchanged', '5 Factorial', '120')
(root/'reports/complex-gamma-tests.json').write_text(json.dumps(cases, indent=2)+'\n')
raise SystemExit(not all(case['passed'] for case in cases))
