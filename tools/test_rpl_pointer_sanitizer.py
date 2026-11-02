#!/usr/bin/env python3
"""Instrument production return-frame paths for undefined pointer arithmetic."""
from pathlib import Path
import os
import subprocess

root = Path(__file__).resolve().parents[1]
out = root/'build/simulator-rgb332'
names = ('runtime', 'program', 'object', 'conditionals', 'locals')
flags = '-fsanitize=pointer-overflow -fno-sanitize-recover=pointer-overflow'
rules = ['include Makefile\n']
for name in names:
    rules.append(f'''frame-{name}.o: ../../db48x/src/{name}.cc ../../db48x/src/runtime.h
\t$(CXX) -c $(CXXFLAGS) {flags} $(INCPATH) -o $@ $<
''')
objects = ' '.join(f'frame-{name}.o' for name in names)
removed = ' '.join(f'obj/{name}.o' for name in names)
remaining = f'$(filter-out {removed},$(OBJECTS))'
rules.append(f'''frames-sanitized: {objects} {remaining}
\t$(LINK) $(LFLAGS) {flags} -o $@ {objects} {remaining} $(OBJCOMP) $(LIBS)
''')
(out/'FrameSanitizer.mk').write_text(''.join(rules))
subprocess.run(['make', '-j4', '-f', 'FrameSanitizer.mk', 'frames-sanitized'],
               cwd=out, check=True)
environment = dict(os.environ, UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')
subprocess.run(['python3', str(root/'tools/test_rpl_frames.py'), '--binary',
                str(out/'frames-sanitized'), '--tag', 'port198-rpl-sanitized'],
               cwd=root, env=environment, check=True)
print('PASS: production runtime/program/object/conditionals/locals pointer-overflow sanitizer')
