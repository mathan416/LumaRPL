#!/usr/bin/env python3
"""Verify manual calculator entries and language examples with production engines.

Lua device I/O is substituted; plots use the production numerical sampler.
CHIP-8 complete examples run in the production VM with synthetic key/timer input.
This does not assert physical-device acceptance or exact LCD/sound timing.
"""
from pathlib import Path
import hashlib,json,re,shutil,subprocess,tempfile,unicodedata
root=Path(__file__).resolve().parents[1];work=root/'build/manual-examples';work.mkdir(parents=True,exist_ok=True)
reports=[]
def canonical(s):
 s=unicodedata.normalize('NFKC',s).replace('⁄','/').replace('_','')
 return [''.join(line.split()) for line in s.strip().splitlines()]
cases=[
 ('First sum','2 3 +','5'),('Reuse answer','2 3 + 4 *','20'),('Subtract','10 3 -','7'),
 ('Divide','10 2 /','5'),('Power','2 3 ^','8'),('Parentheses on stack','3 4 + 5 6 + *','77'),
 ('Duplicate','7 DUP *','49'),('Swap','10 3 SWAP -','-7'),('Drop','1 2 DROP','1'),('Over','1 2 OVER','1\n2\n1'),
 ('Fraction','1 3 /','1/3'),('Complex','(1;2) (3;4) +','4+6i'),('Store','42 \'ANSWER\' STO ANSWER','42'),
 ('Determinant','[[1 2][3 4]] DET','-2'),('List sum','{1 2 3} ListSum','6'),
 ('Units','100_cm 1_m Convert','1m'),('Stored program','« DUP * » \'SQDEMO\' STO 12 SQDEMO','144'),
 ('Hypotenuse','3 4 « DUP * SWAP DUP * + SQRT » EVAL','5.'),
 ('Locals','3 4 « -> A B « A A * B B * + SQRT » » EVAL','5.'),
 ('Nested quotation','« « 2 3 + » » EVAL EVAL','5'),
 ('IF','« IF 2 1 > THEN 7 ELSE 9 END » EVAL','7'),
 ('FOR','0 « 1 5 FOR K K + NEXT » EVAL','15'),
 ('WHILE','0 « WHILE DUP 3 < REPEAT 1 + END » EVAL','3'),
 ('RPL comment','«\n@ Square the input\n12 DUP *\n» EVAL','144'),
 ('Decimal to hex','255 Hex','#FF₁₆'),('Binary input','\u0023101010b Dec','#42₁₀'),('Octal input','#377o Dec','#255₁₀'),
 ('Hex to decimal','#FFD2h Dec','#6\u205f5490₁₀'),('BinaryToReal','255 Hex BinaryToReal','255'),
]
binary=root/'build/simulator-rgb332/help-console'
assert binary.exists(),'Build production controllers and test_help_examples.py first'
for name,source,expected in cases:
 with tempfile.TemporaryDirectory(prefix='lumarpl-manual-') as directory:
  path=Path(directory);shutil.copytree(root/'db48x/config',path/'config');(path/'example.48s').write_text(source)
  r=subprocess.run([str(binary),'-H','-Z','-m208','-F',str(path/'example.48s')],cwd=path,capture_output=True,text=True,timeout=30)
  ok=r.returncode==0 and not r.stderr.strip() and canonical(r.stdout)==canonical(expected)
  reports.append(dict(language='RPL',name=name,source=source,expected=expected,actual=r.stdout,passed=ok))
  print(('PASS' if ok else 'FAIL'),name,repr(r.stdout),flush=True)
assert all(r['passed'] for r in reports),[r for r in reports if not r['passed']]

