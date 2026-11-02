#!/usr/bin/env python3
"""Generate checked applied-math inputs with the production encoder."""
from pathlib import Path
import subprocess,argparse
root=Path(__file__).resolve().parents[1];p=argparse.ArgumentParser();p.add_argument('--check',action='store_true');a=p.parse_args();out=root/'build/science-demo';out.mkdir(exist_ok=True)
(out/'demo.cc').write_text('#include "science_document.h"\n#include <cstdio>\nint main(){pico_science_document d;uint8_t bytes[d.bytes];if(d.encode(bytes)!=sizeof bytes)return 1;return fwrite(bytes,1,sizeof bytes,stdout)==sizeof bytes?0:2;}\n')
subprocess.run(['clang++','-std=c++17','-Ipico',str(out/'demo.cc'),'-o',str(out/'demo')],cwd=root,check=True)
data=subprocess.check_output([str(out/'demo')]);path=root/'resources/science/DEMO.science'
if a.check:assert path.read_bytes()==data
else:path.parent.mkdir(exist_ok=True);path.write_bytes(data)
print('PASS: 2048-byte production-encoded applied-math demo')
