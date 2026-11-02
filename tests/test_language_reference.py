import json,struct,subprocess,unittest,zlib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
class LanguageReference(unittest.TestCase):
 def test_generated(self):subprocess.run(['python3',str(ROOT/'tools/generate_language_references.py'),'--check'],check=True,capture_output=True)
 def test_books(self):
  for book,count in [('lua',38),('chip8',44)]:
   rows=json.loads((ROOT/'docs/help'/f'{book}-reference.json').read_text());self.assertEqual(len(rows),count)
   index=(ROOT/'resources/help'/f'{book}.lri').read_bytes();data=(ROOT/'resources/help'/f'{book}.lrt').read_bytes();self.assertEqual(struct.unpack_from('<4sIII',index),(b'LRH1',count,len(data),460));self.assertEqual(len(index),16+460*count)
   for i in range(count):
    offset,length,category,reserved,crc,title,keys=struct.unpack_from('<IHBBI64s384s',index,16+i*460);text=data[offset:offset+length];text.decode();self.assertEqual(zlib.crc32(text),crc);self.assertLessEqual(length,4096);self.assertIn(b'Example script' if book=='lua' else b'program' if b'Complete program' in text else b'Instruction fragment',text)
   self.assertTrue(all(r['source'] and r['expected'] for r in rows))
 def test_all_lua_platform_functions(self):
  names={r['name'] for r in json.loads((ROOT/'docs/help/lua-reference.json').read_text())}
  for group,functions in [('screen','clear pixel line rect circle text show sprite font measure'),('keyboard','poll wait down'),('system','sleep millis'),('sound','beep stop'),('plot','clear window function2d surface show'),('calc','eval')]:
   for name in functions.split():self.assertIn(group+'.'+name,names)
 def test_rpl_examples_visible(self):
  rows=json.loads((ROOT/'docs/help/command-examples.json').read_text());report=json.loads((ROOT/'resources/help/picocalc-reference.json').read_text());raw=(ROOT/'resources/help/commands.lrt').read_bytes();records={r['name']:r for r in report['records']}
  self.assertGreater(len(rows),250)
  for name,e in rows.items():
   r=records[name];text=raw[r['offset']:r['offset']+r['length']].decode();self.assertIn(e['source'],text);self.assertIn(e['expected'],text)
 def test_task_first_structure(self):
  navigation=json.loads((ROOT/'docs/help/language-navigation.json').read_text())
  for book in ['lua','chip8']:
   self.assertEqual(navigation[book][0],'Start here')
   rows=json.loads((ROOT/'docs/help'/f'{book}-reference.json').read_text());intro=rows[0]
   self.assertEqual(intro['category'],'Start here');self.assertTrue(intro['steps']);self.assertIn('5',intro['expected'])
  rows=json.loads((ROOT/'docs/help/chip8-reference.json').read_text());self.assertTrue(all(r['example_kind'] in ['program','snippet'] for r in rows))
 def test_routes(self):
  text=(ROOT/'pico/desktop.cc').read_text();self.assertIn('false,lua_book',text);self.assertIn('false,chip8_book',text)
if __name__=='__main__':unittest.main()
