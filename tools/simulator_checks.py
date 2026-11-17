"""Shared, strict host-simulator execution and variant-matched help fixtures."""
from pathlib import Path
import hashlib
import json
import os
import re
import subprocess
import sys


def simulator_env(work, base=None, home=None):
    """Keep a simulator run inside its prepared work directory.

    The DB48X simulator changes into its Qt AppDataLocation at startup
    (sim/sim-main.cpp), so a cwd alone is ignored: relative test paths would
    resolve in the user's real ~/Library or ~/.local data folder. A private home
    whose AppDataLocation links to `work` keeps runs hermetic and leaves the
    user's simulator data and preferences untouched.
    """
    work = Path(work).resolve()
    home = Path(home) if home else work.parent / (work.name + '.home')
    env = dict(os.environ if base is None else base)
    if sys.platform == 'darwin':
        env['CFFIXED_USER_HOME'] = str(home)
        data = home / 'Library/Application Support'
    else:
        env['XDG_DATA_HOME'] = str(home / 'share')
        env['XDG_CONFIG_HOME'] = str(home / 'config')
        data = home / 'share'
    env['HOME'] = str(home)
    link = data / 'DB48X/DB48X'
    link.parent.mkdir(parents=True, exist_ok=True)
    if link.is_symlink() or link.is_file():
        link.unlink()
    elif link.exists():
        raise SystemExit(f'{link} is a directory; remove it so it can point at {work}')
    link.symlink_to(work, target_is_directory=True)
    return env


def prepare_help(root, work, colour):
    """Colour goldens use the upstream DB50X manual; never rewrite goldens."""
    source = root / 'db48x/help' / ('db50x.md' if colour else 'db48x.md')
    data = source.read_bytes()
    entries = []
    offset = 0
    for line in data.splitlines(keepends=True):
        value = line.rstrip(b'\r\n')
        if value.startswith(b'#'):
            entries.append((value, offset))
        elif match := re.match(rb'^\* `[^`]*`', value):
            entries.append((match[0], offset))
        offset += len(line)
    index = b''.join(str(offset).encode() + b':' + text + b'\n'
                     for text, offset in sorted(entries))
    # The binary's configured filenames remain db48x.*, including for RGB332.
    (work / 'help/db48x.md').write_bytes(data)
    (work / 'help/db48x.idx').write_bytes(index)
    metadata = dict(manual=str(source.relative_to(root)),
                    manual_sha256=hashlib.sha256(data).hexdigest(),
                    index_sha256=hashlib.sha256(index).hexdigest(),
                    index_entries=len(entries), colour_goldens=colour)
    (work / 'HELP-FIXTURES.json').write_text(json.dumps(metadata, indent=2) + '\n')
    return metadata


def run_checked(command, cwd, report, output=None):
    """An assertion is a failure even when the simulator exits successfully."""
    output = output if output is not None else sys.stdout.buffer
    assertions = []
    with subprocess.Popen(command, cwd=cwd, env=simulator_env(cwd),
                          stdout=subprocess.PIPE,
                          stderr=subprocess.STDOUT) as process:
        for line in process.stdout:
            output.write(line)
            output.flush()
            if b'Assertion failed:' in line:
                assertions.append(line.decode('utf-8', errors='replace').strip())
        code = process.wait()
    result = dict(process_returncode=code, assertion_count=len(assertions),
                  assertions=assertions, passed=code == 0 and not assertions)
    Path(report).write_text(json.dumps(result, indent=2) + '\n')
    return code if code else int(bool(assertions))
