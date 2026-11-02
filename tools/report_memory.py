#!/usr/bin/env python3
import json,subprocess,argparse,struct
from pathlib import Path
root=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser()
parser.add_argument('elf',nargs='?',type=Path,default=root/'build/pico/db48x-picocalc-pico2w.elf')
elf=parser.parse_args().elf
output=subprocess.check_output(['arm-none-eabi-nm','-S','-n',str(elf)],text=True)
symbols={}
sizes={}
for line in output.splitlines():
    fields=line.split()
    if len(fields)>=3:
        try:
            symbols[fields[-1]]=int(fields[0],16)
            if len(fields)==4:sizes[fields[-1]]=int(fields[1],16)
        except ValueError:pass
report={'flash_image_bytes':symbols['__flash_binary_end']-symbols['__flash_binary_start'],
'application_partition_bytes':4194304-8192,
'internal_ram_before_heap_bytes':symbols['end']-0x20000000,
'calculator_arena_bytes':sizes['_ZZ12program_initvE5arena'],'framebuffer_bytes':sizes['_ZL11framebuffer'],'bounded_heap_bytes':symbols['__HeapLimit']-symbols['__end__'],
'reserved_stack_bytes':symbols['__StackTop']-symbols['__StackBottom'],
'unused_gap_bytes':symbols['__StackBottom']-symbols['__HeapLimit'],
'note':'Arena/framebuffer are included in internal_ram_before_heap_bytes. Linker RAM-region usage is 100% because stack is at top of RAM; unused_gap_bytes remains unallocated.'}
# Read the target's sizeof results from its ELF32 load segments, rather than
# substituting host ABI sizes or estimates.
if 'pico_memory_profile' in symbols:
    data=elf.read_bytes();assert data[:6]==b'\x7fELF\x01\x01'
    address=symbols['pico_memory_profile'];phoff=struct.unpack_from('<I',data,28)[0]
    phsize,phcount=struct.unpack_from('<HH',data,42)
    for i in range(phcount):
        kind,offset,vaddr,paddr,filesize,memsize,flags,align=struct.unpack_from('<8I',data,phoff+i*phsize)
        if kind==1 and vaddr<=address and address+28<=vaddr+filesize:
            profile=struct.unpack_from('<7I',data,offset+address-vaddr);break
    else:raise AssertionError('Memory profile is not in flash')
    report['parkable_document_payload_bytes']=sum(profile[:5])
    if sizes['pico_memory_profile']>=36:
        extra=struct.unpack_from('<2I',data,offset+address-vaddr+28)
        report['parkable_document_payload_bytes']+=sum(extra)
        report['matrix_document_bytes'],report['finance_document_bytes']=extra
    if sizes['pico_memory_profile']>=40:
        report['matrix_b_document_bytes']=struct.unpack_from('<I',data,offset+address-vaddr+36)[0]
        report['parkable_document_payload_bytes']+=report['matrix_b_document_bytes']
    if sizes['pico_memory_profile']>=48:
        additional=struct.unpack_from('<2I',data,offset+address-vaddr+40)
        report['complex_document_bytes'],report['geometry_document_bytes']=additional
        report['parkable_document_payload_bytes']+=sum(additional)
    if sizes['pico_memory_profile']>=52:
        report['math_lab_document_bytes']=struct.unpack_from('<I',data,offset+address-vaddr+48)[0]
        report['parkable_document_payload_bytes']+=report['math_lab_document_bytes']
    if sizes['pico_memory_profile']>=56:
        report['advanced_math_document_bytes']=struct.unpack_from('<I',data,offset+address-vaddr+52)[0]
        report['parkable_document_payload_bytes']+=report['advanced_math_document_bytes']
    if sizes['pico_memory_profile']>=60:
        report['science_document_bytes']=struct.unpack_from('<I',data,offset+address-vaddr+56)[0]
        report['parkable_document_payload_bytes']+=report['science_document_bytes']
    if sizes['pico_memory_profile']>=64:
        report['research_document_bytes']=struct.unpack_from('<I',data,offset+address-vaddr+60)[0]
        report['parkable_document_payload_bytes']+=report['research_document_bytes']
    report['editor_tools_internal_bytes']=profile[5]
    report['editor_undo_payload_bytes']=profile[6]
    report['shared_workspace_bytes']=next(size for name,size in sizes.items() if name.endswith('shared_workspace'))
    report['psram_chip_bytes']=8*1024*1024
    report['psram_backing_pool_bytes']=127*65536
    report['psram_physically_verified']=False
if 'pico_network_memory_profile' in symbols:
    address=symbols['pico_network_memory_profile']
    for i in range(phcount):
        kind,offset,vaddr,paddr,filesize,memsize,flags,align=struct.unpack_from('<8I',data,phoff+i*phsize)
        if kind==1 and vaddr<=address and address+8<=vaddr+filesize:
            state,client=struct.unpack_from('<2I',data,offset+address-vaddr);break
    else:raise AssertionError('Network profile is not in flash')
    report['network_application_state_bound_bytes']=state
    report['network_boot_client_heap_bytes']=client
    report['network_sdk_heap_delta_device_measured']=False
    assert state<=512
assert report['calculator_arena_bytes']>=100*1024
assert report['unused_gap_bytes']>=0 and report['reserved_stack_bytes']==32768
assert report['flash_image_bytes']<=report['application_partition_bytes']
assert symbols['__StackBottom']>=symbols['end']+report['bounded_heap_bytes']
print(json.dumps(report,indent=2))
