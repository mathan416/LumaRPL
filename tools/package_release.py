#!/usr/bin/env python3
"""Create a current-only bundle and corresponding source; never deploy."""
import argparse,hashlib,json,shutil,subprocess,tarfile
from pathlib import Path
root=Path(__file__).resolve().parents[1]
version=(root/'VERSION').read_text().strip()
parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--loader',type=Path);args=parser.parse_args()
if subprocess.check_output(['git','status','--porcelain'],cwd=root):raise SystemExit('Commit candidate source changes before packaging')
verify=['python3',str(root/'tools/verify_release.py')]
if args.loader:verify+=['--loader',str(args.loader.resolve())]
subprocess.run(verify,check=True)
firmware=root/'build/pico/db48x-picocalc-pico2w.uf2'
command=['python3',str(root/'tools/validate_uf2.py'),str(firmware)]
if args.loader:command+=['--loader',str(args.loader.resolve())]
metadata=json.loads(subprocess.check_output(command,text=True))
output=root/'dist'/version
if output.exists():
    raise SystemExit(f'{output} exists; archive or remove it explicitly before repackaging')
output.mkdir(parents=True)
card=output/'sd-card'
(card/'pico2-apps').mkdir(parents=True)
shutil.copy2(firmware,card/'pico2-apps/LumaRPL-Pico-2-W.uf2')
shutil.copy2(firmware,output/'LumaRPL-Pico-2-W.uf2')
for name in ('config','state','help','library'):
    shutil.copytree(root/'db48x'/name,card/'db48x'/name,ignore=shutil.ignore_patterns('.DS_Store','bad','*.log'))
shutil.copytree(root/'resources',card/'db48x',dirs_exist_ok=True,ignore=shutil.ignore_patterns('.DS_Store','__pycache__'))
for name in ('README.md','BUILDING.md','NOTICE.md','LICENSE','CHANGELOG.md','VERSION'):
    shutil.copy2(root/name,output/name)
shutil.copytree(root/'licenses',output/'licenses')
shutil.copytree(root/'docs',output/'docs')
linker_map=root/'build/pico/db48x-picocalc-pico2w.elf.map'
if linker_map.exists():(output/'firmware.map').write_text(linker_map.read_text().replace(str(root)+'/', ''))
(output/'SOURCE.json').write_text(json.dumps({'version':version,'git_commit':subprocess.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip(),'firmware_sha256':metadata['sha256'],'tracked_source_clean':True},indent=2)+'\n')
(output/'UF2.json').write_text(json.dumps(metadata,indent=2)+'\n')
(output/'MEMORY.json').write_bytes(subprocess.check_output(['python3',str(root/'tools/report_memory.py')]))
# Include complete pinned dependency working trees and the patched upstream source.
# Builds, Git databases and local device/user data are never included.
def filter_source(info):
    parts=Path(info.name).parts
    if any(p in ('.git','.build','__pycache__','.DS_Store','bad','db48x.app') for p in parts): return None
    if info.name.endswith(('.o','.a','.pyc','.log')): return None
    return info
with tarfile.open(output/'LumaRPL-source.tar.gz','w:gz') as archive:
    # Only versioned files from the application and each dependency checkout.
    paths = set(subprocess.check_output(['git','ls-files','-z'],cwd=root).decode().split('\0')) - {''}
    lock=json.loads((root/'SOURCES.lock.json').read_text())
    repos=[root/'db48x']+[root/item['directory'] for item in lock.values() if isinstance(item,dict) and 'commit' in item]
    for name,modules in lock['required_submodules'].items():
        repos += [root/lock[name]['directory']/spec.split()[1] for spec in modules]
    for repo in repos:
        for name in subprocess.check_output(['git','ls-files','-z'],cwd=repo).decode().split('\0'):
            path=repo/name
            if name and path.is_file():paths.add(str(path.relative_to(root)))
    paths.update('db48x/'+name for name in json.loads((root/'patches/db48x-picocalc.json').read_text())['files'])
    lua=root/lock['lua']['directory']
    paths.update(str(path.relative_to(root)) for path in lua.rglob('*') if path.is_file() and path.suffix not in ('.o','.a'))
    for name in sorted(paths):
        path=root/name
        if path.is_file():archive.add(path,arcname='LumaRPL/'+name,filter=filter_source)
shutil.make_archive(str(output/'LumaRPL-SD'),'zip',card)
lines=[]
for path in sorted(output.rglob('*')):
    if path.is_file():lines.append(hashlib.sha256(path.read_bytes()).hexdigest()+'  '+str(path.relative_to(output)))
(output/'SHA256SUMS').write_text('\n'.join(lines)+'\n')
print(output)
