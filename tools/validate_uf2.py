#!/usr/bin/env python3
"""Validate the application against the user's real UF2 Loader partition."""
import argparse,hashlib,json,re,struct,subprocess,sys
from pathlib import Path
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser()
p.add_argument('firmware',type=Path)
p.add_argument('--loader',type=Path,default=root/'PicoCalc-SD-Pico2W/Install-on-Pico2W/bootloader_pico2.uf2')
a=p.parse_args()
tool=root/'build/picotool/picotool'
capacity=4*1024*1024-8192
loader_hash=None
if a.loader.exists():
    metadata=subprocess.check_output([str(tool),'info','-a',str(a.loader)],text=True)
    match=re.search(r'partition 0 \(A\):\s*([0-9a-fA-F]+)->([0-9a-fA-F]+)',metadata)
    assert match,'Loader partition table missing'
    start,end=(int(v,16) for v in match.groups())
    assert start==8192 and end==4*1024*1024,'Unexpected loader application partition'
    assert re.search(r'version:\s+2\.4\.1\b',metadata),'Expected UF2 Loader v2.4.1'
    capacity=end-start
    loader_hash=hashlib.sha256(a.loader.read_bytes()).hexdigest()
else:
    print('Loader file absent: checking the v2.4.1 4 MiB board contract only',file=sys.stderr)
data=a.firmware.read_bytes()
assert data and len(data)%512==0,'Incomplete UF2'
seen=set();ranges=[];total=None;workaround=0
for offset in range(0,len(data),512):
    block=data[offset:offset+512]
    m0,m1,flags,address,size,index,count,family=struct.unpack_from('<8I',block)
    assert (m0,m1,struct.unpack_from('<I',block,508)[0])==(0x0a324655,0x9e5d5157,0x0ab16f30),'UF2 magic'
    if family==0xe48bff57:
        # SDK 2.2 E10 dummy block: v2.4.1 explicitly skips this exact form.
        assert offset==0 and flags==0xa000 and address==0x10ffff00
        assert size==256 and index==0 and count==2
        workaround+=1
        continue
    assert flags==0x2000 and family==0xe48bff59,'Expected RP2350 Arm Secure family'
    if total is None: total=count
    assert not flags&1,'Non-flash UF2 block'
    assert size==256 and index not in seen and count==total,'UF2 block sequencing'
    assert 0x10000000<=address and address+size<=0x10000000+capacity,'Block outside remapped app partition'
    seen.add(index);ranges.append((address,address+size))
assert total and seen==set(range(total))
assert len(data)//512==total+workaround
ranges.sort()
for before,after in zip(ranges,ranges[1:]):assert before[1]<=after[0],'Overlapping UF2 payloads'
assert ranges[0][0]==0x10000000,'Not a standard SDK application'
metadata=subprocess.check_output([str(tool),'info','-a',str(a.firmware)],text=True)
application=re.search(r'^\s*name:\s*(.*?)\s*$',metadata,re.M)
board=re.search(r'^\s*pico_board:\s*(.*?)\s*$',metadata,re.M)
assert board and board.group(1)=='pico2_w' and application and application.group(1) in ('LumaRPL','DB48X PicoCalc Colour'),'Board/application metadata'
report={'firmware':a.firmware.name,'application_name':application.group(1),'sha256':hashlib.sha256(data).hexdigest(),
        'family':'rp2350-arm-s','blocks':total,'sdk_e10_workaround_blocks':workaround,'application_payload_extent':ranges[-1][1]-0x10000000,
        'application_partition_bytes':capacity,'loader_sha256':loader_hash,
        'loader_contract':'UF2 Loader v2.4.1; standard SDK logical addresses remapped into App at physical offset 8192',
        'hardware_boot_tested':False}
print(json.dumps(report,indent=2))
