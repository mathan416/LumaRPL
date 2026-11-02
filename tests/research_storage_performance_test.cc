#include "bulk_storage.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>
static uint8_t memory[8*1024*1024];static bool present=true,bad_read=false,bad_write=false;static unsigned reads=0,writes=0;
extern "C" int pico_psram_available(){return present;}
extern "C" int pico_psram_read(uint32_t at,void *p,size_t n){++reads;if(!present||at>sizeof memory||n>sizeof memory-at)return 0;memcpy(p,memory+at,n);if(bad_read&&n)static_cast<uint8_t *>(p)[0]^=1;return 1;}
extern "C" int pico_psram_write(uint32_t at,const void *p,size_t n){++writes;if(bad_write||!present||at>sizeof memory||n>sizeof memory-at)return 0;memcpy(memory+at,p,n);return 1;}
struct segment{float x,y,xx,yy;uint32_t level;};
int main(){
 for(bool external:{false,true}){present=external;reads=writes=0;
  {pico_bulk_records<segment,1536> records(false);assert(records.good()&&reads==0&&writes==0);assert(records.reset_append());
   for(unsigned i=0;i<200;++i)assert(records.put(i,{float(i),2,3,4,i%3}));assert(reads==0&&writes==(external?200:0));unsigned written=writes;
   for(unsigned pass=0;pass<3;++pass)for(unsigned i=0;i<200;++i){segment value;assert(records.read(i,value)&&value.x==float(i)&&value.level==i%3);}assert(writes==written&&reads==(external?600:0));
   assert(records.reset_append());assert(records.put(0,{9,8,7,6,2}));segment value;assert(records.read(0,value)&&value.x==9);assert(!records.read(1,value)&&!records.good());
  }assert(!pico_bulk_used());
 }
 present=true;{pico_bulk_records<segment,8> records(false);bad_write=true;assert(!records.put(0,{})&&!records.good());bad_write=false;}
 {pico_bulk_records<segment,8> records(false);assert(records.put(0,{1,2,3,4,0}));bad_read=true;segment value;assert(!records.read(0,value)&&!records.good());bad_read=false;}
 {pico_bulk_records<segment,8> records;records[0]={1,2,3,4,1};records[1]={2,3,4,5,2};segment value;assert(records.read(0,value)&&value.x==1);assert(records.put(1,{7,8,9,10,1}));assert(records.read(1,value)&&value.x==7);}
 assert(!pico_bulk_used());puts("PASS: lazy contour storage: zero initialization writes; one checked write per segment; zero writes while drawing; CRC/write failure and unwritten-record rejection; legacy and no-PSRAM fallback preserved");
}
