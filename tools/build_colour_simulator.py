#!/usr/bin/env python3
"""Build an optimized RGB565 or RGB332 upstream simulator in an isolated folder."""
from pathlib import Path
import argparse,shutil,subprocess
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--qmake',default=shutil.which('qmake') or '/opt/homebrew/opt/qtbase/bin/qmake');p.add_argument('--indexed',action='store_true');p.add_argument('--incremental',action='store_true');a=p.parse_args()
out=root/('build/simulator-rgb332' if a.indexed else 'build/simulator-colour');obj=out/'obj';obj.mkdir(parents=True,exist_ok=True)
configs=list((root/'db48x/.build').rglob('config.h'))
if not configs:
    subprocess.run(['make', '-C', str(root/'db48x'), 'sim', '-j4'], check=True)
    configs=list((root/'db48x/.build').rglob('config.h'))
shutil.copy2(max(configs,key=lambda f:f.stat().st_mtime),obj/'config.h')
if not a.incremental:
    for f in obj.glob('*.o'):f.unlink()
version=out/'host-version.h';version.write_text('#define DB48X_VERSION "0.10.0-colour"\n')
args=[a.qmake,'db48x.pro','-o',str(out/'Makefile'),'CONFIG+=color','-after','CONFIG-=debug','CONFIG+=release',
      'DEFINES-=DEBUG','DEFINES+=NDEBUG','QMAKE_CXXFLAGS+=-include '+str(version),
      'DESTDIR='+str(out),'OBJECTS_DIR='+str(obj),'RCC_DIR='+str(obj),'MOC_DIR='+str(obj),'UI_DIR='+str(obj)]
if a.indexed:args.append('DEFINES+=PICOCALC_RGB332')
subprocess.run(args,cwd=root/'db48x/sim',check=True)
makefile=(out/'Makefile').read_text()
assert '-DCONFIG_COLOR' in makefile and '-DNDEBUG' in makefile and '-O2' in makefile
assert '-DDEBUG ' not in makefile
subprocess.run(['make','-C',str(out),'-j8'],check=True)
print('Verified CONFIG_COLOR and release optimization. Executable:',out/'db48x.app/Contents/MacOS/db48x')
