#!/usr/bin/env python3
"""Test the current study codec and legacy geometry compatibility with sanitizers."""
from pathlib import Path
import subprocess,tempfile,tarfile
root=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix='research-codecs-') as tmp:
 d=Path(tmp)
 (d/'exploration_documents.h').write_bytes((root/'tests/fixtures/legacy/exploration_documents.h').read_bytes())
 (d/'old.cc').write_text('#include "exploration_documents.h"\n#include <cstdio>\nint main(int,char**a){pico_geometry_document g;unsigned char b[g.bytes];g.encode(b);FILE*f=fopen(a[1],"wb");if(!f)return 1;bool ok=fwrite(b,1,sizeof b,f)==sizeof b;return fclose(f)||!ok;}')
 subprocess.run(['clang++','-std=c++17','-I'+str(root/'pico'),str(d/'old.cc'),'-o',str(d/'old')],check=True)
 subprocess.run([str(d/'old'),str(d/'old.geometry')],check=True);(d/'exploration_documents.h').unlink()
 (d/'test.cc').write_text(r'''#include "research_document.h"
#include "exploration_documents.h"
#include <cassert>
#include <cstdio>
int main(int,char**a){unsigned char b[pico_research_document::bytes];FILE*f=fopen(a[1],"rb");assert(f);size_t n=fread(b,1,sizeof b,f);fclose(f);assert(n==144);pico_geometry_document g;assert(g.decode(b,n)&&!*g.units);strcpy(g.units,"m");assert(g.encode(b)==g.bytes);pico_geometry_document loaded;assert(loaded.decode(b,g.bytes)&&!strcmp(loaded.units,"m"));b[70]^=1;assert(!loaded.decode(b,g.bytes)&&!strcmp(loaded.units,"m"));pico_research_document d,r;assert(d.encode(b)==sizeof b&&r.decode(b,sizeof b));for(unsigned i=0;i<sizeof b;++i){b[i]^=1;assert(!r.decode(b,sizeof b));b[i]^=1;}for(unsigned n=0;n<sizeof b;++n)assert(!r.decode(b,n));d.correlation[0]=NAN;assert(!d.valid());puts("PASS: legacy 144-byte geometry and unit-bearing geometry roundtrip; every-byte study corruption/truncation rejection; invalid covariance validation");}''')
 subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-I'+str(root/'pico'),str(d/'test.cc'),'-o',str(d/'test')],check=True)
 subprocess.run([str(d/'test'),str(d/'old.geometry')],check=True)
