#!/usr/bin/env python3
"""Build LumaRPL's independently written, bounded native command reference."""
from pathlib import Path
import argparse,json,re,struct,zlib
from help_command_metadata import command_metadata
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--check',action='store_true');args=p.parse_args()
meta=command_metadata();written=json.loads((root/'docs/help/commands.json').read_text())
# These files describe the engine's settings, not inherited manual prose.
settings=json.loads((root/'docs/help/settings.json').read_text())
flags=json.loads((root/'docs/help/flags.json').read_text())
examples=json.loads((root/'docs/help/examples.json').read_text())
worked=json.loads((root/'docs/help/command-examples.json').read_text())
lookup={s.casefold():c['id'] for c in meta.values() if c['parseable'] for s in c['spellings']}
by_command={}
for e in examples:
 command=lookup.get(e['source'].split()[-1].casefold())
 if command:by_command.setdefault(command,e)
def words(s):return re.sub(r'(?<=[a-z])(?=[A-Z])',' ',s).lower()
choices={'Std':'Use standard number display. The calculator chooses a suitable decimal or exponent form.', 'Sig':'Choose how many significant digits to display.', 'Fix':'Choose how many digits to show after the decimal point. For example, 3 Fix displays 1.25 as 1.250.', 'Sci':'Show numbers in scientific notation with the supplied digit count.', 'Eng':'Show numbers in engineering notation, with exponents in multiples of three and the supplied digit count.', 'Deg':'Use degrees for angles: a full turn is 360.', 'Rad':'Use radians for angles: a full turn is 2*pi.', 'Grad':'Use grads for angles: a full turn is 400.', 'PiRadians':'Measure angles in multiples of pi radians: 2 is a full turn, and 0.5 is a right angle.', 'Bin':'Show based integers in binary (base 2).', 'Oct':'Show based integers in octal (base 8).', 'Dec':'Show based integers in decimal (base 10).', 'Hex':'Show based integers in hexadecimal (base 16).', 'SingleRowMenus':'Show one calculator soft-menu row at a time.', 'ThreeRowsMenus':'Show the calculator soft-menu rows together.', 'FlatMenus':'Use the engine flat-menu layout.'}
for name,base in [('Bin',2),('Oct',8),('Dec',10),('Hex',16)]:
 choices[name]=f'Show based integers in base {base}. On LumaRPL this also makes a fixed-base integer at the top follow the selected display base, without changing its value. Fixed-base values below the top retain their representation. Ordinary decimal integers at the top convert directly too; decimal fractions remain unchanged. Negative integers use the current WordSize, as with RealToBinary. Ctrl+Y opens Bases from the calculator.'
rows=[]
for name,c in meta.items():
 if not c['parseable']:continue
 kind=c['kind'];aliases=list(dict.fromkeys(c['spellings']));body=''
 if kind=='command':
  assert name in written,name
  category=written[name]['category'];body=written[name]['text']
 elif kind=='setting':
  category='Calculator settings';body=settings[name]
  if c['group'].isdigit() and c['high'].isdigit():body+=' The allowed values are '+c['group']+' to '+c['high']+'.'
  body+='\n\nTo change it, enter a value followed by '+name+'. Read the current value with \''+name+"' RCL."
 elif kind=='flag':
  category='Calculator settings';body=flags[name]
 elif kind=='preset':
  category='Calculator settings'
  body=choices.get(name)
  if body is None:
   if c['group']=='CommandDisplayMode':body='Choose '+words(name)+' rendering for calculator command names.'
   elif c['group']=='NameDisplayMode':body='Choose '+words(name.removesuffix('Names'))+' rendering for symbolic names.'
   elif c['group'] in ('NumberSeparatorCommand','BasedSeparatorCommand'):
    what='based-integer' if name.startswith('Based') else 'decimal';style={'Spaces':'spaces','DotOrComma':'dots or commas','Ticks':'ticks','Underscore':'underscores'}
    suffix=name.removeprefix('Based').removeprefix('Number');body='Separate groups of '+what+' digits with '+style[suffix]+'.'
   elif c['group']=='DateSeparatorCommand':body='Use '+{'DateSlash':'slashes','DateDash':'dashes','DateDot':'dots','DateSpace':'spaces'}[name]+' between the parts of displayed dates.'
   else:raise AssertionError(name)
  body+='\n\n'+('Put the digit count on the stack first.' if name in ('Sig','Fix','Sci','Eng') else 'Enter the command without a stack value.')+' This choice stays active until you select another; choosing it again does not turn it off.'
 elif kind=='menu':
  category='Calculator menus';label=words(name.removesuffix('Menu'))
  body='Open the calculator '+label+' soft menu. Type '+name+' and press Enter.\n\nTab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc restores the parent menu page and Tab-selected row, then clears the top-level menu.'
 else:raise AssertionError((name,kind))
 other=[s for s in aliases if s!=name]
 if other:body+='\n\nYou can also enter: '+', '.join(other)+'.'
 if name in worked or name in by_command:
  e=worked.get(name) or by_command[name]
  start=body.find('Example: ')
  if start>=0 and e['source'] in body[start:]:
   end=body.find('. ',start)
   body=body[:start]+(body[end+2:] if end>=0 else '')
  body=body.rstrip()+'\n\nTry it\n'+e['source']+'\n\nYou should get: '+e['expected']
  if e.get('note'):body+='\n'+e['note']
 elif kind=='setting':
  body+='\n\nTry it\n'+"'"+name+"' RCL"+'\n\nShows the current value. To change it, replace the recalled value with your chosen value and enter '+name+'. Save the session to keep it after restart.'
 elif kind=='flag':
  body+='\n\nTry it\n'+name+'\n\nApplies the choice described above without a stack argument. Enter '+c['group']+' to choose its opposite. Save the session to keep this preference.'
 elif kind=='menu':
  body+='\n\nTry it\n'+name+'\n\nThe soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.'
 elif kind=='preset':
  body+='\n\nTry it\n'+('3 ' if name in ('Sig','Fix','Sci','Eng') else '')+name+'\n\n'+('Shows three digits using this display style.' if name in ('Sig','Fix','Sci','Eng') else 'Applies the display or angle choice described above.')
 rows.append(dict(name=name,category=category,text=body,spellings=list(dict.fromkeys([name]+aliases))))
