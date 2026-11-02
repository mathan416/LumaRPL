#!/usr/bin/env python3
"""Execute the help's console examples using the real DB48X engine."""
from pathlib import Path
import argparse,hashlib,json,os,shutil,subprocess,tempfile
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--release',default='release');p.add_argument('--binary',type=Path,default=root/'build/simulator-rgb332/help-console');args=p.parse_args();binary=args.binary.resolve()
if args.binary == root/'build/simulator-rgb332/help-console':
 out=root/'build/simulator-rgb332'
 source=(out/'ControllerProberelease.mk').read_text().replace('controller-probe-release:', 'help-console:').replace('controller-probe-main.o','help-main.o').replace('controller-probe-main.cc','help-main.cc')
 (out/'help-main.cc').write_text((root/'db48x/src/dmcp/main.cc').read_text())
 (out/'HelpConsole.mk').write_text(source)
 subprocess.run(['make','-C',str(out),'-f','HelpConsole.mk','help-console'],check=True)
examples=json.loads((root/'docs/help/examples.json').read_text());examples += [dict(title=name,**e) for name,e in json.loads((root/'docs/help/command-examples.json').read_text()).items()];results=[]
env=dict(os.environ,QT_QPA_PLATFORM='offscreen')
with tempfile.TemporaryDirectory(prefix='lumarpl-help-') as work:
 shutil.copytree(root/'db48x/config',Path(work)/'config')
 for e in examples:
  for item in Path(work).iterdir():
   if item.is_dir():shutil.rmtree(item)
   else:item.unlink()
  shutil.copytree(root/'db48x/config',Path(work)/'config')
  source=Path(work)/'example.48s';source.write_text(e['source'])
  r=subprocess.run([str(binary),'-H','-Z','-m208','-F',str(source)],cwd=work,env=env,capture_output=True,text=True,timeout=30)
  ok=r.returncode==0 and r.stdout.strip()==e['expected'] and not r.stderr.strip()
  results.append(dict(**e,returncode=r.returncode,actual=r.stdout.strip(),stderr=r.stderr.strip(),passed=ok))
  print(('PASS: ' if ok else 'FAIL: ')+e['title'],flush=True)
report=dict(binary=str(binary),binary_sha256=hashlib.sha256(binary.read_bytes()).hexdigest(),scope='Typed examples in the shipped RPL help executed with a fresh stack/config per example. Output and expected stack order are compared exactly; UI-only actions are checked separately.',examples=results)
(root/'reports'/(args.release+'-help-examples.json')).write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n')
raise SystemExit(not all(e['passed'] for e in results))
