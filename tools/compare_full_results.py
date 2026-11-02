#!/usr/bin/env python3
from pathlib import Path
import json,re,sys
root=Path(__file__).resolve().parents[1]
if len(sys.argv)!=3:raise SystemExit('Usage: compare_full_results.py BASELINE.log POST.log')
(root/'reports').mkdir(exist_ok=True)
def read(name):
    text=Path(sys.argv[1 if name=='baseline' else 2]).read_text(errors='replace');text=re.sub(r'\x1b\[[0-9;]*m','',text)
    match=re.search(r'Ran (\d+) tests, (\d+) failures',text)
    assert match,f'{name} has not completed'
    tail=text[text.rfind('Summary of '):]
    failures=[{'test':i,'description':d} for i,d in re.findall(r'(?m)^.*tests\.cc:\d+:\s+(\d+:\d+\.\d+):\s+([^\n]+)',tail)]
    assertions=sorted(set(t for t in re.findall(r'assert_error: Assertion failed: ([^\n]+)',text) if not t.startswith('0x')))
    assert len(failures)==int(match[2]),failures
    return {'tests':int(match[1]),'failures':failures,'assertion_families':assertions}
a,b=read('baseline'),read('post');old={f['test'] for f in a['failures']}
regressions=[f for f in b['failures'] if f['test'] not in old]
report={'baseline':a,'post':b,'new_regressions':regressions,'same_failure_list':a['failures']==b['failures'],'same_test_count':a['tests']==b['tests'],'new_assertion_families':sorted(set(b['assertion_families'])-set(a['assertion_families']))}
(root/'reports/upstream-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
assert report['same_test_count'] and not regressions and not report['new_assertion_families'],report
print('Full suite comparison: no new failures/assertion families; baseline failures remain documented.')
