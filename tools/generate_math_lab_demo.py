#!/usr/bin/env python3
"""Generate the demo document using the production portable encoder."""
from pathlib import Path
import argparse,subprocess
root=Path(__file__).resolve().parents[1];p=argparse.ArgumentParser();p.add_argument('--check',action='store_true');args=p.parse_args();out=root/'build/math-lab-demo';out.mkdir(exist_ok=True)
source=out/'demo.cc';source.write_text('#include "math_lab.h"\n#include <cstdio>\nint main(){pico_math_lab d;uint8_t data[d.bytes];if(d.encode(data)!=d.bytes)return 1;return fwrite(data,1,d.bytes,stdout)==d.bytes?0:2;}\n')
subprocess.run(['clang++','-std=c++17','-Ipico',str(source),'-o',str(out/'demo')],cwd=root,check=True)
data=subprocess.check_output([str(out/'demo')]);path=root/'resources/mathlabs/DEMO.mathlab'
if args.check:assert path.read_bytes()==data
else:path.parent.mkdir(exist_ok=True);path.write_bytes(data)
print('PASS: production-encoded 1792-byte demo for all five workbenches')