assert len(rows)==sum(c["parseable"] for c in meta.values())
command_count=len(rows)
for name,text in json.loads((root/'docs/help/topics.json').read_text()).items():rows.append(dict(name=name,category='Using RPL',text=text,spellings=[name]))
categories=sorted({r['category'] for r in rows});assert len(categories)<=32
rows.sort(key=lambda r:(categories.index(r['category']),r['name'].casefold()))
index=bytearray();data=bytearray();markdown='# LumaRPL command reference\n\nBrowse commands by subject, or use Find. These pages use the same arrows and Back key as the rest of Help.\n\n'
records=[]
for r in rows:
 encoded=r['text'].encode();assert 0<len(encoded)<=4096,(r['name'],len(encoded))
 title=r['name'].encode();keys=('\x1f'+'\x1f'.join(r['spellings'])+'\x1f').encode();assert len(title)<64 and len(keys)<384,(r['name'],len(keys))
 offset=len(data);crc=zlib.crc32(encoded);category=categories.index(r['category'])
 index+=struct.pack('<IHBBI64s384s',offset,len(encoded),category,0,crc,title,keys);data+=encoded
 records.append(dict(name=r['name'],category=r['category'],offset=offset,length=len(encoded),crc32=crc))
 markdown+='## '+r['name']+'\n\n'+r['text']+'\n\n'
# Keep accepted spellings searchable by the upstream desktop reader too.
for r in rows:
 for alias in r['spellings']:
  if alias.casefold()!=r['name'].casefold():markdown+='## '+alias+'\n\n'+r['text']+'\n\n'
md=markdown.encode();md_index=b''
for m in re.finditer(rb'(?m)^#+ ([^\n]+)',md):md_index+=str(m.start()).encode()+b':'+m[0]+b'\n'
header=struct.pack('<4sIII',b'LRH1',len(rows),len(data),460)
# Category ranges let ordinary browsing read just one eight-row page from SD.
ranges=[]
for category in categories:
 ids=[i for i,r in enumerate(rows) if r['category']==category];ranges.append((ids[0],len(ids)))
cpp='// Generated by tools/generate_native_reference.py. GPL-3.0.\n#pragma once\nnamespace pico_reference_data {\ninline constexpr unsigned record_count='+str(len(rows))+';\nstruct Category { const char *name; unsigned first,count; };\ninline constexpr Category categories[]={\n'+''.join('{'+json.dumps(c)+','+str(a)+','+str(b)+'},\n' for c,(a,b) in zip(categories,ranges))+'};\n}\n'
report=dict(format='LumaRPL native help v1',command_count=command_count,topic_count=len(rows),categories=categories,record_bytes=460,max_topic_bytes=max(r['length'] for r in records),source='docs/help/commands.json',records=records)
outputs={'resources/help/commands.lri':header+index,'resources/help/commands.lrt':bytes(data),'resources/help/db48x.md':md,'resources/help/db48x.idx':md_index,'resources/help/picocalc-reference.json':(json.dumps(report,ensure_ascii=False,indent=2)+'\n').encode(),'pico/ui/reference_data.h':cpp.encode()}
for name,content in outputs.items():
 path=root/name
 if args.check:assert path.read_bytes()==content,('Regenerate native reference',name)
 else:path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes(content)
print('PASS: freshly authored native reference:',command_count,'commands/settings and',len(rows)-command_count,'guide topics,',len(categories),'subjects; topic limit 4096 bytes')
