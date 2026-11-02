#pragma once
#include "storage.h"
#include "text_editor.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cctype>
// SD-backed reference. Eight visible records and one bounded topic; no full index
// or manual is loaded into calculator memory.
namespace pico_reference {
constexpr unsigned record_bytes=460,topic_bytes=4096;
inline uint32_t u32(const unsigned char *p){return uint32_t(p[0])|(uint32_t(p[1])<<8)|(uint32_t(p[2])<<16)|(uint32_t(p[3])<<24);}
inline uint32_t crc32(const char *p,size_t n){uint32_t c=~0u;for(size_t i=0;i<n;++i){c^=uint8_t(p[i]);for(int b=0;b<8;++b)c=(c>>1)^(0xedb88320u&uint32_t(-int(c&1)));}return ~c;}
inline bool contains(const char *text,const char *query)
{for(;*text;++text){const char *a=text,*b=query;while(*a&&*b&&std::tolower(uint8_t(*a))==std::tolower(uint8_t(*b))){++a;++b;}if(!*b)return true;}return !*query;}
struct entry {uint32_t offset=0,crc=0;uint16_t length=0;uint8_t category=0;char title[64]{},keys[384]{};};
class store {
 FIL index{};bool opened=false;const char *text_path="help/commands.lrt";
 public:
 unsigned count=0;uint32_t data_size=0;
 ~store(){close();}
 void close(){if(opened)pico_f_close(&index);opened=false;count=data_size=0;}
 bool open(const char *index_path="help/commands.lri",const char *topics_path="help/commands.lrt"){close();text_path=topics_path;if(pico_f_open(&index,index_path,FA_READ)!=FR_OK)return false;opened=true;unsigned char h[16];UINT n=0;
  if(pico_f_read(&index,h,sizeof h,&n)!=FR_OK||n!=sizeof h||memcmp(h,"LRH1",4))return close(),false;
  count=u32(h+4);data_size=u32(h+8);
  if(!count||count>2048||data_size>8*1024*1024||u32(h+12)!=record_bytes||f_size(&index)!=16+count*record_bytes)return close(),false;
  return true;}
 bool read(unsigned ordinal,entry &e){if(!opened||ordinal>=count)return false;unsigned char raw[record_bytes];UINT n=0;
  if(f_lseek(&index,16+ordinal*record_bytes)!=FR_OK||pico_f_read(&index,raw,sizeof raw,&n)!=FR_OK||n!=sizeof raw)return false;
  e.offset=u32(raw);e.length=unsigned(raw[4])|(unsigned(raw[5])<<8);e.category=raw[6];e.crc=u32(raw+8);memcpy(e.title,raw+12,64);memcpy(e.keys,raw+76,384);
  return !raw[7]&&e.length&&e.length<=topic_bytes&&e.offset<=data_size&&e.length<=data_size-e.offset&&memchr(e.title,0,64)&&memchr(e.keys,0,384)&&*e.title&&pico_text_editor::valid_utf8(e.title,strlen(e.title));}
 bool topic(const entry &e,char *out,size_t capacity){if(capacity<=e.length)return false;FIL f{};if(pico_f_open(&f,text_path,FA_READ)!=FR_OK)return false;UINT n=0;
  bool ok=f_size(&f)==data_size&&f_lseek(&f,e.offset)==FR_OK&&pico_f_read(&f,out,e.length,&n)==FR_OK&&n==e.length;
  if(pico_f_close(&f)!=FR_OK)ok=false;
  if(!ok||crc32(out,e.length)!=e.crc||!pico_text_editor::valid_utf8(out,e.length))return false;out[e.length]=0;return true;}
 bool exact(const char *name,entry &e,unsigned *ordinal=nullptr){char match[386];if(snprintf(match,sizeof match,"\x1f%s\x1f",name)>=int(sizeof match))return false;
  for(unsigned i=0;i<count;++i){if(!read(i,e))return false;if(contains(e.keys,match)){if(ordinal)*ordinal=i;return true;}}return false;}
};
}
