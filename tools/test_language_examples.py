#!/usr/bin/env python3
"""Execute shipped Lua examples with production Lua and CHIP-8 examples with the actual assembler/VM."""
from pathlib import Path
import json,subprocess,hashlib,argparse
p=argparse.ArgumentParser();p.add_argument("--release",default="port216");args=p.parse_args()
root=Path(__file__).resolve().parents[1];out=root/'build/language-help-tests';out.mkdir(parents=True,exist_ok=True)
rows=json.loads((root/'docs/help/lua-reference.json').read_text());chip=json.loads((root/'docs/help/chip8-reference.json').read_text());q=lambda s:json.dumps(s,ensure_ascii=True)
expected={'Start with Lua':'5\n','Variables and functions':'144\n','Loops and decisions':'Correct\n','Tables and strings':'Iain: LumaRPL\n','Available libraries':'table\tnil\tnil\tnil\n','print':'Answer\t42\n','math functions':'9.0\n1.0\n','Errors and pcall':'false\tstring\n','screen.font':'compact\nmono\n','screen.measure':'true\ttrue\n','keyboard.poll':'a\n','keyboard.wait':'a\n','keyboard.down':'false\n','system.sleep':'Wait\nDone\n','system.millis':'true\n','plot.show':'Back from plot\n','Memory and saving':'100\n','calc.eval':'5\n'}
src='''#include "lua_runner.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
extern int test_key,test_draws,test_plots;
int pico_lua_cancel(void){return 0;}
// The controller suite separately exercises the real calculator bridge. This fixture supplies its known 2+3 result.
int pico_lua_eval(const char *s,char *out,size_t n){assert(!strcmp(s,"2+3"));snprintf(out,n,"5");return 1;}
int main(void){pico_lua_result r;
'''
for r in rows:
 src+='test_key=\'a\';test_plots=0;assert(pico_lua_run('+q(r['source'])+',strlen('+q(r['source'])+'),&r));\n'
 if r.get('stdout') is not None:src+='assert(!strcmp(r.output,'+q(r['stdout'])+'));\n'
 elif r['name'] in expected:src+='assert(!strcmp(r.output,'+q(expected[r['name']])+'));\n'
 elif r['name'].startswith('plot.'):src+='assert(test_plots>0);\n'
 elif r['name'].startswith('screen.') or r['name']=='Colours and coordinates':src+='assert(test_draws>0);\n'
 if 'screen.show(' in r['source']:src+='assert(r.graphics && test_draws>0);\n'
 if 'plot.show(' in r['source']:src+='assert(test_plots>0);\n'
 src+='puts('+q('PASS Lua: '+r['name'])+');\n'
for code,line in [('\nerror("script:9999: custom",0)',2),('local value =\n',2),('print(string.rep("x",3000))\nlocal a=1\nerror("kept")',3),('error({})',1)]:
 src+='assert(!pico_lua_run('+q(code)+',strlen('+q(code)+'),&r));assert(r.error_line=='+str(line)+');assert(*r.error);\n'
src+='return 0;}\n';(out/'lua.c').write_text(src)
# Whole programs are assembled exactly as published. Selected arithmetic/memory pages also run to a checked state.
checks={'Start with CHIP-8':(0,5),'Registers and numbers':(0,12),'Labels and comments':(0,7),'LD':(1,12),'ADD':(0,4),'SUB':(0,5),'SUBN':(0,5),'OR':(0,15),'AND':(0,3),'XOR':(0,12),'SHR':(0,4),'SHL':(0,2),'SE':(2,9),'SNE':(2,9),'CALL':(0,6),'LOAD':(0,7),'FLAGSAVE':(0,42),'FLAGLOAD':(0,42),'EXIT':(0,7),'DW':(0,7),'JP':(0,5),'JPOFF':(1,9),'Save CHIP-8 source':(0,5),'Assembly error':(0,7),'Blank CHIP-8 screen':(0,5),'Draw a digit':(0,9)}
src='''#include "chip8.h"
#include "chip8_assembler.h"
#include <cassert>
#include <cstdio>
using namespace pico_chip8;
int main(){assembler a;unsigned char rom[3584];size_t n;machine m;
'''
for r in chip:
 src+='if(!a.assemble('+q(r['source'])+',rom,sizeof rom,n)){fprintf(stderr,"%s line %u: %s\\n",'+q(r['name'])+',a.line,a.error);return 1;}\n'
 if r['name'] in checks or r['example_kind']=='program':
  src+='assert(m.load(rom,n,schip));for(unsigned steps=0;steps<2000&&!m.exited&&m.waiting<0&&m.pc<0x200+n;++steps){if(steps%20==0)m.tick();assert(m.step()&&!m.error);}'
  if r['name'] in checks:
   reg,val=checks[r['name']];src+='assert(m.v['+str(reg)+']=='+str(val)+');'
  if r['example_kind']=='program':src+='assert(m.exited||m.waiting>=0);'
  if r['name'] in ['Start with CHIP-8','Blank CHIP-8 screen','Save CHIP-8 source','Draw a digit']:src+='assert(m.pixel(20,16));'
  src+='\n'
 if r['name']=='SAVE':src+='assert(m.load(rom,n,schip));for(int i=0;i<4;++i)m.step();assert(m.memory[0x300]==7&&m.memory[0x301]==9);\n'
 if r['name']=='Fonts and BCD':src+='assert(m.load(rom,n,schip));for(int i=0;i<3;++i)m.step();assert(m.memory[0x300]==1&&m.memory[0x301]==2&&m.memory[0x302]==3);\n'
 if r['name']=='DRW':src+='assert(m.load(rom,n,schip));for(int i=0;i<5;++i)m.step();assert(m.pixel(20,16));\n'
 src+='puts('+q('PASS CHIP-8: '+r['name'])+');\n'
src+='unsigned line=0;assert(a.assemble("CLS\\nRET\\n",rom,sizeof rom,n,0x202,&line)&&line==2);\n'
src+='}\n';(out/'chip8.cc').write_text(src)
def run(args):subprocess.run(args,cwd=root,check=True)
flags=['-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-Ipico']
run(['clang++','-std=c++17',*flags,'-c','tests/plot_engine_adapter.cc','-o',str(out/'plot.o')])
lua=[str(p) for p in (root/'dependencies/lua-5.4.9/src').glob('*.c') if p.name not in ('lua.c','luac.c')]
run(['clang','-std=c11','-DLUAI_MAXCCALLS=40',*flags,'-Idependencies/lua-5.4.9/src',*lua,'pico/lua_runner.c','tests/lua_device_stub.c',str(out/'lua.c'),str(out/'plot.o'),'-lc++','-lm','-o',str(out/'lua')])
run([str(out/'lua')]);run(['clang++','-std=c++17',*flags,str(out/'chip8.cc'),'-o',str(out/'chip8')]);run([str(out/'chip8')])
report=dict(lua_examples=len(rows),chip8_examples_assembled=len(chip),chip8_state_examples=len(set(checks)|{r['name'] for r in chip if r['example_kind']=='program'}|{'SAVE','Fonts and BCD','DRW'}),sanitizers=['address','undefined'],error_location_checks=4,scope='Production Lua runtime and native sampler/assembler/VM; Lua hardware drawing, keyboard, clock, sound and calculator bridge boundaries use host fixtures. Device appearance/timing are not established.',source_sha256={str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'docs/help/lua-reference.json',root/'docs/help/chip8-reference.json']})
(root/'reports'/(args.release+'-language-examples.json')).write_text(json.dumps(report,indent=2)+'\n')
