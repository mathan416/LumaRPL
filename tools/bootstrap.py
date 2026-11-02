#!/usr/bin/env python3
"""Fetch pinned sources and verify the complete LumaRPL patch."""
import argparse, hashlib, json, subprocess, tarfile, tempfile, urllib.request
from pathlib import Path
root = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--source-cache', type=Path, help='Existing checkout; Git objects only are reused')
args = parser.parse_args()
lock = json.loads((root / 'SOURCES.lock.json').read_text())
def run(*cmd, cwd=root):
    subprocess.run(cmd, cwd=cwd, check=True)
def checkout(dest, url, commit, cached=None):
    if not (dest / '.git').exists():
        if dest.exists() and any(dest.iterdir()):
            raise SystemExit(f'{dest}: existing directory is not a Git checkout')
        dest.parent.mkdir(parents=True, exist_ok=True)
        source = str(cached) if cached and cached.exists() else url
        run('git', 'clone', '--no-checkout', source, str(dest))
        run('git', 'checkout', '--detach', commit, cwd=dest)
    head = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=dest, text=True).strip()
    if head != commit:
        raise SystemExit(f'{dest}: wrong revision; resolve manually')
for name in ('db48x', 'pico-sdk', 'uf2loader', 'picotool', 'chip8-test-suite', 'mbedtls'):
    item = lock[name]
    cached = args.source_cache / item['directory'] if args.source_cache else None
    checkout(root / item['directory'], item['url'], item['commit'], cached)
for name, modules in lock['required_submodules'].items():
    directory = lock[name]['directory']
    for spec in modules:
        commit, path, *_ = spec.split()
        dest = root / directory / path
        cached = args.source_cache / directory / path if args.source_cache else None
        if cached and cached.exists():
            checkout(dest, str(cached), commit, cached)
        else:
            parent = root / directory
            if path == 'recorder/make-it-quick':
                parent /= 'recorder'
                path = 'make-it-quick'
            run('git', 'submodule', 'update', '--init', path, cwd=parent)
            checkout(dest, '', commit)
manifest = json.loads((root / 'patches/db48x-picocalc.json').read_text())
patch = root / 'patches/db48x-picocalc.patch'
def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()
if digest(patch) != manifest['patch_sha256']:
    raise SystemExit('Patch checksum mismatch')
source = root / 'db48x'
if not all((source / name).is_file() and digest(source / name) == expected
           for name, expected in manifest['files'].items()):
    run('git', 'apply', '--check', str(patch), cwd=source)
    run('git', 'apply', str(patch), cwd=source)
if not all(digest(source / name) == expected for name, expected in manifest['files'].items()):
    raise SystemExit('Patched source checksum mismatch')
changed = set(subprocess.check_output(['git','diff','--name-only','HEAD'],cwd=source,text=True).splitlines())
# The upstream simulator build regenerates these tracked documentation files.
if changed - set(manifest['files']) - {'doc/8-menus-tree-dm42.md','help/db48x.md'}:
    raise SystemExit('Unexpected upstream modifications outside the verified patch')
item = lock['lua']
dest = root / item['directory']
# Always authenticate the archive, even when a Lua tree already exists.
archive_cache = root / 'dependencies' / ('lua-' + item['version'] + '.tar.gz')
payload = archive_cache.read_bytes() if archive_cache.exists() else urllib.request.urlopen(item['url'], timeout=60).read()
if hashlib.sha256(payload).hexdigest() != item['sha256']:
    raise SystemExit('Lua archive checksum mismatch')
archive_cache.write_bytes(payload)
with tempfile.TemporaryDirectory() as temporary:
    archive = Path(temporary) / 'lua.tar.gz'
    archive.write_bytes(payload)
    with tarfile.open(archive) as source:
        source.extractall(temporary, filter='data')
    extracted = Path(temporary) / dest.name
    if dest.exists():
        for entry in extracted.rglob('*'):
            if entry.is_file() and (not (dest / entry.relative_to(extracted)).is_file()
                                    or digest(entry) != digest(dest / entry.relative_to(extracted))):
                raise SystemExit('Existing Lua source differs from authenticated archive')
    else:
        import shutil
        shutil.copytree(extracted, dest)
print('Pinned sources and complete patch verified')
