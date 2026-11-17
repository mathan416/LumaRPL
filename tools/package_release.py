#!/usr/bin/env python3
"""Create a current-only bundle and corresponding source; never deploy."""
import argparse,os,hashlib,io,json,shutil,subprocess,tarfile
from pathlib import Path
root=Path(__file__).resolve().parents[1]
version=(root/'VERSION').read_text().strip()
parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--loader',type=Path);args=parser.parse_args()
if subprocess.check_output(['git','status','--porcelain'],cwd=root):raise SystemExit('Commit candidate source changes before packaging')
python_env=dict(os.environ)
python_env.pop("PYTHONOPTIMIZE",None)

verify=['python3',str(root/'tools/verify_release.py')]
if args.loader:verify+=['--loader',str(args.loader.resolve())]
subprocess.run(verify,check=True,env=python_env)
firmware=root/'build/pico/db48x-picocalc-pico2w.uf2'
command=['python3',str(root/'tools/validate_uf2.py'),str(firmware)]
if args.loader:command+=['--loader',str(args.loader.resolve())]
metadata=json.loads(subprocess.check_output(command,text=True,env=python_env))
output=root/'dist'/version
if output.exists():
    raise SystemExit(f'{output} exists; archive or remove it explicitly before repackaging')
output.mkdir(parents=True)
card=output/'sd-card'
(card/'pico2-apps').mkdir(parents=True)
shutil.copy2(firmware,card/'pico2-apps/LumaRPL-Pico-2-W.uf2')
shutil.copy2(firmware,output/'LumaRPL-Pico-2-W.uf2')
def copy_tracked(repo, folders, destination, prefix=None, verified=()):
    # Committed content only: local edits to tracked files never ship.
    # `verified` names working-tree files whose hashes were already checked.
    payload=subprocess.check_output(['git','archive','--format=tar','HEAD','--',*folders],cwd=repo)
    with tarfile.open(fileobj=io.BytesIO(payload)) as archive:
        for member in archive.getmembers():
            if not member.isfile() or member.name in verified:continue
            relative=Path(member.name).relative_to(prefix) if prefix else Path(member.name)
            target=destination/relative
            target.parent.mkdir(parents=True,exist_ok=True)
            target.write_bytes(archive.extractfile(member).read())
            target.chmod(member.mode&0o777)
    for name in verified:
        if not any(Path(name).is_relative_to(folder) for folder in folders):continue
        relative=Path(name).relative_to(prefix) if prefix else Path(name)
        target=destination/relative
        target.parent.mkdir(parents=True,exist_ok=True)
        shutil.copy2(repo/name,target)
# Package versioned defaults only; local simulator states and credentials stay private.
# db48x is upstream HEAD plus the hash-verified LumaRPL patch files.
patched=tuple(json.loads((root/'patches/db48x-picocalc.json').read_text())['files'])
copy_tracked(root/'db48x',('config','state','help','library'),card/'db48x',verified=patched)
copy_tracked(root,('resources',),card/'db48x',Path('resources'))
copy_tracked(root,('licenses',),card/'db48x')
shutil.copy2(root/'NOTICE.md',card/'db48x/licenses/PORT-NOTICES.md')
shutil.copy2(root/'SOURCES.lock.json',card/'db48x/licenses/SOURCES.lock.json')
copy_tracked(root,('docs/manuals/pdf',),card/'db48x/manuals',Path('docs/manuals/pdf'))
for name in ('README.md','BUILDING.md','NOTICE.md','LICENSE','CHANGELOG.md','VERSION'):
    shutil.copy2(root/name,output/name)
copy_tracked(root,('licenses','docs'),output)
linker_map=root/'build/pico/db48x-picocalc-pico2w.elf.map'
if linker_map.exists():(output/'firmware.map').write_text(linker_map.read_text().replace(str(root)+'/', ''))
(output/'SOURCE.json').write_text(json.dumps({'version':version,'git_commit':subprocess.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip(),'firmware_sha256':metadata['sha256'],'tracked_source_clean':True},indent=2)+'\n')
(output/'UF2.json').write_text(json.dumps(metadata,indent=2)+'\n')
(output/'MEMORY.json').write_bytes(subprocess.check_output(['python3',str(root/'tools/report_memory.py')],env=python_env))
# Include committed pinned dependencies and the verified patched upstream source.
# Builds, Git databases and local device/user data are never included.
def filter_source(info):
    parts=Path(info.name).parts
    if any(p in ('.git','.build','__pycache__','.DS_Store','bad','db48x.app') for p in parts): return None
    if info.name.endswith(('.o','.a','.pyc','.log')): return None
    return info
def add_committed_source(archive, repo, prefix, verified=()):
    payload=subprocess.check_output(['git','archive','--format=tar','HEAD'],cwd=repo)
    with tarfile.open(fileobj=io.BytesIO(payload)) as committed:
        for member in committed.getmembers():
            if member.isdir() or member.name in verified:continue
            stream=committed.extractfile(member) if member.isfile() else None
            member.name=prefix+'/'+member.name
            if member.islnk():member.linkname=prefix+'/'+member.linkname
            if filter_source(member):archive.addfile(member,stream)
    for name in verified:
        archive.add(repo/name,arcname=prefix+'/'+name,recursive=False,filter=filter_source)

with tarfile.open(output/'LumaRPL-source.tar.gz','w:gz') as archive:
    add_committed_source(archive,root,'LumaRPL')
    lock=json.loads((root/'SOURCES.lock.json').read_text())
    repos={root/item['directory']:item['commit'] for item in lock.values() if isinstance(item,dict) and 'commit' in item}
    for name,modules in lock['required_submodules'].items():
        for spec in modules:
            pin,path,*_=spec.split()
            repos[root/lock[name]['directory']/path]=pin
    for repo,pin in repos.items():
        actual=subprocess.check_output(['git','rev-parse','HEAD'],cwd=repo,text=True).strip()
        if actual!=pin:raise SystemExit('Pinned source revision mismatch: '+str(repo))
        verified=patched if repo==root/'db48x' else ()
        add_committed_source(archive,repo,'LumaRPL/'+str(repo.relative_to(root)),verified)
    lua_archive=root/'dependencies'/('lua-'+lock['lua']['version']+'.tar.gz')
    if hashlib.sha256(lua_archive.read_bytes()).hexdigest()!=lock['lua']['sha256']:
        raise SystemExit('Lua source archive checksum mismatch')
    with tarfile.open(lua_archive) as lua:
        for member in lua.getmembers():
            if not member.isfile():continue
            stream=lua.extractfile(member)
            member.name='LumaRPL/dependencies/'+member.name
            if filter_source(member):archive.addfile(member,stream)
shutil.make_archive(str(output/'LumaRPL-SD'),'zip',card)
lines=[]
for path in sorted(output.rglob('*')):
    if path.is_file():lines.append(hashlib.sha256(path.read_bytes()).hexdigest()+'  '+str(path.relative_to(output)))
(output/'SHA256SUMS').write_text('\n'.join(lines)+'\n')
print(output)
