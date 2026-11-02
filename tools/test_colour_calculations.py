#!/usr/bin/env python3
"""Check the real colour simulator with the native calculator arena size."""
import json, subprocess, os, argparse
from pathlib import Path
root=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser();parser.add_argument('--indexed',action='store_true');args=parser.parse_args()
binary=root/('build/simulator-rgb332/db48x.app/Contents/MacOS/db48x' if args.indexed else 'build/simulator-colour/db48x.app/Contents/MacOS/db48x')
cases=[
('Arithmetic','2 3 +','5'),
('Exact fraction','1 3 /','¹/₃'),
('Complex','(1;2) (3;4) +','4+6ⅈ'),
('Matrix','[[1 2][3 4]] DET','-2'),
('Scientific','90 SIN','1'),
('Precision','0.1 0.2 +','0.3'),
('RPL program','« 2 3 + » EVAL','5'),
('HOME program',"« DUP * » 'SQDEMO' STO 12 SQDEMO",'144'),
('Saved source program','« DUP * » "colour-test.48s" STO 7 "colour-test.48s" RCL EVAL','49'),
('Graph equation',"'X^2' 'EQ' STO 'X' INDEP -10 10 XRNG -6 6 YRNG CLLCD DrawAxes DRAW",''),
('Colour setting','0 0 1 RGB Foreground','')]
results=[]
for name,source,expected in cases:
 r=subprocess.run([str(binary),'-H','-Z','-m208','-E',source],cwd=Path(os.environ.get('PICOCALC_TEST_WORKDIR',str(root/'db48x'))),capture_output=True,text=True,timeout=60)
 passed=r.returncode==0 and r.stdout.strip()==expected and not r.stderr.strip()
 results.append(dict(case=name,source=source,expected=expected,returncode=r.returncode,stdout=r.stdout.strip(),stderr=r.stderr.strip(),passed=passed,colour=True,arena_kib=208))
 print(('PASS' if passed else 'FAIL'),name)
(root/('reports/port18-rgb332-calculations.json' if args.indexed else 'reports/colour-calculation-tests.json')).write_text(json.dumps(dict(binary=str(binary),tests=results),indent=2)+'\n')
raise SystemExit(not all(r['passed'] for r in results))
