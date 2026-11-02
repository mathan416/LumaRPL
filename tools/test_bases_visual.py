#!/usr/bin/env python3
"""Capture the production Bases controller without altering full-suite outputs."""
from pathlib import Path
import os,subprocess,shutil
root=Path(__file__).resolve().parents[1];build=root/'build/simulator-rgb332'
source=(root/'tests/port301_controller_test.cc').read_text().replace('reports/port301-controllers/','reports/port301-bases-visual/')
a=source.index('static void bases_cases()');b=source.index('void port301_controller_cases()',a)
source=source[:a]+source[a:b].replace('pico_draw_calculator_frame(true);','calculator_frame("calculator.softmenus","Number bases");')+source[b:]
(build/'bases-visual.cc').write_text(source)
makefile=(build/'SanitizedControllerProbe.mk').read_text().replace('controller-301-san.o','controller-301-visual.o').replace('../../tests/port301_controller_test.cc','bases-visual.cc').replace('controller-probe-301-sanitized','controller-probe-301-visual')
(build/'BasesVisual.mk').write_text(makefile)
subprocess.run(['make','-j4','-f','BasesVisual.mk','controller-probe-301-visual'],cwd=build,check=True)
work=root/'build/port301-visual-work';shutil.rmtree(work,ignore_errors=True);work.mkdir()
for folder in ('help','config','library','state'):shutil.copytree(root/'db48x'/folder,work/folder)
shutil.copytree(root/'resources',work,dirs_exist_ok=True)
out=root/'reports/port301-bases-visual';shutil.rmtree(out,ignore_errors=True);out.mkdir()
subprocess.run([str(build/'controller-probe-301-visual'),'-H','-Z','-m208'],cwd=work,env=dict(os.environ,LUMARPL_BASES_ONLY='1'),check=True,timeout=60)
assert len(list(out.glob('bases-2.*.calculator.softmenus.ppm')))==8
print('PASS: production base conversion screens and command Help captured in eight themes')
