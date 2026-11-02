#!/usr/bin/env python3
"""Check measured firmware resources; does not establish physical acceptance."""
from pathlib import Path
import json,subprocess
root=Path(__file__).resolve().parents[1]
rows=[]
for path in (root/'build/pico').rglob('*.su'):
    for line in path.read_text().splitlines():
        fields=line.split('\t')
        if len(fields)==3:rows.append({'function':fields[0],'bytes':int(fields[1]),'kind':fields[2]})
assert rows,'Build with -fstack-usage before checking'
dynamic=[row for row in rows if 'dynamic' in row['kind']]
assert not dynamic,dynamic
symbols=subprocess.check_output(['arm-none-eabi-nm','-C',str(root/'build/pico/db48x-picocalc-pico2w.elf')],text=True)
for unwanted in ('pico_network_probe','__cxa_throw','__cxa_demangle','__gxx_personality','__cxa_begin_catch'):
    assert unwanted not in symbols,unwanted
memory=json.loads(subprocess.check_output(['python3',str(root/'tools/report_memory.py')],text=True))
for name,value in {'calculator_arena_bytes':212992,'framebuffer_bytes':102400,'bounded_heap_bytes':98304,'reserved_stack_bytes':32768}.items():
    assert memory[name]==value,(name,memory[name],value)
assert memory['unused_gap_bytes']>=0 and memory['flash_image_bytes']<=memory['application_partition_bytes']
assert 'pico_network_rates' in symbols,'Port 17 requires on-device Wi-Fi/HTTPS'
report={'supported_variant':'colour','memory':memory,'variable_stack_frames':dynamic,'stack_records':len(rows),'largest_static_frames':sorted(rows,key=lambda r:r['bytes'],reverse=True)[:12],'exception_runtime_absent':True,'physical_acceptance':False}
print(json.dumps(report,indent=2))
