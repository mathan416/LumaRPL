#!/usr/bin/env python3
"""Bounded, checksummed examples for the port-21 document codecs."""
from pathlib import Path
import struct
root=Path(__file__).resolve().parents[1]/'resources'
def save(folder,name,data):
 h=2166136261
 for b in data[:-4]:h=((h^b)*16777619)&0xffffffff
 struct.pack_into('<I',data,len(data)-4,h)
 (root/folder).mkdir(exist_ok=True);(root/folder/name).write_bytes(data)
data=bytearray(144);data[:8]=b'LUMACPX\1'
for i,text in enumerate([b'3',b'4',b'1',b'-2']):data[12+i*32:12+i*32+len(text)]=text
save('complex','BASICS.complex',data)
data=bytearray(144);data[:8]=b'LUMAGEO\1';data[8]=7;struct.pack_into('<16d',data,12,-4,-2,4,2,-3,3,3,-3,0,0,2,0,-6,6,-6,6)
save('geometry','LINES.geometry',data)
data=bytearray(224);data[:8]=b'LUMAFIN\2';data[8]=3;struct.pack_into('<26d',data,12,10000,5,60,12,5,-100,230,-132,*([0]*14),188.7123364401093,100,0,100)
save('finance','DOUBLEIRR.finance',data)
print('PASS: complex, geometry and version-two finance examples generated')
