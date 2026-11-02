#!/usr/bin/env python3
"""Generate bounded port-20 example documents using their version-one wire layouts."""
from pathlib import Path
import struct
root=Path(__file__).resolve().parents[1]/'resources'
def finish(data):
 h=2166136261
 for b in data[:-4]:h=((h^b)*16777619)&0xffffffff
 struct.pack_into('<I',data,len(data)-4,h);return data
for name,rows in {'FRACTION':[['1/3','0'],['0','2']],'SOLVE':[['1','1','3'],['1','-1','1']],'EIGEN':[['2','1'],['1','2']],'MATRIXA':[['1','2'],['3','4']],'MATRIXB':[['2','0'],['0','3']]}.items():
 data=bytearray(1168);data[:8]=b'LUMAMAT\1';data[8:10]=bytes([len(rows),len(rows[0])])
 for r in range(6):
  for c in range(6):
   text=(rows[r][c] if r<len(rows) and c<len(rows[0]) else '0').encode();at=12+(r*6+c)*32;data[at:at+len(text)]=text
 path=root/'matrices'/(name+'.matrix');path.parent.mkdir(exist_ok=True);path.write_bytes(finish(data))
data=bytearray(192);data[:8]=b'LUMAFIN\1';data[8]=4;struct.pack_into('<22d',data,12,10000,5,60,12,5,-1000,400,400,400,*([0]*13))
path=root/'finance/LOAN.finance';path.parent.mkdir(exist_ok=True);path.write_bytes(finish(data));print('PASS: matrix/finance examples generated')
