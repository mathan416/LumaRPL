#!/usr/bin/env python3
"""Build illustrated, searchable LumaRPL manuals from maintained sources.

Requires reportlab, Pillow and pypdf. Set LUMARPL_PDF_FONTS to a directory
containing DejaVuSans{,-Bold,-Oblique}.ttf and DejaVuSansMono.ttf.
"""
from pathlib import Path
import argparse, hashlib, html, json, os, re, textwrap, unicodedata
from reportlab import rl_config
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.lib import colors
from reportlab.lib.styles import ParagraphStyle
from reportlab.lib.utils import ImageReader
from reportlab.platypus import (BaseDocTemplate, PageTemplate, Frame, Paragraph,
    Spacer, PageBreak, KeepTogether, Preformatted, Table, TableStyle, Image, Flowable)
from reportlab.platypus.tableofcontents import TableOfContents, SimpleIndex
from pypdf import PdfReader

ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'docs/manuals'
OUT=ROOT/'output/pdf'
VERSION=(ROOT/'VERSION').read_text().strip()
FALLBACK=Path.home()/'.cache/codex-runtimes/codex-primary-runtime/dependencies/native/libreoffice-headless/libreoffice/LibreOfficeDev.app/Contents/Resources/fonts/truetype'
FONTS=Path(os.environ.get('LUMARPL_PDF_FONTS',str(FALLBACK)))
W,H=504,648
LEFT,RIGHT,TOP,BOTTOM=48,42,49,43
CW=W-LEFT-RIGHT
NAVY=colors.HexColor('#123047')
INK=colors.HexColor('#192B36')
MUTED=colors.HexColor('#526873')
PALE=colors.HexColor('#EDF3F6')
for alias,name in [('Body','DejaVuSans.ttf'),('Bold','DejaVuSans-Bold.ttf'),('Italic','DejaVuSans-Oblique.ttf'),('Mono','DejaVuSansMono.ttf')]:
    pdfmetrics.registerFont(TTFont(alias,str(FONTS/name)))
pdfmetrics.registerFontFamily('Body',normal='Body',bold='Bold',italic='Italic',boldItalic='Bold')
rl_config.warnOnMissingFontGlyphs=1
ST={
 'body':ParagraphStyle('body',fontName='Body',fontSize=10,leading=14.4,textColor=INK,spaceAfter=8.5,allowWidows=0,allowOrphans=0),
 'h1':ParagraphStyle('h1',fontName='Bold',fontSize=23,leading=29,textColor=NAVY,spaceAfter=19,keepWithNext=True),
 'h2':ParagraphStyle('h2',fontName='Bold',fontSize=12.4,leading=16,textColor=NAVY,spaceBefore=13,spaceAfter=7,keepWithNext=True),
 'h3':ParagraphStyle('h3',fontName='Bold',fontSize=10.5,leading=14,textColor=NAVY,spaceBefore=9,spaceAfter=5,keepWithNext=True),
 'small':ParagraphStyle('small',fontName='Body',fontSize=8.2,leading=11.2,textColor=MUTED,spaceAfter=8),
 'caption':ParagraphStyle('caption',fontName='Italic',fontSize=8.1,leading=11,textColor=MUTED,alignment=1,spaceBefore=5,spaceAfter=12),
 'cell':ParagraphStyle('cell',fontName='Body',fontSize=8.5,leading=11.8,textColor=INK),
 'th':ParagraphStyle('th',fontName='Bold',fontSize=8.5,leading=11.8,textColor=colors.white),
 'note':ParagraphStyle('note',fontName='Body',fontSize=9.5,leading=13.5,textColor=NAVY,backColor=PALE,borderPadding=9,spaceBefore=7,spaceAfter=13),
 'code':ParagraphStyle('code',fontName='Mono',fontSize=9.2,leading=12.5,textColor=INK,backColor=PALE,borderPadding=9,spaceBefore=5,spaceAfter=13),
 'legal':ParagraphStyle('legal',fontName='Body',fontSize=7.4,leading=10,textColor=INK,spaceAfter=6),
}

def norm(text):
    # Keep code delimiters/Greek intact; normalize only problematic dash/space variants.
    for old in ('\u2010','\u2011','\u2012','\u2013','\u2014','\u2212'):
        text=text.replace(old,'-')
    return text.replace('\u00a0',' ').replace('\u205f',' ').replace('\u202f',' ')

