"""Classify documented spellings using the target's real opcode declarations."""
from pathlib import Path
import json,re,shutil,subprocess,unicodedata
ROOT=Path(__file__).resolve().parents[1]
def key(text):return ' '.join(unicodedata.normalize('NFKC',text.strip('` ')).casefold().split())
def command_metadata():
 definitions=r'''
#define CONFIG_FIXED_BASED_OBJECTS 1
#define CONFIG_COLOR 1
#define PICOCALC 1
#define ID_RANGE(...)
#define ID(i) [#i,"type",#i,"","",""]
#define CMD(i) [#i,"command",#i,"","",""]
#define NAMED(i,n) [#i,"command",n,"","",""]
#define OP(i,n) NAMED(i,n)
#define ALIAS(i,n) [#i,"alias",n,"","",""]
#define MENU(i) [#i,"menu",#i,"","",""]
#define UNIT_MENU(i) ID(i)
#define CONSTANT_MENU(i) ID(i)
#define EQUATION_MENU(i) ID(i)
#define LIBRARY_MENU(i) ID(i)
#define CHARACTER_MENU(i) ID(i)
#define FLAG(a,b) [#a,"flag",#a,#b,"",""] [#b,"flag",#b,#a,"",""]
#define SETTING(n,l,h,i) [#n,"setting",#n,#l,#h,#i]
#define SETTING_BITS(n,t,b,l,h,i) SETTING(n,l,h,i)
#define SETTING_VALUE(n,a,b,v) [#n,"preset",a,#b,"",""]
#define SETTING_ENUM(n,a,b) SETTING_VALUE(n,a,b,0)
'''
 source=definitions+'\n#include "'+str(ROOT/'db48x/src/ids.tbl')+'"\n'
 raw=subprocess.check_output([shutil.which('clang++') or 'c++','-E','-P','-x','c++','-'],input=source,text=True)
 raw=re.sub(r',\s*nullptr\s*,',',null,',raw)
 decoder=json.JSONDecoder();rows=[]
 while raw.strip():
  raw=raw.lstrip();
  try:row,end=decoder.raw_decode(raw)
  except json.JSONDecodeError:raise ValueError(raw[:220])
  rows.append(row);raw=raw[end:]
 ids=list(dict.fromkeys(r[0] for r in rows if r[1]!='alias'));at={id:n for n,id in enumerate(ids)}
 def parseable(id):return at['Drop']<=at[id]<=at['ToQuotient'] or at['ToList']<=at[id]<=at['LastMenu'] or at['Termial']<=at[id]<=at['AckAll']
 commands={}
 for id,kind,spelling,group,high,initial in rows:
  if kind=='alias':continue
  commands[id]=dict(id=id,kind=kind,spellings=[id] if spelling is None else list(dict.fromkeys([spelling,id])),group=group,high=high,initial=initial,parseable=parseable(id))
 for id,kind,spelling,*_ in rows:
  if kind=='alias' and spelling is not None and spelling not in commands[id]['spellings']:commands[id]['spellings'].append(spelling)
 return commands
