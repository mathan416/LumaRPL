#!/usr/bin/env python3
"""Compile every insertion template with the supported Lua parser/CHIP-8 assembler."""
from pathlib import Path
import subprocess,json,hashlib,argparse
p=argparse.ArgumentParser();p.add_argument('--release',default='port217');args=p.parse_args()
root=Path(__file__).resolve().parents[1];out=root/'build/editor-command-tests';out.mkdir(exist_ok=True)
source=r'''#include "ui/editor_commands.h"
#include "chip8_assembler.h"
#include <cassert>
#include <cstring>
#include <cstdio>
extern "C" {
#include "lua.h"
#include "lauxlib.h"
}
int main(){lua_State *L=luaL_newstate();assert(L);unsigned lua=0,chip=0;pico_chip8::assembler a;unsigned char bytes[3584];size_t size;
for(const auto &c:pico_editor_commands::entries){assert(strlen(c.text)<=256);assert(c.cursor<=strlen(c.text)&&c.selection<=strlen(c.text)-c.cursor);if(c.language==2){if(luaL_loadbufferx(L,c.text,strlen(c.text),"@template","t")){fprintf(stderr,"%s: %s\n",c.name,lua_tostring(L,-1));return 1;}lua_pop(L,1);++lua;}else if(c.language==1){char text[512];snprintf(text,sizeof text,"start:\nroutine:\n%s",c.text);if(!a.assemble(text,bytes,sizeof bytes,size)){fprintf(stderr,"%s: %s\n",c.name,a.error);return 1;}++chip;}}
lua_close(L);assert(lua==24&&chip==32);puts("PASS: 24 Lua call templates compile; 32 CHIP-8 instruction templates assemble; all insertions bounded to 256 bytes");}
'''
(out/'templates.cc').write_text(source)
subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-Ipico','-Idependencies/lua-5.4.9/src',str(out/'templates.cc'),'build/simulator-rgb332/lua-probe.a','-o',str(out/'templates')],cwd=root,check=True)
subprocess.run([str(out/'templates')],cwd=root,check=True)
paths=['pico/ui/editor_commands.h','pico/chip8_assembler.h','tools/generate_editor_commands.py','tools/test_editor_commands.py']
report=dict(lua_templates=24,chip8_templates=32,rpl_command_lookup_checks=1026,scope='Production metadata/assembler and pinned Lua compiler. RPL lookup is separately exercised by production controllers. Host shim and assembler sanitizer-instrumented; linked pinned Lua compiler objects are not all instrumented.',source_sha256={p:hashlib.sha256((root/p).read_bytes()).hexdigest() for p in paths})
(root/'reports'/(args.release+'-command-templates.json')).write_text(json.dumps(report,indent=2)+'\n')
