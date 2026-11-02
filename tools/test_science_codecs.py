#!/usr/bin/env python3
"""Validate old release files with current bounded decoders, under sanitizers."""
from pathlib import Path
import subprocess,tarfile,tempfile,json
root=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix='lumarpl24-codec-') as name:
 d=Path(name)
 for header in ['advanced_math.h','math_lab.h']:
  (d/header).write_bytes((root/'tests/fixtures/legacy'/header).read_bytes())
 (d/'old.cc').write_text('#include "advanced_math.h"\n#include "math_lab.h"\n#include <cstdio>\nint main(int,char**a){pico_advanced_math x;pico_math_lab y;unsigned char b[x.bytes],c[y.bytes];x.encode(b);y.encode(c);FILE*f=fopen(a[1],"wb");fwrite(b,1,sizeof b,f);fclose(f);f=fopen(a[2],"wb");fwrite(c,1,sizeof c,f);fclose(f);}')
 subprocess.run(['clang++','-std=c++17','-I'+str(root/'pico'),str(d/'old.cc'),'-o',str(d/'old')],check=True)
 subprocess.run([str(d/'old'),str(d/'advanced'),str(d/'mathlab')],check=True)
 (d/'advanced_math.h').unlink();(d/'math_lab.h').unlink()
 source=r'''#include "science_document.h"
#include "advanced_math.h"
#include <cassert>
#include <cstdio>
static void file(const char *p,unsigned char *b,size_t n){FILE *f=fopen(p,"rb");assert(f&&fread(b,1,n,f)==n&&fgetc(f)==EOF);fclose(f);}
int main(int argc,char **argv){
 unsigned char a[pico_advanced_math::bytes],m[pico_math_lab::bytes];file(argv[1],a,sizeof a);file(argv[2],m,sizeof m);
 pico_advanced_math advanced; pico_math_lab lab;assert(advanced.decode(a,sizeof a)&&lab.decode(m,sizeof m));for(auto &s:advanced.units)assert(!*s);assert(!*lab.normal_units);
 strcpy(advanced.units[0],"s");strcpy(advanced.units[1],"m");strcpy(lab.normal_units,"m");assert(advanced.encode(a)==sizeof a&&lab.encode(m)==sizeof m);
 pico_advanced_math ar;pico_math_lab lr;assert(ar.decode(a,sizeof a)&&!strcmp(ar.units[1],"m"));assert(lr.decode(m,sizeof m)&&!strcmp(lr.normal_units,"m"));a[100]^=1;m[100]^=1;assert(!ar.decode(a,sizeof a)&&!lr.decode(m,sizeof m));assert(!strcmp(ar.units[1],"m")&&!strcmp(lr.normal_units,"m"));
 pico_science_document science,restored;unsigned char b[pico_science_document::bytes];assert(science.encode(b)==sizeof b&&restored.decode(b,sizeof b));for(size_t n=0;n<sizeof b;++n)assert(!restored.decode(b,n));b[100]^=1;assert(!restored.decode(b,sizeof b)&&!strcmp(restored.quantity,science.quantity));puts("PASS: port23 v1 advanced/mathlab compatibility; current unit-bearing round trips; CRC/truncated reads preserve documents");}
'''
 (d/'test.cc').write_text(source)
 subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-I'+str(root/'pico'),str(d/'test.cc'),'-o',str(d/'test')],check=True)
 subprocess.run([str(d/'test'),str(d/'advanced'),str(d/'mathlab')],check=True)
