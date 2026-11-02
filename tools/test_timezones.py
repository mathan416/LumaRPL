#!/usr/bin/env python3
"""Compare named timezone rules against system IANA data at daily UTC samples."""
from pathlib import Path
from datetime import datetime,timezone,timedelta
from zoneinfo import ZoneInfo
import subprocess
root=Path(__file__).resolve().parents[1];out=root/'build/timezone-vectors.inc'
names=['America/Toronto','UTC','America/Vancouver','America/Denver','America/Chicago','America/Halifax','America/St_Johns','Europe/London','Europe/Berlin','Asia/Tokyo','Asia/Kolkata','Australia/Sydney','Pacific/Auckland']
# Use compact data rather than generating hundreds of thousands of code blocks.
with out.open('w') as stream:
    stream.write('struct vector {unsigned index;long long utc;int offset;};static const vector vectors[]={\n')
    for index,name in enumerate(names):
        zone=ZoneInfo(name);day=datetime(2020,1,1,tzinfo=timezone.utc)
        while day.year<2038:
            for hour in (0,6,12,18):
                instant=day+timedelta(hours=hour);offset=int(instant.astimezone(zone).utcoffset().total_seconds())//60
                stream.write(f'{{{index},{int(instant.timestamp())}LL,{offset}}},\n')
            day+=timedelta(days=1)
    stream.write('};for(const auto &v:vectors){pico_timezone z;z.minutes=pico_timezones[v.index].minutes;z.rule=pico_timezones[v.index].rule;if(z.offset(v.utc)!=v.offset){fprintf(stderr,"Timezone mismatch index %u UTC %lld expected %d, got %d\\n",v.index,v.utc,v.offset,z.offset(v.utc));return 1;}}\n')
subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-DTIMEZONE_VECTORS','-I'+str(root/'pico'),'-I'+str(root/'build'),str(root/'tests/timezone_test.cc'),'-o',str(root/'build/timezone-test')],check=True)
subprocess.run([str(root/'build/timezone-test')],check=True)
print('PASS: 2020–2037 named timezone offsets against system IANA data (four samples/day)')
