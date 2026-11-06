#!/usr/bin/env python3
"""Link a probe into the initialized colour simulator, using its existing objects."""
from pathlib import Path
import subprocess,os,argparse
parser=argparse.ArgumentParser();parser.add_argument('--indexed',action='store_true');parser.add_argument('--ui',action='store_true');args=parser.parse_args()
root=Path(__file__).resolve().parents[1];out=root/('build/simulator-rgb332' if args.indexed else 'build/simulator-colour')
# Link the same pinned Lua runner to the real calculator bridge for integration.
lua_obj=out/'lua-objects';lua_obj.mkdir(exist_ok=True)
lua_sources=[p for p in (root/'dependencies/lua-5.4.9/src').glob('*.c') if p.name not in ('lua.c','luac.c')]+[root/'pico/lua_runner.c',root/'tests/lua_device_stub.c']
objects=[]
for source in lua_sources:
    obj=lua_obj/(source.stem+'.o');objects.append(str(obj))
    if not obj.exists() or obj.stat().st_mtime<max(source.stat().st_mtime,Path(__file__).stat().st_mtime):
        subprocess.run(['clang','-DLUAI_MAXCCALLS=40','-std=c11','-Os','-I'+str(root/'dependencies/lua-5.4.9/src'),'-I'+str(root/'pico'),'-c',str(source),'-o',str(obj)],check=True)
plot_obj=lua_obj/'plot-engine-adapter.o'
subprocess.run(['clang++','-std=c++17','-Os','-I'+str(root/'pico'),'-c',str(root/'tests/plot_engine_adapter.cc'),'-o',str(plot_obj)],check=True)
objects.append(str(plot_obj))
subprocess.run(['ar','rcs',str(out/'lua-probe.a'),*objects],check=True)
original=(root/'db48x/src/dmcp/main.cc').read_text()
probe='#include "surface_engine_test.h"\n'+original.replace('    program_init();','    program_init();\n    run_surface_engine_tests();',1)
if args.ui:
    probe='#include "surface_engine_test.h"\n#include "native_ui_engine_test.h"\n'+original.replace('    program_init();','    program_init();\n    native_ui_test::run();\n    run_surface_engine_tests();',1)
(out/'surface-probe-main.cc').write_text(probe)
# The generated simulator Makefile supplies the exact Qt/engine compiler flags.
(out/'SurfaceProbe.mk').write_text('''include Makefile
surface-probe-main.o: surface-probe-main.cc
\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -I../../pico -I../../pico/fatfs -I../../tests -o $@ $<
source-font.o: ../../pico/SourceFont.cc
\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -I../../pico/fatfs -o $@ $<
header-font.o: ../../pico/HeaderFont.cc
\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -I../../pico/fatfs -o $@ $<
calculator-bridge.o: ../../pico/calculator_bridge.cc
\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -I../../pico -o $@ $<
surface-probe: surface-probe-main.o header-font.o source-font.o calculator-bridge.o lua-probe.a $(filter-out obj/main.o,$(OBJECTS))
\t$(LINK) $(LFLAGS) -o $@ surface-probe-main.o header-font.o source-font.o calculator-bridge.o $(filter-out obj/main.o,$(OBJECTS)) lua-probe.a $(OBJCOMP) $(LIBS)
''')
subprocess.run(['make','-j8','-f','SurfaceProbe.mk','surface-probe'],cwd=out,check=True)
subprocess.run([str(out/'surface-probe'),'-H','-Z','-m208'],cwd=Path(os.environ.get('PICOCALC_TEST_WORKDIR',str(root/'db48x'))),check=True,timeout=120)
