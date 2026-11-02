#!/usr/bin/env python3
"""Reject unregistered screens and unreviewed direct rendering paths."""
from pathlib import Path
import re,json,hashlib
root=Path(__file__).resolve().parents[1]
inventory=json.loads((root/'pico/ui/screens.json').read_text())
ids={s['id'] for s in inventory};assert len(ids)==len(inventory),'duplicate screen ID'
files=sorted(set((root/'pico').rglob('*.cc'))|set((root/'pico').rglob('*.h'))|set((root/'pico').rglob('*.inc')))
files+= [root/'db48x/src'/name for name in ('user_interface.cc','stack.cc','graphics.cc','dmcp/sysmenu.cc')]
calls={};rendering=[]
for file in files:
    source=file.read_text();relative=str(file.relative_to(root))
    if file.parent==root/'pico' and (file.suffix=='.inc' or file.name=='desktop.cc'):
        for line in source.splitlines():
            if re.search(r'\b(?:page|choose_impl|view_text|value_result)\(',line) and not re.match(r'\s*(?:int|void|bool)\s+(?:page|choose_impl|view_text|value_result)\(',line):
                raise AssertionError((relative,'Controller bypasses registered screen wrapper',line.strip()))
    for id in re.findall(r'(?:screen_(?:page|menu|text|result)\(|ScreenScope\s+\w+\()"([^"]+)"',source):
        assert id in ids,(relative,'Unregistered screen',id)
        calls.setdefault(id,[]).append(relative)
    # Generated insertion strings name APIs but never execute rendering. Their
    # data-only generator and freshness check are covered by the release verifier.
    if relative=="pico/ui/editor_commands.h":continue
    for line in source.splitlines():
        # Calls rather than declarations; SVG text is a reviewed exported rendering path.
        if re.search(r'(?:\b(?:Screen|screen)\.text\(|\b(?:Screen|screen)\.draw\(|->graph\(|Settings\.(?:\w*[Ff]ont)\(|pico_information_text::draw\(|pico_native_layout::draw\(|svg\.text\()',line):
            normalized=re.sub(r'\s+',' ',line.strip())
            rendering.append({'source':relative,'sha256':hashlib.sha256(normalized.encode()).hexdigest(),'call':normalized})
review=json.loads((root/'pico/ui/rendering-review.json').read_text())
known={(s['source'],s['sha256']) for s in review['sites']}
actual={(s['source'],s['sha256']) for s in rendering}
assert actual==known,{'unreviewed':list(actual-known),'obsolete':list(known-actual)}
assert all(s.get('purpose') and s.get('tests') for s in review['sites'])
assert not re.search(r'(?:between\(|\.index\(|\.find\().*render_editor',(root/'tools/test_surface_engine.py').read_text()),'Extracted widget test'
report={'registered_screens':len(ids),'registered_call_sites':sum(map(len,calls.values())),'reviewed_direct_rendering_sites':len(actual),'production_widget_includes':True,'unreviewed_paths':[]}
(root/'reports/rendering-audit.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS:',report)