manual=(root/'docs/manuals/programming-guide.md').read_text()
lua=json.loads((root/'docs/help/lua-reference.json').read_text())
manual_lua=re.findall(r'```lua\n(.*?)\n```',manual,re.S)
expected={
 'Start with Lua':'5\n','Lua console':'144\n','Save a Lua script':'Saved script\n',
 'Variables and functions':'144\n','Loops and decisions':'Correct\n','Tables and strings':'Iain: LumaRPL\n',
 'math functions':'9.0\n1.0\n','print':'Answer\t42\n','calc.eval':'5\n',
 'Available libraries':'table\tnil\tnil\tnil\n','Errors and pcall':'false\tstring\n',
 'screen.font':'compact\nmono\n','screen.measure':'true\ttrue\n',
 'keyboard.poll':'a\n','keyboard.wait':'a\n','keyboard.down':'false\n',
 'system.sleep':'Wait\nDone\n','system.millis':'true\n','Memory and saving':'100\n'}
luacases=[dict(name=e['name'],source=e['source'],expected=e.get('stdout',expected.get(e['name']))) for e in lua]
for n,source in enumerate(manual_lua,1):
 exp={1:'Hello, LumaRPL\n5\n',2:'144\n',3:'Correct\n',4:'1\t1\n2\t4\n3\t9\n4\t16\n',5:'5\n',9:'Returned from graph\n',13:'false\tstring\n'}.get(n)
 # Native canvas and plot callbacks are asserted independently below.
 luacases.append(dict(name='Programming tutorial '+str(n),source=source,expected=exp))
api=root/'docs/manuals/examples/api'
for path in sorted(api.glob('*.lua')):
 exp={'plot.lua':'Back from graph\n','bridge.lua':'¹/₃\nfalse\tstring\n','errors.lua':'false\tstring\ncompact\n320\t320\n'}.get(path.name)
 luacases.append(dict(name='API '+path.name,source=path.read_text(),expected=exp))
q=lambda s:json.dumps(s,ensure_ascii=False)
inc=[]
for c in luacases:
 draw=bool(re.search(r'screen\.(?:clear|text|pixel|line|circle|rect|sprite)\(',c['source']))
 plot='plot.show()' in c['source']
 inc.append('{'+','.join([q(c['name']),q(c['source']),'nullptr' if c['expected'] is None else q(c['expected']),'true' if draw else 'false','true' if plot else 'false'])+'},')
(work/'lua-cases.inc').write_text('\n'.join(inc)+'\n')
runner=r'''#include "program.h"
#include "lua_runner.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
extern "C" {extern int test_key,test_draws,test_plots,test_plot_count;int pico_lua_cancel(void){return 0;}}
void run_manual_tests(){
 struct Case{const char *name,*source,*expected;bool drawing,plot;};Case cases[]={
 #include "lua-cases.inc"
 };
 for(auto &c:cases){test_key='a';test_plots=0;test_plot_count=0;pico_lua_result r;
 bool ok=pico_lua_run(c.source,strlen(c.source),&r);
 if(!ok||(c.expected&&strcmp(r.output,c.expected))||(c.drawing&&test_draws==0)||(c.plot&&(!test_plots||!test_plot_count))){fprintf(stderr,"FAIL manual Lua %s: %s\n",c.name,r.output);std::exit(1);}
 printf("PASS manual Lua: %s\n",c.name);
 } std::exit(0);
}
'''
(work/'manual-lua.cc').write_text(runner)
out=root/'build/simulator-rgb332'
original=(root/'db48x/src/dmcp/main.cc').read_text()
(out/'manual-main.cc').write_text('void run_manual_tests();\n'+original.replace('    program_init();','    program_init();\n    run_manual_tests();',1))
(out/'ManualProbe.mk').write_text('''include Makefile
manual-main.o: manual-main.cc
\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<
manual-cases.o: ../manual-examples/manual-lua.cc ../manual-examples/lua-cases.inc
\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -I../../pico -I../manual-examples -o $@ $<
manual-probe: manual-main.o manual-cases.o calculator-bridge.o lua-probe.a $(filter-out obj/main.o,$(OBJECTS))
\t$(LINK) $(LFLAGS) -o $@ manual-main.o manual-cases.o calculator-bridge.o $(filter-out obj/main.o,$(OBJECTS)) lua-probe.a $(OBJCOMP) $(LIBS)
''')
subprocess.run(['make','-j4','-f','ManualProbe.mk','manual-probe'],cwd=out,check=True)
private=work/'runtime';private.mkdir(exist_ok=True)
shutil.copytree(root/'db48x/config',private/'config',dirs_exist_ok=True)
r=subprocess.run([str(out/'manual-probe'),'-H','-Z','-m208'],cwd=private,capture_output=True,text=True,timeout=120)
print(r.stdout);assert r.returncode==0,r.stderr
reports += [dict(language='Lua',name=c['name'],passed=True,checks='production runner + calculator bridge; host device I/O; production plot sampler; exact stdout where specified') for c in luacases]

