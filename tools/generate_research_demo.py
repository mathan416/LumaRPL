#!/usr/bin/env python3
"""Generate the study example with the exact production wire codec."""
from pathlib import Path
import argparse,subprocess,tempfile
root=Path(__file__).resolve().parents[1];parser=argparse.ArgumentParser();parser.add_argument('--check',action='store_true');args=parser.parse_args()
with tempfile.TemporaryDirectory(prefix='lumarpl-study-') as tmp:
 folder=Path(tmp);source=folder/'demo.cc';binary=folder/'demo';out=folder/'DEMO.study'
 source.write_text('#include "research_document.h"\n#include <cstdio>\nint main(int,char**a){pico_research_document d;unsigned char b[d.bytes];if(!d.encode(b))return 2;FILE*f=fopen(a[1],"wb");if(!f)return 3;bool ok=fwrite(b,1,sizeof b,f)==sizeof b;return fclose(f)||!ok;}')
 subprocess.run(['clang++','-std=c++17','-O1','-I'+str(root/'pico'),str(source),'-o',str(binary)],check=True)
 subprocess.run([str(binary),str(out)],check=True);target=root/'resources/studies/DEMO.study'
 if args.check:assert target.read_bytes()==out.read_bytes(),'Regenerate study example'
 else:target.parent.mkdir(exist_ok=True);target.write_bytes(out.read_bytes())
print('PASS: production-codec DEMO.study')
