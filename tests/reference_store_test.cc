#include <cassert>
#include <cstdio>
#include <map>
#include <string>
#include <vector>
#include "../pico/reference_store.h"
static std::map<FIL*,FILE*> files;
extern "C" FRESULT pico_f_open(FIL *f,const char *p,BYTE){FILE *v=fopen(p,"rb");if(!v)return FR_NO_FILE;fseek(v,0,SEEK_END);f->obj.objsize=ftell(v);rewind(v);files[f]=v;return FR_OK;}
extern "C" FRESULT pico_f_read(FIL *f,void *p,UINT n,UINT *got){auto i=files.find(f);if(i==files.end())return FR_INVALID_OBJECT;*got=fread(p,1,n,i->second);return ferror(i->second)?FR_DISK_ERR:FR_OK;}
extern "C" FRESULT pico_f_close(FIL *f){auto i=files.find(f);if(i==files.end())return FR_INVALID_OBJECT;int n=fclose(i->second);files.erase(i);return n?FR_DISK_ERR:FR_OK;}
extern "C" FRESULT f_lseek(FIL *f,FSIZE_t p){auto i=files.find(f);return i==files.end()||fseek(i->second,p,SEEK_SET)?FR_DISK_ERR:FR_OK;}
static std::vector<char> read(const char *p){FILE *f=fopen(p,"rb");assert(f);fseek(f,0,SEEK_END);std::vector<char> v(ftell(f));rewind(f);assert(fread(v.data(),1,v.size(),f)==v.size());fclose(f);return v;}
static void write(const char *p,const std::vector<char>&v){FILE *f=fopen(p,"wb");assert(f&&fwrite(v.data(),1,v.size(),f)==v.size());fclose(f);}
int main(){
 const char *idx="help/commands.lri",*txt="help/commands.lrt";auto index=read(idx),text=read(txt);
 for(unsigned cycle=0;cycle<20;++cycle){pico_reference::store s;assert(s.open()&&s.count==1042);pico_reference::entry e;char buffer[4097];for(unsigned i=0;i<s.count;++i)assert(s.read(i,e)&&s.topic(e,buffer,sizeof buffer));assert(!s.read(s.count,e));assert(s.exact("FixedDisplay",e)&&std::string(e.title)=="Fix");assert(s.exact("sIN",e)&&std::string(e.title)=="sin");assert(!s.topic(e,buffer,1));}
 assert(files.empty());
 assert(rename(idx,"help/commands.away")==0);{pico_reference::store s;assert(!s.open());}assert(rename("help/commands.away",idx)==0);
 {auto bad=index;bad[0]='X';write(idx,bad);pico_reference::store s;assert(!s.open());}write(idx,index);
 {auto bad=index;bad[4]=char(0xff);bad[5]=char(0xff);write(idx,bad);pico_reference::store s;assert(!s.open());}write(idx,index);
 {pico_reference::store s;assert(s.open());pico_reference::entry e;assert(s.read(0,e));auto bad=text;bad[e.offset]^=1;write(txt,bad);char buf[4097];assert(!s.topic(e,buf,sizeof buf));}write(txt,text);
 {auto bad=index;bad[16+4]=0;bad[16+5]=32;write(idx,bad);pico_reference::store s;assert(s.open());pico_reference::entry e;assert(!s.read(0,e));}write(idx,index);
 for(const char *book:{"lua","chip8"}){char ip[64],tp[64];snprintf(ip,sizeof ip,"help/%s.lri",book);snprintf(tp,sizeof tp,"help/%s.lrt",book);for(unsigned cycle=0;cycle<20;++cycle){pico_reference::store s;assert(s.open(ip,tp));pico_reference::entry e;char b[4097];for(unsigned i=0;i<s.count;++i)assert(s.read(i,e)&&s.topic(e,b,sizeof b));}assert(files.empty());}
 assert(files.empty());puts("PASS: 20 full reference cycles, accepted spellings, bounded reads, missing/bad metadata, CRC failures and complete file cleanup");
}
