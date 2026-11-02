#!/usr/bin/env python3
"""Compile production controllers with host device/storage I/O, at Pico dimensions."""
from pathlib import Path
import subprocess,shutil,hashlib,json,os
root=Path(__file__).resolve().parents[1];out=root/'build/simulator-rgb332'
(root/'reports').mkdir(exist_ok=True)
if not (out/'Makefile').exists():
    subprocess.run(['python3',str(root/'tools/build_colour_simulator.py'),'--indexed'],check=True)
if not all((out/name).exists() for name in ('lua-probe.a','header-font.o','calculator-bridge.o')):
    subprocess.run(['python3',str(root/'tools/test_surface_engine.py'),'--indexed'],check=True)
subprocess.run(['python3',str(root/'tools/prepare_port302_baseline.py')],check=True)
def source_state():
    paths=[p for folder in ('pico','db48x/src','tests') for p in (root/folder).rglob('*') if p.is_file() and p.suffix in ('.c','.cc','.h','.inc','.json')]
    return {str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(paths)}
def reference_state():
    return {str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for folder in ('resources/help','docs/help') for p in sorted((root/folder).rglob('*')) if p.is_file()}
initial_reference=reference_state()
initial_source=source_state()
original=(root/'db48x/src/dmcp/main.cc').read_text()
(out/'controller-probe-main.cc').write_text('void release_controller_cases();\n'+original.replace('    program_init();','    program_init();\n    release_controller_cases();',1))
(out/'ControllerProbe.mk').write_text('''include Makefile
UI_FLAGS=-I../../tests/ui_stub -I../../pico -I../../pico/fatfs -I../../tests -DPICOCALC_UI_HOST=1
controller-probe-main.o: controller-probe-main.cc
\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<
controller.o: ../../tests/controller_test.cc ../../tests/math_lab_cases.inc ../../tests/advanced_math_cases.inc ../../tests/science_cases.inc ../../tests/research_cases.inc ../../tests/legacy_evaluator_cases.inc ../../tests/advanced_settings_cases.inc ../../tests/port21_feature_cases.inc ../../tests/matrix_arithmetic_cases.inc ../../tests/port21_new_cases.inc ../../pico/desktop.cc $(wildcard ../../pico/*.inc ../../pico/*.h ../../pico/ui/*.inc ../../pico/ui/*.h)
\t$(CXX) -c $(CXXFLAGS) $(UI_FLAGS) -include ../../pico/target.h $(INCPATH) -o $@ $<
firmware-help-font.o: ../../db48x/fonts/HelpFont.cc
\t$(CXX) -c $(CXXFLAGS) $(UI_FLAGS) -include ../../pico/target.h $(INCPATH) -o $@ $<
firmware-editor-font.o: ../../db48x/fonts/EditorFont.cc
\t$(CXX) -c $(CXXFLAGS) $(UI_FLAGS) -include ../../pico/target.h $(INCPATH) -o $@ $<
firmware-reduced-font.o: ../../db48x/fonts/ReducedFont.cc
\t$(CXX) -c $(CXXFLAGS) $(UI_FLAGS) -include ../../pico/target.h $(INCPATH) -o $@ $<
firmware-stack-font.o: ../../db48x/fonts/StackFont.cc
\t$(CXX) -c $(CXXFLAGS) $(UI_FLAGS) -include ../../pico/target.h $(INCPATH) -o $@ $<
pico-system-menu.o: ../../pico/sysmenu.cc
\t$(CXX) -c $(CXXFLAGS) $(UI_FLAGS) -Dredraw_lcd=pico_ui_host_redraw -Dget_reset_state_file=pico_ui_host_reset_state -include ../../pico/target.h $(INCPATH) -o $@ $<
reference-font.o: ../../pico/ReferenceFont.cc
\t$(CXX) -c $(CXXFLAGS) $(UI_FLAGS) -include ../../pico/target.h $(INCPATH) -o $@ $<
ui-host-settings.o: ../../db48x/src/settings.cc ../../pico/base_settings.h
\t$(CXX) -c $(CXXFLAGS) $(UI_FLAGS) -include ../../pico/target.h $(INCPATH) -o $@ $<
ui-host-menu.o: ../../db48x/src/menu.cc
\t$(CXX) -c $(CXXFLAGS) $(UI_FLAGS) -include ../../pico/target.h $(INCPATH) -o $@ $<
ui-host-interface.o: ../../db48x/src/user_interface.cc ../../db48x/src/settings.h
\t$(CXX) -c $(CXXFLAGS) $(UI_FLAGS) -include ../../pico/target.h $(INCPATH) -o $@ $<
ui-host-graphics.o: ../../db48x/src/graphics.cc ../../db48x/src/settings.h
\t$(CXX) -Dredraw_lcd=pico_ui_host_redraw -c $(CXXFLAGS) $(UI_FLAGS) -include ../../pico/target.h $(INCPATH) -o $@ $<
ui-host-stack.o: ../../db48x/src/stack.cc ../../db48x/src/settings.h
\t$(CXX) -c $(CXXFLAGS) $(UI_FLAGS) -include ../../pico/target.h $(INCPATH) -o $@ $<
ui-lua-device.o: ../../pico/lua_devices.cc
	$(CXX) -c $(CXXFLAGS) $(UI_FLAGS) -Dlcd_refresh=pico_ui_host_refresh -Dstop_buzzer=pico_ui_host_stop_buzzer -Dstart_buzzer_freq=pico_ui_host_start_buzzer -include ../../pico/target.h $(INCPATH) -o $@ $<
ui-bulk.o: ../../pico/bulk_storage.cc
\t$(CXX) -c $(CXXFLAGS) -I../../pico $(INCPATH) -o $@ $<
ui-workspace.o: ../../pico/workspace.cc
\t$(CXX) -c $(CXXFLAGS) -I../../pico $(INCPATH) -o $@ $<
source-font.o: ../../pico/SourceFont.cc
\t$(CXX) -c $(CXXFLAGS) $(UI_FLAGS) -include ../../pico/target.h $(INCPATH) -o $@ $<
controller-probe: ui-host-settings.o source-font.o controller-probe-main.o controller.o reference-font.o ui-host-menu.o pico-system-menu.o ui-host-interface.o ui-host-stack.o ui-host-graphics.o firmware-help-font.o firmware-editor-font.o firmware-reduced-font.o firmware-stack-font.o ui-bulk.o ui-workspace.o ui-lua-device.o header-font.o calculator-bridge.o lua-probe.a $(filter-out obj/main.o obj/settings.o obj/menu.o obj/user_interface.o obj/stack.o obj/graphics.o obj/sysmenu.o obj/HelpFont.o obj/EditorFont.o obj/ReducedFont.o obj/StackFont.o,$(OBJECTS))
\t$(LINK) $(LFLAGS) -o $@ ui-host-settings.o source-font.o controller-probe-main.o controller.o reference-font.o ui-host-menu.o pico-system-menu.o ui-host-interface.o ui-host-stack.o ui-host-graphics.o firmware-help-font.o firmware-editor-font.o firmware-reduced-font.o firmware-stack-font.o ui-bulk.o ui-workspace.o ui-lua-device.o header-font.o calculator-bridge.o $(filter-out obj/main.o obj/settings.o obj/menu.o obj/user_interface.o obj/stack.o obj/graphics.o obj/sysmenu.o obj/HelpFont.o obj/EditorFont.o obj/ReducedFont.o obj/StackFont.o,$(OBJECTS)) lua-probe.a $(OBJCOMP) $(LIBS)
''')
sanitized=os.environ.get('LUMARPL_UI_SANITIZERS')=='1'
makefile='SanitizedControllerProbe.mk' if sanitized else 'ControllerProberelease.mk'
binary='controller-probe-release-sanitized' if sanitized else 'controller-probe-release'
source=(out/'ControllerProbe.mk').read_text()
objects=['ui-host-settings.o','source-font.o','controller.o','reference-font.o','ui-host-menu.o','pico-system-menu.o','ui-host-interface.o','ui-host-stack.o','ui-host-graphics.o','firmware-help-font.o','firmware-editor-font.o','firmware-reduced-font.o','firmware-stack-font.o','ui-bulk.o','ui-workspace.o','ui-lua-device.o']
source+='\n'+' '.join(objects)+': $(wildcard ../../db48x/src/*.h ../../pico/*.h)\n'
for name in objects:source=source.replace(name,name[:-2]+('-release-san.o' if sanitized else '-release.o'))
source=source.replace('controller-probe:',binary+':')
if sanitized:
    source=source.replace('include Makefile','include Makefile\nLFLAGS+=-fsanitize=address,undefined').replace('UI_FLAGS=', 'UI_FLAGS=-fsanitize=address,undefined -fno-omit-frame-pointer ')