def inline(text):
    text=norm(text)
    chunks=re.split(r'(`[^`]+`)',text)
    values=[]
    for chunk in chunks:
        if chunk.startswith('`') and chunk.endswith('`'):
            values.append('<font name="Mono" size="9">'+html.escape(chunk[1:-1])+'</font>')
        else:
            chunk=html.escape(chunk)
            chunk=re.sub(r'\*\*(.+?)\*\*',r'<b>\1</b>',chunk)
            chunk=re.sub(r'\[([^\]]+)\]\((https?://[^)]+)\)',r'<link href="\2" color="#13637B">\1</link>',chunk)
            values.append(chunk)
    return ''.join(values)

class Rule(Flowable):
    def __init__(self,colour=NAVY,width=CW):
        Flowable.__init__(self);self.width=width;self.height=7;self.colour=colour
    def draw(self):
        self.canv.setFillColor(self.colour);self.canv.rect(0,2,35,2,fill=1,stroke=0)

class Cover(Flowable):
    def __init__(self,title,subtitle,accent):
        Flowable.__init__(self);self.width=CW;self.height=H-TOP-BOTTOM-2
        self.title=title;self.subtitle=subtitle;self.accent=accent
    def draw(self):
        c=self.canv;c.saveState();c.translate(-LEFT,-BOTTOM-2)
        c.setFillColor(NAVY);c.rect(0,0,W,H,fill=1,stroke=0)
        c.setFillColor(self.accent);c.rect(40,H-35,34,3,fill=1,stroke=0)
        c.setFont('Bold',34);c.setFillColor(colors.white);c.drawString(40,H-84,'LumaRPL')
        c.setFont('Body',19);c.drawString(42,H-115,self.title)
        c.setFont('Body',9.5);c.setFillColor(colors.HexColor('#D6E8EE'));c.drawString(42,H-139,self.subtitle)
        # Clip the original oriented photograph to an editorial cover window.
        photo=ImageReader(str(SOURCE/'assets/cover-photo.jpg'));iw,ih=photo.getSize()
        x,y,w,h=40,78,424,398
        scale=max(w/iw,h/ih)
        c.saveState();clip=c.beginPath();clip.rect(x,y,w,h);c.clipPath(clip,stroke=0)
        c.drawImage(photo,x+(w-iw*scale)/2,y+(h-ih*scale)/2,width=iw*scale,height=ih*scale)
        c.restoreState()
        c.setFillColor(self.accent);c.rect(40,67,424,2,fill=1,stroke=0)
        c.setFont('Body',8.4);c.setFillColor(colors.white);c.drawString(42,46,'ClockworkPi PicoCalc  |  Raspberry Pi Pico 2 W')
        c.setFillColor(colors.HexColor('#D6E8EE'));c.drawString(42,29,'First edition  |  '+VERSION+'  |  November 2026')
        c.restoreState()

class Book(BaseDocTemplate):
    def __init__(self,path,title):
        BaseDocTemplate.__init__(self,str(path),pagesize=(W,H),leftMargin=LEFT,rightMargin=RIGHT,
          topMargin=TOP,bottomMargin=BOTTOM,title='LumaRPL '+title,author='LumaRPL; design direction and device testing: Iain Bennett',
          subject='LumaRPL on ClockworkPi PicoCalc / Pico 2 W',pageCompression=1)
        self.booktitle=title;self.chapter='';self.heading_count=0;self.headings=[]
        frame=Frame(LEFT,BOTTOM,CW,H-TOP-BOTTOM,leftPadding=0,rightPadding=0,topPadding=0,bottomPadding=0,id='body')
        self.addPageTemplates(PageTemplate(id='book',frames=[frame],onPage=self.chrome))
    def beforeDocument(self):
        self.chapter='';self.heading_count=0;self.headings=[]
    def chrome(self,c,doc):
        if doc.page==1:return
        c.saveState();c.setFillColor(MUTED);c.setFont('Body',7.5)
        c.drawString(LEFT,H-27,'LumaRPL  /  '+self.booktitle)
        c.setStrokeColor(colors.HexColor('#CEDBE2'));c.setLineWidth(.4);c.line(LEFT,32,W-RIGHT,32)
        c.drawString(LEFT,20,'PicoCalc  |  '+VERSION)
        c.drawRightString(W-RIGHT,20,str(doc.page));c.restoreState()
    def afterFlowable(self,flow):
        if isinstance(flow,Paragraph) and flow.style.name=='h3':
            text=flow.getPlainText()
            if re.match(r'^(?:screen|plot|keyboard|system|sound|calc)\.|^[A-Z][A-Z0-9]*$',text):
                self.notify('IndexEntry',(text,self.page))
        if isinstance(flow,Paragraph) and flow.style.name in ('h1','h2'):
            text=flow.getPlainText();self.heading_count+=1;key='heading-'+str(self.heading_count)
            level=0 if flow.style.name=='h1' else 1
            self.canv.bookmarkPage(key)
            self.canv.addOutlineEntry(text,key,level=level,closed=level==0)
            if level==0:
                self.chapter=text
                if text not in ('About this edition','Contents'):
                    self.notify('TOCEntry',(0,text,self.page,key))
            else:
                self.notify('IndexEntry',(text,self.page))
            self.headings.append(dict(text=text,page=self.page,level=level))


