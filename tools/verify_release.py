#!/usr/bin/env python3
"""Check current firmware and source evidence; physical acceptance stays separate."""
from pathlib import Path
import argparse,hashlib,json,subprocess
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--loader',type=Path);args=p.parse_args()
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
examples=json.loads((root/'reports/release-help-examples.json').read_text())
assert examples['examples'] and all(e['passed'] for e in examples['examples'])
comparison=json.loads((root/'reports/upstream-comparison.json').read_text())
assert comparison['same_test_count'] and not comparison['new_regressions'] and not comparison['new_assertion_families']
recorded=json.loads((root/'reports/release-controller-source.json').read_text())
for name,digest in recorded.items():assert sha(root/name)==digest,name
report={'version':(root/'VERSION').read_text().strip(),'memory':resources['memory'],'loader':loader,'help_examples':len(examples['examples']),'upstream':comparison,'device_verified':False,'scope':'Host evidence; full UI visual review and physical acceptance are separate.'}
(root/'reports/release-verification.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS: current source, original evaluator, memory, UF2, help examples and upstream comparison')
