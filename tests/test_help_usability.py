import json,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
class HelpUsability(unittest.TestCase):
 def test_guides_and_tools_are_useful_without_preambles(self):
  guide=json.loads((ROOT/'docs/ui/reference.json').read_text());tools=json.loads((ROOT/'docs/ui/tool-help.json').read_text())
  for page in guide+list(tools.values()):
   text=page['text'];self.assertGreater(len(text),150)
   for old in ['On-screen native workbench','Result is a preview','does not operate the SD source editor']:self.assertNotIn(old,text)
  self.assertIn('F5',guide[0]['text']);self.assertIn('SQDEMO',guide[2]['text'])
 def test_help_uses_native_navigation(self):
  text='\n'.join(p['text'] for p in json.loads((ROOT/'docs/ui/reference.json').read_text()))
  self.assertNotIn('F4 previous link',text);self.assertIn('Command reference',text)
 def test_details_summaries_match_guide_index(self):
  d=json.loads((ROOT/'docs/ui/menu-help.json').read_text());self.assertEqual(len(d['reference.topics']['items']),12)
  for value in d['reference.topics']['items'].values():self.assertLess(len(value),200)
if __name__=='__main__':unittest.main()