def para(text,style='body'):
    return Paragraph(inline(text),ST[style])

class CodeBlock(Preformatted):
    def split(self, availWidth, availHeight):
        return []

def code(lines,language=''):
    result=[]
    if language:
        names={'rpl':'Calculator / RPL source','lua':'Lua','asm':'CHIP-8 assembly','text':'Worked notation'}
        result.append(Paragraph(inline(names.get(language,language)), ParagraphStyle('code-label',parent=ST['small'],keepWithNext=True)))
    data=norm('\n'.join(lines))
    # Wrap at whitespace only, keeping tokens intact. Source examples are also
    # retained unwrapped in maintained Markdown/JSON for exact copying.
    wrapped=[]
    for line in data.splitlines():
        if len(line)<=74:wrapped.append(line);continue
        indent=' '*(len(line)-len(line.lstrip()))
        wrapped.extend(textwrap.wrap(line,width=74,subsequent_indent=indent+'  ',break_long_words=False,break_on_hyphens=False,replace_whitespace=False))
    data='\n'.join(wrapped)
    longest=max([pdfmetrics.stringWidth(line,'Mono',9.2) for line in data.splitlines()]+[0])
    size=9.2 if longest<=CW-20 else max(7.8,9.2*(CW-20)/longest)
    style=ParagraphStyle('code-instance',parent=ST['code'],fontSize=size,leading=size*1.36)
    result.append(CodeBlock(data,style))
    # Examples fit a page; never separate a program body across a page turn.
    return result

def table(rows):
    count=len(rows[0]);assert all(len(row)==count for row in rows),rows
    if count==2:widths=[CW*.52,CW*.48] if rows[0][0]=='Signature' else [CW*.32,CW*.68]
    elif count==3:widths=[CW*.22,CW*.39,CW*.39]
    else:widths=[CW/count]*count
    cells=[[Paragraph(inline(v),ST['th' if i==0 else 'cell']) for v in row] for i,row in enumerate(rows)]
    t=Table(cells,colWidths=widths,repeatRows=1,hAlign='LEFT')
    t.setStyle(TableStyle([('BACKGROUND',(0,0),(-1,0),NAVY),('VALIGN',(0,0),(-1,-1),'TOP'),
      ('ROWBACKGROUNDS',(0,1),(-1,-1),[colors.white,PALE]),('LEFTPADDING',(0,0),(-1,-1),7),
      ('RIGHTPADDING',(0,0),(-1,-1),7),('TOPPADDING',(0,0),(-1,-1),7),('BOTTOMPADDING',(0,0),(-1,-1),7),
      ('LINEBELOW',(0,0),(-1,0),.5,NAVY),('LINEBELOW',(0,1),(-1,-1),.3,colors.HexColor('#D8E3E8'))]))
    return [t,Spacer(1,10)]

def reference(which):
    data=json.loads((ROOT/'docs/help'/(which+'-reference.json')).read_text())
    blocks=[];last=None
    navigation=json.loads((ROOT/'docs/help/language-navigation.json').read_text())[which]
    for category in navigation:
        selected=[entry for entry in data if entry['category']==category]
        blocks.append(Paragraph(html.escape(category),ST['h2']))
        for item in selected:
            blocks.append(Paragraph(html.escape(norm(item.get('title',item['name']))),ST['h3']))
            if item.get('signature'):blocks+=code([item['signature']])
            blocks.append(para(item['text']))
            if item.get('steps'):
                for n,step in enumerate(item['steps'],1):blocks.append(para(str(n)+'. '+step))
            if item.get('source'):
                blocks.append(para('Fragment' if item.get('example_kind')=='snippet' else 'Example script / program','small'))
                blocks+=code(item['source'].splitlines())
            if item.get('expected'):blocks.append(para('**What to expect:** '+item['expected']))
            if item.get('after'):blocks.append(para(item['after']))
    return blocks

