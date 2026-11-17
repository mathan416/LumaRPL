#!/usr/bin/env python3
"""Check release resource privacy and rejection of stale release evidence."""
from pathlib import Path
import ast,io,json,os,shutil,subprocess,tarfile,tempfile
root=Path(__file__).resolve().parents[1]
checks=[]
with tempfile.TemporaryDirectory(prefix='lumarpl-package-check-') as directory:
    repo=Path(directory)/'repo';repo.mkdir()
    subprocess.run(['git','init','-q',str(repo)],check=True)
    for name in ('resources/default.txt','resources/private-token.txt','state/default.48S','state/local-session.48S'):
        path=repo/name;path.parent.mkdir(parents=True,exist_ok=True);path.write_text(name)
    (repo/'state/patched.48S').write_text('upstream')
    subprocess.run(['git','add','resources/default.txt','state/default.48S','state/patched.48S'],cwd=repo,check=True)
    subprocess.run(['git','-c','user.name=release-test','-c','user.email=release-test@invalid','commit','-q','-m','defaults'],cwd=repo,check=True)
    # Local edits to tracked files must not ship unless they are hash-verified patch files.
    (repo/'state/default.48S').write_text('local session')
    (repo/'state/patched.48S').write_text('verified patch')
    tree=ast.parse((root/'tools/package_release.py').read_text())
    functions=[node for node in tree.body if isinstance(node,ast.FunctionDef) and node.name in ('copy_tracked','filter_source','add_committed_source')]
    namespace={'Path':Path,'subprocess':subprocess,'shutil':shutil,'io':io,'tarfile':tarfile}
    exec(compile(ast.Module(body=functions,type_ignores=[]),'copy_tracked','exec'),namespace)
    destination=Path(directory)/'payload'
    namespace['copy_tracked'](repo,('resources',),destination,Path('resources'))
    namespace['copy_tracked'](repo,('state',),destination,verified=('state/patched.48S',))
    assert (destination/'default.txt').read_text()=='resources/default.txt'
    assert (destination/'state/default.48S').read_text()=='state/default.48S'
    assert (destination/'state/patched.48S').read_text()=='verified patch'
    assert not (destination/'private-token.txt').exists()
    assert not (destination/'state/local-session.48S').exists()
    checks.append({'name':'Only committed defaults and verified patch files are packaged; private resources and local edits are excluded','passed':True})
    buffer=io.BytesIO()
    with tarfile.open(fileobj=buffer,mode='w') as archive:
        namespace['add_committed_source'](archive,repo,'source',verified=('state/patched.48S',))
    with tarfile.open(fileobj=io.BytesIO(buffer.getvalue())) as archive:
        assert archive.extractfile('source/state/default.48S').read()==b'state/default.48S'
        assert archive.extractfile('source/state/patched.48S').read()==b'verified patch'
        assert 'source/resources/private-token.txt' not in archive.getnames()
        assert 'source/state/local-session.48S' not in archive.getnames()
    checks.append({'name':'Corresponding source excludes local tracked edits and untracked files','passed':True})
    # Optimized parents must run their verification subprocesses without -O.
    for tool in ('verify_release.py','package_release.py'):
        fixture=Path(directory)/tool.removesuffix('.py');(fixture/'tools').mkdir(parents=True)
        shutil.copy2(root/'tools'/tool,fixture/'tools'/tool)
        (fixture/'VERSION').write_text('test')
        if tool=='verify_release.py':
            (fixture/'patches').mkdir();(fixture/'patches/db48x-picocalc.patch').write_bytes(b'')
            (fixture/'patches/db48x-picocalc.json').write_text(json.dumps({'patch_sha256':__import__('hashlib').sha256(b'').hexdigest(),'files':{}}))
            for name in ('research_engine.h','research_numeric.h'):
                for parent in ('pico','tools/baselines/port301'):
                    path=fixture/parent/name;path.parent.mkdir(parents=True,exist_ok=True);path.write_text('same')
            gate=fixture/'tools/generate_ui_help.py'
        else:
            gate=fixture/'tools/verify_release.py'
        gate.write_text("assert False, 'verification subprocess gate active'\n")
        subprocess.run(['git','init','-q',str(fixture)],check=True)
        subprocess.run(['git','add','.'],cwd=fixture,check=True)
        subprocess.run(['git','-c','user.name=release-test','-c','user.email=release-test@invalid','commit','-q','-m','fixture'],cwd=fixture,check=True)
        result=subprocess.run(['python3',str(fixture/'tools'/tool)],cwd=fixture,capture_output=True,text=True,env=dict(os.environ,PYTHONOPTIMIZE='1'))
        assert result.returncode!=0 and 'verification subprocess gate active' in result.stderr,(tool,result.stderr)
        checks.append({'name':tool+' preserves subprocess gates under PYTHONOPTIMIZE','passed':True})

    missing=subprocess.run(['python3',str(root/'tools/verify_release.py'),'--evidence',str(Path(directory)/'missing.json')],cwd=root,capture_output=True,text=True,env=os.environ.copy())
    assert missing.returncode!=0 and 'Missing candidate verification evidence' in missing.stderr
    checks.append({'name':'Reject missing candidate evidence','passed':True})
    optimized=subprocess.run(['python3',str(root/'tools/verify_release.py'),'--evidence',str(Path(directory)/'missing.json')],cwd=root,capture_output=True,text=True,env=dict(os.environ,PYTHONOPTIMIZE='1'))
    assert optimized.returncode!=0 and 'Missing candidate verification evidence' in optimized.stderr
    checks.append({'name':'Release checks stay active under PYTHONOPTIMIZE','passed':True})
    evidence=json.loads((root/'docs/review/release-evidence.json').read_text())
    # The published evidence is intentionally stale after unreleased changes;
    # isolate each rejection test from that independent mismatch.
    evidence['firmware_sha256']=__import__('hashlib').sha256((root/'build/pico/db48x-picocalc-pico2w.uf2').read_bytes()).hexdigest()
    for name in evidence['source_hashes']:
        path=root/name
        if path.is_file():evidence['source_hashes'][name]=__import__('hashlib').sha256(path.read_bytes()).hexdigest()

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