assembly=json.loads((root/'docs/help/chip8-reference.json').read_text())
asmcases=[dict(name=e['name'],source=e['source'],complete=e.get('example_kind')=='program') for e in assembly]
asmcases += [dict(name='Programming tutorial '+str(i),source=s,complete=True) for i,s in enumerate(re.findall(r'```asm\n(.*?)\n```',manual,re.S),1)]
asmcases += [dict(name='API '+p.name,source=p.read_text(),complete=True) for p in sorted(api.glob('*.asm'))]
(work/'asm-cases.inc').write_text('\n'.join('{'+q(e['name'])+','+q(e['source'])+','+('true' if e['complete'] else 'false')+'},' for e in asmcases)+'\n')
(work/'manual-asm.cc').write_text(r'''#include "chip8.h"
#include "chip8_assembler.h"
#include <cassert>
#include <cstdio>
#include <cstring>
int main(){struct Case{const char *name,*source;bool complete;};Case cases[]={
#include "asm-cases.inc"
};for(auto &c:cases){pico_chip8::assembler a;unsigned char rom[3584];size_t n;if(!a.assemble(c.source,rom,sizeof rom,n)){fprintf(stderr,"FAIL %s: line %u %s\n",c.name,a.line,a.error);return 1;}
if(c.complete){pico_chip8::machine m;assert(m.load(rom,n,pico_chip8::schip));for(int i=0;i<10000&&!m.exited&&!m.error;++i){m.tick();if(m.waiting>=0){m.set_keys(1);m.set_keys(0);}else m.step();}if(!m.exited||m.error){fprintf(stderr,"FAIL VM %s: %s\n",c.name,m.error?m.error:"no exit");return 1;}if(strstr(c.source,"DRW")){bool lit=false;for(auto b:m.screen)lit|=b!=0;assert(lit);}}
printf("PASS manual CHIP-8: %s\n",c.name);}}
''')
subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-I'+str(root/'pico'),str(work/'manual-asm.cc'),'-o',str(work/'manual-asm')],check=True)
subprocess.run([str(work/'manual-asm')],check=True)
reports += [dict(language='CHIP-8',name=c['name'],passed=True,checks='production assembler; complete examples run in production SUPER-CHIP VM with synthetic keys/timers') for c in asmcases]
# The companion's downloadable files must be identical to its printed programs.
api_text=(root/'docs/manuals/api-assembly-reference.md').read_text()
for path in sorted(api.glob('*')):
 if path.suffix not in ('.lua','.asm'):continue
 assert ('```'+('asm' if path.suffix=='.asm' else 'lua')+'\n'+path.read_text().rstrip()+'\n```') in api_text,path.name
subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-I'+str(root/'pico'),str(root/'tests/api_reference_test.cc'),'-o',str(work/'api-reference-test')],check=True)
subprocess.run([str(work/'api-reference-test'),str(api)],check=True)

record=dict(version=(root/'VERSION').read_text().strip(),cases=reports,device_verified=False,api_routine_contracts_passed=True,source_sha256={str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'docs/manuals/user-guide.md',root/'docs/manuals/programming-guide.md',root/'docs/help/lua-reference.json',root/'docs/help/chip8-reference.json',root/'docs/manuals/api-assembly-reference.md',*sorted(api.glob('*'))]})
(root/'output/pdf/manual-example-check.json').write_text(json.dumps(record,ensure_ascii=False,indent=2)+'\n')
print('PASS:',len(cases),'RPL,',len(luacases),'Lua,',len(asmcases),'CHIP-8 examples')
