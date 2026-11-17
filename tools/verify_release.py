#!/usr/bin/env python3
"""Check current firmware and source evidence; physical acceptance stays separate."""
from pathlib import Path
import argparse,os,hashlib,json,subprocess
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--loader',type=Path);p.add_argument('--evidence',type=Path,default=root/'docs/review/release-evidence.json');args=p.parse_args()
python_env=dict(os.environ)
python_env.pop("PYTHONOPTIMIZE",None)

manifest=json.loads((root/'patches/db48x-picocalc.json').read_text())
sha=lambda path:hashlib.sha256(path.read_bytes()).hexdigest()
def require(condition,message='Release check failed'):
    # Explicit checks keep the gate active under python -O / PYTHONOPTIMIZE.
    if not condition:raise SystemExit(message)
require(sha(root/'patches/db48x-picocalc.patch')==manifest['patch_sha256'],'Patch checksum mismatch')
for name,digest in manifest['files'].items():require(sha(root/'db48x'/name)==digest,name)
for name in ('research_engine.h','research_numeric.h'):
 require((root/'pico'/name).read_bytes()==(root/'tools/baselines/port301'/name).read_bytes(),name)
for generator in ('generate_ui_help.py','generate_command_reference.py','generate_reference_font.py','generate_editor_commands.py'):
 subprocess.run(['python3',str(root/'tools'/generator),'--check'],check=True,env=python_env)
subprocess.run(['python3',str(root/'tools/audit_ui.py')],check=True,env=python_env)
resources=json.loads(subprocess.check_output(['python3',str(root/'tools/check_release_resources.py')],text=True,env=python_env))
command=['python3',str(root/'tools/validate_uf2.py'),str(root/'build/pico/db48x-picocalc-pico2w.uf2')]
if args.loader:command+=['--loader',str(args.loader)]
loader=json.loads(subprocess.check_output(command,text=True,env=python_env))
require(args.evidence.is_file(),'Missing candidate verification evidence')
evidence=json.loads(args.evidence.read_text())
require(evidence['version']==(root/'VERSION').read_text().strip(),'Evidence is for another version')
require(evidence['firmware_sha256']==loader['sha256'],'Evidence is for another firmware build')
require(evidence['source_hashes'],'Missing current source bindings')
required={'VERSION','SOURCES.lock.json','patches/db48x-picocalc.patch','patches/db48x-picocalc.json'}
tracked=subprocess.check_output(['git','ls-files','-z'],cwd=root).decode().split('\0')
required.update(name for name in tracked if name.startswith(('pico/','resources/','tests/','tools/','docs/help/','docs/ui/','docs/manuals/')))
require(required<=set(evidence['source_hashes']),'Incomplete release source bindings')
for name,digest in evidence['source_hashes'].items():require(sha(root/name)==digest,name)
runtime_changes=subprocess.check_output(['git','diff',evidence['runtime_source_commit'],'--name-only','--','pico','resources','patches','tests'],cwd=root)
require(not runtime_changes.strip(),'Runtime changed after the recorded full suite')
checks={item['name']:item for item in evidence['checks']}
expected={'full_simulator','calculator_normal','calculator_sanitized','controller_normal','controller_sanitized','host','lua','arm_allocation','arm_tls','help_examples','manual_examples','manual_layout','release_tools'}
require(expected<=set(checks),'Missing required release checks')
for item in checks.values():
    require(item['passed'],item['name'])
    for record in item.get('evidence',[]):require(sha(root/record['path'])==record['sha256'],record['path'])
comparison=evidence['upstream_comparison']
for run in ('baseline','post'):
    require(comparison[run]['tests']>0,'Upstream comparison has no '+run+' tests')
    require(comparison[run]['failures']==0,'Upstream '+run+' run has failures')
require(comparison['post']['assertions']==0,'Upstream post run has assertions')
require(comparison['baseline']['assertions']==comparison['known_baseline_assertions'],'Unexpected baseline assertion count')
require(not comparison['known_baseline_assertions'] or comparison['baseline_assertion_reason'],'Missing baseline assertion reason')
delta=comparison['post']['tests']-comparison['baseline']['tests']
require(delta==comparison['expected_additional_tests'] and delta>=0,'Unexpected upstream test count change')
require(delta==0 or comparison['additional_test_reason'],'Missing additional test reason')
report={'version':evidence['version'],'memory':resources['memory'],'loader':loader,'checks':{name:check['passed'] for name,check in checks.items()},'upstream':comparison,'device_acceptance':evidence['device_acceptance'],'scope':'Candidate checks bound to current firmware and tracked source; confirmed predecessor hardware results remain scoped to their recorded hash.'}
(root/'reports/release-verification.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS: current source, original evaluator, memory, UF2, release checks and upstream comparison')
