#!/usr/bin/env python3
"""Check current firmware and source evidence; physical acceptance stays separate."""
from pathlib import Path
import argparse,hashlib,json,subprocess
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--loader',type=Path);p.add_argument('--evidence',type=Path,default=root/'docs/review/release-evidence.json');args=p.parse_args()
manifest=json.loads((root/'patches/db48x-picocalc.json').read_text())
sha=lambda path:hashlib.sha256(path.read_bytes()).hexdigest()
assert sha(root/'patches/db48x-picocalc.patch')==manifest['patch_sha256']
for name,digest in manifest['files'].items():assert sha(root/'db48x'/name)==digest,name
for name in ('research_engine.h','research_numeric.h'):
 assert (root/'pico'/name).read_bytes()==(root/'tools/baselines/port301'/name).read_bytes(),name
for generator in ('generate_ui_help.py','generate_command_reference.py','generate_reference_font.py','generate_editor_commands.py'):
 subprocess.run(['python3',str(root/'tools'/generator),'--check'],check=True)
subprocess.run(['python3',str(root/'tools/audit_ui.py')],check=True)
resources=json.loads(subprocess.check_output(['python3',str(root/'tools/check_release_resources.py')],text=True))
command=['python3',str(root/'tools/validate_uf2.py'),str(root/'build/pico/db48x-picocalc-pico2w.uf2')]
if args.loader:command+=['--loader',str(args.loader)]
loader=json.loads(subprocess.check_output(command,text=True))
assert args.evidence.is_file(), 'Missing candidate verification evidence'
evidence=json.loads(args.evidence.read_text())
assert evidence['version']==(root/'VERSION').read_text().strip()
assert evidence['firmware_sha256']==loader['sha256'], 'Evidence is for another firmware build'
assert evidence['source_hashes'], 'Missing current source bindings'
required={'VERSION','SOURCES.lock.json','patches/db48x-picocalc.patch','patches/db48x-picocalc.json'}
tracked=subprocess.check_output(['git','ls-files','-z'],cwd=root).decode().split('\0')
required.update(name for name in tracked if name.startswith(('pico/','resources/','tests/','tools/','docs/help/','docs/ui/','docs/manuals/')))
assert required<=set(evidence['source_hashes']), 'Incomplete release source bindings'
for name,digest in evidence['source_hashes'].items():assert sha(root/name)==digest,name
runtime_changes=subprocess.check_output(['git','diff',evidence['runtime_source_commit'],'--name-only','--','pico','resources','patches','tests'],cwd=root)
assert not runtime_changes.strip(), 'Runtime changed after the recorded full suite'
checks={item['name']:item for item in evidence['checks']}
expected={'full_simulator','calculator_normal','calculator_sanitized','controller_normal','controller_sanitized','host','lua','arm_allocation','arm_tls','help_examples','manual_examples','manual_layout','release_tools'}
assert expected<=set(checks), 'Missing required release checks'
for item in checks.values():
    assert item['passed'], item['name']
    for record in item.get('evidence',[]):assert sha(root/record['path'])==record['sha256'],record['path']
comparison=evidence['upstream_comparison']
for run in ('baseline','post'):
    assert comparison[run]['tests']>0
    assert comparison[run]['failures']==0
assert comparison['post']['assertions']==0
assert comparison['baseline']['assertions']==comparison['known_baseline_assertions']
assert not comparison['known_baseline_assertions'] or comparison['baseline_assertion_reason']
delta=comparison['post']['tests']-comparison['baseline']['tests']
assert delta==comparison['expected_additional_tests'] and delta>=0
assert delta==0 or comparison['additional_test_reason']
report={'version':evidence['version'],'memory':resources['memory'],'loader':loader,'checks':{name:check['passed'] for name,check in checks.items()},'upstream':comparison,'device_acceptance':evidence['device_acceptance'],'scope':'Candidate checks bound to current firmware and tracked source; confirmed predecessor hardware results remain scoped to their recorded hash.'}
(root/'reports/release-verification.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS: current source, original evaluator, memory, UF2, release checks and upstream comparison')
