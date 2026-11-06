#!/usr/bin/env python3
"""Check release resource privacy and rejection of stale release evidence."""
from pathlib import Path
import ast,json,os,shutil,subprocess,tempfile
root=Path(__file__).resolve().parents[1]
checks=[]
with tempfile.TemporaryDirectory(prefix='lumarpl-package-check-') as directory:
    repo=Path(directory)/'repo';repo.mkdir()
    subprocess.run(['git','init','-q',str(repo)],check=True)
    for name in ('resources/default.txt','resources/private-token.txt','state/default.48S','state/local-session.48S'):
        path=repo/name;path.parent.mkdir(parents=True,exist_ok=True);path.write_text(name)
    subprocess.run(['git','add','resources/default.txt','state/default.48S'],cwd=repo,check=True)
    tree=ast.parse((root/'tools/package_release.py').read_text())
    function=next(node for node in tree.body if isinstance(node,ast.FunctionDef) and node.name=='copy_tracked')
    namespace={'Path':Path,'subprocess':subprocess,'shutil':shutil}
    exec(compile(ast.Module(body=[function],type_ignores=[]),'copy_tracked','exec'),namespace)
    destination=Path(directory)/'payload'
    namespace['copy_tracked'](repo,('resources',),destination,Path('resources'))
    namespace['copy_tracked'](repo,('state',),destination)
    assert (destination/'default.txt').read_text()=='resources/default.txt'
    assert (destination/'state/default.48S').is_file()
    assert not (destination/'private-token.txt').exists()
    assert not (destination/'state/local-session.48S').exists()
    checks.append({'name':'Only tracked defaults are packaged; private resources and local saves are excluded','passed':True})
    missing=subprocess.run(['python3',str(root/'tools/verify_release.py'),'--evidence',str(Path(directory)/'missing.json')],cwd=root,capture_output=True,text=True,env=os.environ.copy())
    assert missing.returncode!=0 and 'Missing candidate verification evidence' in missing.stderr
    checks.append({'name':'Reject missing candidate evidence','passed':True})
    evidence=json.loads((root/'docs/review/release-evidence.json').read_text())
    for name,mutate,expected in [
        ('Reject evidence for a different firmware',lambda value:value.update(firmware_sha256='0'*64),'Evidence is for another firmware build'),
        ('Reject changed bound source',lambda value:value['source_hashes'].update(VERSION='0'*64),'VERSION')]:
        value=json.loads(json.dumps(evidence));mutate(value)
        path=Path(directory)/'evidence.json';path.write_text(json.dumps(value))
        result=subprocess.run(['python3',str(root/'tools/verify_release.py'),'--evidence',str(path)],cwd=root,capture_output=True,text=True,env=os.environ.copy())
        assert result.returncode!=0 and expected in result.stderr,(name,result.stdout,result.stderr)
        checks.append({'name':name,'passed':True})
(root/'reports/release-tool-tests.json').write_text(json.dumps(checks,indent=2)+'\n')
for check in checks:print('PASS:',check['name'])
