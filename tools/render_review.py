#!/usr/bin/env python3
"""Render the bounded current-release review set at native resolution."""
from pathlib import Path
import json,shutil,subprocess
root=Path(__file__).resolve().parents[1]
work=root/'build/review-render';work.mkdir(parents=True,exist_ok=True)
selected=work/'selected';selected.mkdir(exist_ok=True)
for p in selected.glob('*.ppm'):p.unlink()
for name in json.loads((root/'tools/review-captures.json').read_text()):
 shutil.copy2(root/'reports/release-controllers'/(name+'.ppm'),selected/(name+'.ppm'))
(work/'render.pro').write_text('QT += gui\nCONFIG += console c++17\nCONFIG -= app_bundle\nSOURCES += '+str(root/'tools/ui_contact_sheet.cc')+'\nTARGET = render\n')
subprocess.run([shutil.which('qmake') or '/opt/homebrew/opt/qtbase/bin/qmake','render.pro'],cwd=work,check=True)
subprocess.run(['make','-j2'],cwd=work,check=True)
subprocess.run([str(work/'render'),str(selected),str(root/'reports/release-screenshots')],check=True)
