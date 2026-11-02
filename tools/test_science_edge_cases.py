#!/usr/bin/env python3
from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[1]
source=r'''#include "science_numeric.h"
#include <cassert>
#include <cstdio>
int main(){pico_data_table d;d.rows=16;for(unsigned i=0;i<d.rows;++i){d.points[i][0]=i;d.points[i][1]=cos(6.283185307179586*3*i/16);}double initial[]={1,1,1};auto bad=pico_science::fit(d,initial,[](void*,double,const double*,double&){return false;},nullptr);assert(bad.status==0&&bad.evaluations==1);pico_science::spectrum s;assert(!s.compute(d,0,0,false)&&!s.compute(d,16,3,false));assert(s.compute(d,16,2,false)&&fabs(s.amplitude(3)-1)<.001);pico_data_table out;assert(!pico_science::filtered(d,out,16,2,1,5,[](void*){return true;}));assert(!pico_science::filtered(d,out,16,2,1,4));uint64_t v;assert(!pico_science::modpow(1000000000001ULL,1,3,v));assert(pico_science::modpow(0,0,1,v)&&v==0);puts("PASS: fit domain failure; Hamming gain and invalid FFT inputs; filter cancellation/invalid span; modular input limits");}
'''
with tempfile.TemporaryDirectory(prefix='science-edge-') as name:
 d=Path(name);(d/'test.cc').write_text(source)
 subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-I'+str(root/'pico'),str(d/'test.cc'),'-o',str(d/'test')],check=True)
 subprocess.run([str(d/'test')],check=True)
