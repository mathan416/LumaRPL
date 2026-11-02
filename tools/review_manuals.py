#!/usr/bin/env python3
"""Check every PDF page and build native-resolution contact-sheet indexes."""
from pathlib import Path
import hashlib,json,subprocess
from PIL import Image,ImageDraw,ImageFont
from pypdf import PdfReader
import pdfplumber
root=Path(__file__).resolve().parents[1];out=root/'tmp/pdfs';out.mkdir(parents=True,exist_ok=True)
font=ImageFont.truetype('/System/Library/Fonts/Supplemental/Arial.ttf',13) if Path('/System/Library/Fonts/Supplemental/Arial.ttf').exists() else ImageFont.load_default()
reports=[]
for stem,file in [('user','LumaRPL-User-Guide.pdf'),('programming','LumaRPL-Programming-Guide.pdf')]:
 pdf=root/'output/pdf'/file
 for old in out.glob(stem+'-[0-9]*.png'):old.unlink()
 subprocess.run(['pdftoppm','-r','70','-png',str(pdf),str(out/stem)],check=True)
 files=sorted(out.glob(stem+'-[0-9]*.png'))
 r=PdfReader(pdf);assert len(files)==len(r.pages)
 bounds=[];sparse=[]
 with pdfplumber.open(pdf) as doc:
  for number,page in enumerate(doc.pages,1):
   chars=[c for c in page.chars if c.get('text','').strip()]
   bad=[c['text'] for c in chars if c['x0']<-.5 or c['x1']>504.5 or c['top']<-.5 or c['bottom']>648.5]
   if bad:bounds.append(dict(page=number,characters=''.join(bad)))
   text=page.extract_text() or ''
   if number>1 and len(text)<180:sparse.append(dict(page=number,characters=len(text)))
 assert not bounds,bounds
 for start in range(0,len(files),9):
  sheet=Image.new('RGB',(490*3,650*3),'#E3E9ED');draw=ImageDraw.Draw(sheet)
  for i,path in enumerate(files[start:start+9]):
   image=Image.open(path).convert('RGB');image.thumbnail((490,630))
   x=(i%3)*490;y=(i//3)*650
   sheet.paste(image,(x,y));draw.text((x+8,y+633),f'{file} / page {start+i+1}',fill='#123047',font=font)
  sheet.save(out/(stem+'-sheet-'+str(start//9+1).zfill(2)+'.png'))
 reports.append(dict(file=file,pages=len(r.pages),outline_entries=len(r.outline),out_of_page_characters=bounds,sparse_pages=sparse,visual_review_required=True,sha256=hashlib.sha256(pdf.read_bytes()).hexdigest()))
(root/'output/pdf/manual-layout-check.json').write_text(json.dumps(reports,indent=2)+'\n')
for report in reports:print(report['file'],report['pages'],'pages; out-of-page characters:',len(report['out_of_page_characters']),'sparse:',report['sparse_pages'])
