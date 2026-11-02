#!/usr/bin/env python3
"""Real optimized-engine regression checks for conditional/local return frames."""
from pathlib import Path
import argparse
import hashlib
import json
import re
import shutil
import subprocess

root = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--binary', type=Path, default=root/'build/simulator-rgb332/db48x.app/Contents/MacOS/db48x')
parser.add_argument('--tag', default='port198-rpl-frames')
args = parser.parse_args()
binary = args.binary.resolve()
work = root/'build'/args.tag
if work.exists():
    shutil.rmtree(work)
work.mkdir()
for folder in ('config', 'help', 'library'):
    shutil.copytree(root/'db48x'/folder, work/folder)
for folder in ('state', 'data'):
    (work/folder).mkdir()
cases = [
    ('CASE first true', '« CASE 1 THEN 42 END 99 END » EVAL', '42'),
    ('CASE default', '« CASE 0 THEN 42 END 99 END » EVAL', '99'),
    ('CASE second true', '« CASE 0 THEN 42 END 1 THEN 7 END 99 END » EVAL', '7'),
    ('CASE no match', '« CASE 0 THEN 42 END END DEPTH » EVAL', '0'),
    ('CASE nested', '« CASE 1 THEN CASE 1 THEN 42 END 99 END END 100 END » EVAL', '42'),
    ('DoSubs CASE locals', '{ 1 2 3 4 5 } « → a b « CASE \'NSUB=1\' THEN a END \'NSUB=ENDSUB\' THEN b END \'a+b\' EVAL END » » DOSUBS', '{ 1 5 7 5 }'),
    ('Local cleanup', "7 'x' STO « 42 → x « CASE 1 THEN x END 99 END » » EVAL DROP x", '7'),
    ('IFT empty branch', '123 0 « 456 » IFT', '123'),
    ('IFTE false', '0 « 42 » « 99 » IFTE', '99'),
    ('Recursive frame exhaustion recovery', "32 MaxCallRecursion « P 1 + » 'P' STO IFERR P THEN 1024 MaxCallRecursion 2 3 + END", '5'),
    ('Conditional frame exhaustion recovery', "32 MaxCallRecursion « IF 1 THEN P END 1 + » 'P' STO IFERR P THEN 1024 MaxCallRecursion 2 3 + END", '5'),
    ('Counted loop locals', '« 0 1 100 START 1 + NEXT » EVAL', '100'),
]
results = []
for name, source, expected in cases:
    result = subprocess.run([str(binary), '-H', '-Z', '-m208', '-F', '-'],
                            cwd=work, input=source.encode(), capture_output=True, timeout=60)
    output = result.stdout.decode(errors='replace').strip()
    errors = result.stderr.decode(errors='replace').strip()
    expected_error = 'frame exhaustion recovery' in name
    clean_errors = not errors if not expected_error else bool(re.fullmatch(
        r'\[\d+ [\d.]+\] errors: Error \[Too many recursive calls\]', errors))
    passed = result.returncode == 0 and output == expected and clean_errors
    results.append(dict(name=name, source=source, expected=expected,
                        output=output, stderr=errors, returncode=result.returncode,
                        passed=passed))
    print(('PASS: ' if passed else 'FAIL: ') + name)
report = dict(binary=str(binary), binary_sha256=hashlib.sha256(binary.read_bytes()).hexdigest(),
              arena_kib=208, passed=all(r['passed'] for r in results), cases=results)
(root/'reports'/f'{args.tag}.json').write_text(json.dumps(report, indent=2)+'\n')
raise SystemExit(not report['passed'])
