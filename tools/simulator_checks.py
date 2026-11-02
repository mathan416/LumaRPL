"""Shared, strict host-simulator execution and variant-matched help fixtures."""
from pathlib import Path
import hashlib
import json
import re
import subprocess
import sys


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
    with subprocess.Popen(command, cwd=cwd, stdout=subprocess.PIPE,
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
