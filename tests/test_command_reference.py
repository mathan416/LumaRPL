import unittest,json,struct,zlib,subprocess,sys,re
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'tools'))
from help_command_metadata import command_metadata
class CommandReference(unittest.TestCase):
 def test_generated(self):
  for tool in ['generate_ui_help','generate_command_reference','generate_reference_font']:subprocess.run(['python3',str(ROOT/'tools'/(tool+'.py')),'--check'],check=True,capture_output=True)
 def test_bounded_index_and_all_spellings(self):
  data=(ROOT/'resources/help/commands.lrt').read_bytes();index=(ROOT/'resources/help/commands.lri').read_bytes();magic,count,size,record=struct.unpack_from('<4sIII',index);self.assertEqual((magic,count,size,record),(b'LRH1',1042,len(data),460));self.assertEqual(len(index),16+count*record)
  found={}
  for n in range(count):
   off,length,category,reserved,crc,title,keys=struct.unpack_from('<IHBBI64s384s',index,16+n*record);name=title.split(b'\0')[0].decode();text=data[off:off+length];text.decode();self.assertEqual(zlib.crc32(text),crc);self.assertTrue(0<length<=4096);self.assertEqual(reserved,0);found[name]=keys.split(b'\0')[0].decode().split('\x1f')
  for name,c in command_metadata().items():
   if c['parseable']:
    self.assertIn(name,found)
    for alias in c['spellings']:self.assertIn(alias,found[name])
 def test_plain_text_and_fresh_sources(self):
  text=(ROOT/'resources/help/commands.lrt').read_text();self.assertNotIn('does not operate the SD source editor',text);self.assertNotIn('Legacy/reference name:',text);self.assertNotIn('For ordinary input use physical keys',text)
  source=json.loads((ROOT/'docs/help/commands.json').read_text());self.assertEqual(set(source),{n for n,c in command_metadata().items() if c['parseable'] and c['kind']=='command'})
  for kind in ['flag','setting']:
   d=json.loads((ROOT/'docs/help'/(kind+'s.json')).read_text());self.assertEqual(set(d),{n for n,c in command_metadata().items() if c['kind']==kind})
 def test_desktop_index_offsets(self):
  raw=(ROOT/'resources/help/db48x.md').read_bytes()
  for line in (ROOT/'resources/help/db48x.idx').read_bytes().splitlines():
   off,title=line.split(b':',1);self.assertTrue(raw[int(off):].startswith(title))
 def test_native_sources_and_controls(self):
  d=json.loads((ROOT/'docs/ui/menu-help.json').read_text());self.assertEqual(set(d), {e["id"] for e in json.loads((ROOT/"pico/ui/screens.json").read_text()) if e["kind"]=="menu"})
  tools=json.loads((ROOT/'docs/ui/tool-help.json').read_text());registered={e['id'] for e in json.loads((ROOT/'pico/ui/screens.json').read_text())};self.assertTrue(set(tools)<=registered);self.assertTrue(all(t['title'] and len(t['text'])>100 for t in tools.values()))
  source=(ROOT/'pico/reference_ui.inc').read_text();self.assertIn('screen_text("calculator.help"',source);self.assertNotIn('ui.load_help',source)
  engine=(ROOT/'db48x/src/user_interface.cc').read_text();self.assertIn('pico_show_command_reference(cstring(topic),len)',engine)
if __name__=='__main__':unittest.main()