def markdown(text):
    lines=text.splitlines();i=0;blocks=[];first=True
    while i<len(lines):
        line=lines[i].strip()
        if not line:i+=1;continue
        if line.startswith('# '):
            if blocks:blocks.append(PageBreak())
            blocks.append(Paragraph(html.escape(norm(line[2:])),ST['h1']));blocks.append(Rule());i+=1;continue
        if line.startswith('## '):blocks.append(Paragraph(html.escape(norm(line[3:])),ST['h2']));i+=1;continue
        if line.startswith('### '):blocks.append(Paragraph(html.escape(norm(line[4:])),ST['h3']));i+=1;continue
        if line.startswith('- '):
            blocks.append(Paragraph(inline(line[2:]),ParagraphStyle('bullet',parent=ST['body'],leftIndent=12,bulletFontName='Body',bulletFontSize=10),bulletText='•'));i+=1;continue
        if line.startswith('```'):
            language=line[3:];data=[];i+=1
            while i<len(lines) and not lines[i].startswith('```'):data.append(lines[i]);i+=1
            blocks+=code(data,language);i+=1;continue
        if line.startswith('|'):
            rows=[]
            while i<len(lines) and lines[i].strip().startswith('|'):
                row=[v.strip() for v in lines[i].strip().strip('|').split('|')]
                if not all(re.fullmatch(r':?-+:?',v) for v in row):rows.append(row)
                i+=1
            blocks+=table(rows);continue
        match=re.fullmatch(r'!\[([^\]]+)\]\(([^)]+)\)',line)
        if match:
            path=SOURCE/match[2];image=Image(str(path),width=210,height=210);image.hAlign='CENTER'
            blocks.append(KeepTogether([image,para(match[1],'caption')]))
            i+=1;continue
        if line.startswith('@lua-reference'):blocks+=reference('lua');i+=1;continue
        if line.startswith('@chip8-reference'):blocks+=reference('chip8');i+=1;continue
        if line.startswith('> '):blocks.append(para(line[2:],'note'));i+=1;continue
        values=[line];i+=1
        numbered=bool(re.match(r'\d+\. ',line))
        while i<len(lines) and lines[i].strip() and not re.match(r'^(#|\||```|!\[|@|>|- |\d+\. )',lines[i].strip()):
            values.append(lines[i].strip());i+=1
        blocks.append(para(' '.join(values)))
    return blocks

def credits(letter):
    blocks=[PageBreak(),Paragraph(letter+' Credits and further reading',ST['h1']),Rule(),
      para('LumaRPL design direction and device testing: **Iain Bennett**. Cover photograph supplied by Iain Bennett. The following components and people make the calculator and its development possible.'),
      para('The combined PicoCalc port is distributed under GPL-3.0. Components and fonts retain their own notices and license terms. The summary below does not replace the complete license texts shipped with the corresponding source and license bundle.')]
    for item in json.loads((SOURCE/'credits.json').read_text()):
        if item['name']=='Iain Bennett':continue
        blocks.append(Paragraph(html.escape(item['name']),ST['h3']))
        blocks.append(para(item['authors']+'. '+item['role']+'. '+item['terms']+'.'))
        if item['url']:blocks.append(para('['+item['url']+']('+item['url']+')','small'))
    blocks.append(Paragraph('Read next',ST['h2']))
    refs=[('DB48X source and documentation','https://github.com/c3d/db48x'),('Lua 5.4 reference manual','https://www.lua.org/manual/5.4/'),
      ('PicoCalc hardware and keyboard resources','https://github.com/clockworkpi/PicoCalc'),('UF2 Loader source and workflow','https://github.com/pelrun/uf2loader'),
      ('Frankfurter API and provider information','https://frankfurter.dev/'),('HP 48G Series User\'s Guide, Hewlett-Packard','https://www.hp.com/ctg/Manual/c00442262.pdf'),
      ('HP 48G Series Advanced User\'s Reference, Hewlett-Packard','https://h10032.www1.hp.com/ctg/Manual/c00748603.pdf')]
    for title,url in refs:blocks.append(para('['+title+']('+url+')'))
    blocks.append(para('The HP books are historical documentation-design inspiration and further reading. The prose, procedures and examples here were prepared for LumaRPL; use the PicoCalc controls and current LumaRPL reference when following this book.'))
    return blocks

def legal(letter):
    blocks=[PageBreak(),Paragraph(letter+' PDF font license notices',ST['h1']),Rule(),para('These notices apply to the DejaVu fonts embedded in the PDF, not to the PicoCalc firmware. The complete text is retained in the manual source as PDF-FONT-NOTICES.txt.','small')]
    # Both embedded font families carry the same distribution terms; retain the
    # complete font metadata notice without a second duplicate copy.
    text=(SOURCE/'PDF-FONT-NOTICES.txt').read_text().split('\n\nDejaVuSansMono.ttf\n',1)[0]
    text=text.replace('DejaVuSans.ttf','DejaVu Sans and DejaVu Sans Mono',1)
    for p in re.split(r'\n\s*\n',text):blocks.append(Paragraph(html.escape(norm(' '.join(p.splitlines()))),ST['legal']))
    return blocks