(out/makefile).write_text(source)
# Rebuild the runner against its current result ABI; retain the existing Lua VM archive.
archive='lua-probe-release-san.a' if sanitized else 'lua-probe-release.a'
shutil.copy2(out/'lua-probe.a',out/archive)
subprocess.run(['ar','d',str(out/archive),'lua_runner.o'],check=True)
runner=out/('lua-runner-release-san.o' if sanitized else 'lua-runner-release.o')
subprocess.run(['clang','-std=c11','-DLUAI_MAXCCALLS=40','-O1','-g',*(['-fsanitize=address,undefined','-fno-omit-frame-pointer'] if sanitized else []),'-I'+str(root/'pico'),'-I'+str(root/'dependencies/lua-5.4.9/src'),'-c',str(root/'pico/lua_runner.c'),'-o',str(runner)],check=True)
subprocess.run(['ar','r',str(out/archive),str(runner)],check=True)
source=source.replace('lua-probe.a',archive)
(out/makefile).write_text(source)
subprocess.run(['make','-j4','-f',makefile,binary],cwd=out,check=True)
work=root/'build/release-ui-work'
if work.exists():shutil.rmtree(work)
work.mkdir()
for folder in ('help','config','library','state'):
    shutil.copytree(root/'db48x'/folder,work/folder,dirs_exist_ok=True)
shutil.copytree(root/'resources',work,dirs_exist_ok=True)
(work/'data').mkdir(exist_ok=True)
(work/'data/UI.48s').write_text('« 2 3 + »')
captures=root/'reports/release-controllers'
if captures.exists():shutil.rmtree(captures)
captures.mkdir()
subprocess.run([str(out/binary),'-H','-Z','-m208'],cwd=work,check=True,timeout=300)

assert reference_state()==initial_reference,"Reference changed during verification"
(root/"reports/release-reference-source.json").write_text(json.dumps(initial_reference,indent=2)+"\n")
assert source_state()==initial_source,"Sources changed during controller verification"
(root/"reports/release-controller-source.json").write_text(json.dumps(initial_source,indent=2)+"\n")
