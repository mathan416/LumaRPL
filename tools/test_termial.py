#!/usr/bin/env python3
"""Exercise Termial in the actual engine with the PicoCalc arena size."""
from pathlib import Path
import json, subprocess
root=Path(__file__).resolve().parents[1]
binary=root/'build/simulator-rgb332/db48x.app/Contents/MacOS/db48x'
cases=[]
def check(name,source,expected=None,error=None):
 r=subprocess.run([str(binary),'-H','-Z','-m208','-E','0 MantissaSpacing '+source],cwd=root/'db48x',capture_output=True,text=True,timeout=30)
 passed=(not r.stderr.strip() and r.stdout.strip()==expected) if error is None else error in r.stderr and 'Assertion failed' not in r.stderr
 cases.append(dict(name=name,source=source,stdout=r.stdout.strip(),stderr=r.stderr.strip(),passed=passed))
 print(('PASS' if passed else 'FAIL'),name,flush=True)
for n in (0,1,2,6,10,100,1000000,10**50+1):
 for spelling in ('?', ' Termial'):
  check(str(n)+spelling,str(n)+spelling,str(n*(n+1)//2))
check('whole decimal','6.0?','21')
check('chained postfix','6??','231')
check('factorial then Termial','4!?','300')
check('Termial then factorial','4?!','3628800')
check('parenthesized expression',"'(2+4)?' ToDecimal",'21')
check('named algebraic expression',"'Termial(6)' ToDecimal",'21')
check('postfix symbolic variable',"4 'TTN' STO 'TTN?' ToDecimal",'10')
check('symbolic rendering',"'Termial(N)'", "'N?'")
check('addition precedence',"'6?+1' ToDecimal",'22')
check('power after Termial',"'2?^3' ToDecimal",'27')
check('power before postfix',"'2^3?' ToDecimal",'64')
check('large based integer normalized','#FFFFFFFFh Termial',str((2**32-1)*2**32//2))
check('existing undefined constant','Ⓒ?','?')
check('question-mark RPL variable',"9 'ⓋREADY?' STO READY?",'9')
check('escaped question name round trip',"'ⓋREADY?'","'ⓋREADY?'")
check('question name text compile round trip',"9 'ⓋREADY?' STO 'ⓋREADY?' ToText Compile RCL",'9')
check('question name GC round trips',"9 'ⓋREADY?' STO 1 2000 START 'ⓋREADY?' ToText Compile RCL DROP NEXT 1",'1')
check('separated variable operator',"6 'N' STO N ?",'21')
for source in ('-1?', '2.5?', '1 2 / Termial'):
 check('domain '+source,source,error='Argument outside domain')
check('complex count rejected','(1;1) Termial',error='Bad argument type')
(root/'reports/termial-tests.json').write_text(json.dumps(cases,indent=2)+'\n')
raise SystemExit(not all(c['passed'] for c in cases))