def lint_glyphs(text):
    missing=set()
    cmap=pdfmetrics.getFont('Body').face.charToGlyph
    for c in norm(text):
        if ord(c)>31 and c not in '\n\r\t' and ord(c) not in cmap:missing.add(c)
    if missing:raise RuntimeError('Missing PDF glyphs: '+repr(missing))

def build(name,title,subtitle,accent,credit_letter,legal_letter):
    text=(SOURCE/(name+'.md')).read_text();lint_glyphs(text)
    doc=Book(OUT/('LumaRPL-'+title.replace(' & ',' ').replace(' ','-')+'.pdf'),title)
    toc=TableOfContents();toc.levelStyles=[ParagraphStyle('toc',fontName='Body',fontSize=9.5,leading=14,textColor=INK,leftIndent=0,firstLineIndent=0,spaceBefore=5,rightIndent=20)]
    toc.dotsMinLevel=0
    index=SimpleIndex(dot=' . ',headers=True);index.textStyle=ParagraphStyle('index',fontName='Body',fontSize=9,leading=12)
    blocks=[Cover(title,subtitle,colors.HexColor(accent)),PageBreak(),Paragraph('About this edition',ST['h1']),Rule(),
      para('LumaRPL '+title+' - first edition, November 2026. Applies to firmware **'+VERSION+'**, based on the port 30.5 calculator and original evaluator.'),
      para('Design direction and physical-device testing: **Iain Bennett**. The cover photograph was supplied by Iain Bennett. Screens inside the book are labelled production-UI host renders where used.'),
      para('Procedures were checked against the maintained PicoCalc platform source and on-device reference. Runnable calculator/language examples have separate host verification records. A printed expected result is a check for the illustrated input, not a claim that every possible problem has a solution.'),
      para('Guide source and build instructions accompany the repository under docs/manuals. Original explanatory text and build tooling follow the repository GPL-3.0 terms; embedded fonts and supplied artwork retain their separate terms.'),
      para('Menu paths use >. Bold key names mean physical keys. A plus joins keys held together, as in Ctrl+T. RPL code uses « and » for program delimiters; Ctrl+B and Ctrl+E enter them. Mathematical answers may be printed in a simpler notation than the calculator\'s typeset display.'),
      para('Save important work before trying an example that changes variables, settings or a document. Use the saving chapter in the User Guide and the editor chapter in the Programming Guide for the distinction between a calculator session and an SD document.'),
      PageBreak(),Paragraph('Contents',ST['h1']),Rule(),toc,PageBreak()]
    blocks+=markdown(text)
    blocks+=credits(credit_letter)+legal(legal_letter)
    blocks+=[PageBreak(),Paragraph('Index',ST['h1']),Rule(),para('Alphabetical index of section subjects. The Contents lists chapter starts.','small'),index]
    doc.multiBuild(blocks,maxPasses=10)
    reader=PdfReader(doc.filename)
    missing=[]
    for i,page in enumerate(reader.pages,1):
        txt=page.extract_text() or ''
        if '\ufffd' in txt:missing.append(i)
    assert not missing,missing
    return dict(file=Path(doc.filename).name,pages=len(reader.pages),sha256=hashlib.sha256(Path(doc.filename).read_bytes()).hexdigest(),headings=doc.headings)

if __name__=='__main__':
    OUT.mkdir(parents=True,exist_ok=True)
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--book',choices=('all','user','programming','api'),default='all')
    args=parser.parse_args()
    specs=[('user','user-guide','User Guide','Learn the stack. Explore graphs. Keep your work.','#39C6DA','C','D'),
      ('programming','programming-guide','Programming Guide','RPL, Lua and CHIP-8 on the PicoCalc.','#E8B153','E','F'),
      ('api','api-assembly-reference','API & Assembly Reference','Lua APIs. CHIP-8 instructions. Reusable ROM routines.','#93C9AC','D','E')]
    reports=[build(*spec[1:]) for spec in specs if args.book in ('all',spec[0])]
    report_name='manual-build.json' if args.book=='all' else args.book+'-manual-build.json'
    (OUT/report_name).write_text(json.dumps(dict(version=VERSION,books=reports),indent=2)+'\n')
    for book in reports:print(book['file'],book['pages'],'pages')
