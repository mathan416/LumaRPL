#include "bulk_storage.h"
#include "workspace.h"
#include "sample_cache.h"
#include "source_backing.h"
#include "editor_tools.h"
#include "notebook.h"
#include "form_document.h"
#include "data_table.h"
#include "path_scene.h"
#include "ode.h"
#include <cassert>
#include <initializer_list>
#include <cstdio>
static uint8_t memory[8*1024*1024];static bool present=true,fail_write=false,corrupt_read=false;
extern "C" int pico_psram_available(){return present;}
extern "C" int pico_psram_read(uint32_t at,void *p,size_t n){if(!present||at>sizeof memory||n>sizeof memory-at)return 0;memcpy(p,memory+at,n);if(corrupt_read&&n)static_cast<uint8_t *>(p)[0]^=1;return 1;}
extern "C" int pico_psram_write(uint32_t at,const void *p,size_t n){if(fail_write||!present||at>sizeof memory||n>sizeof memory-at)return 0;memcpy(memory+at,p,n);return 1;}
struct text{char value[4700]="unsaved document";};
int main()
{
    assert(!pico_bulk_allocate(0)&&!pico_bulk_allocate(65537));
    {uint32_t handles[127];for(auto &h:handles){h=pico_bulk_allocate(1);assert(h);}assert(!pico_bulk_allocate(1));for(auto h:handles)pico_bulk_free(h);}assert(!pico_bulk_used());
    {pico_document_cache<text> c;assert(c.get());strcpy(c.resident->value,"changed, still unsaved");fail_write=true;assert(!c.park()&&c.resident&&!strcmp(c.resident->value,"changed, still unsaved"));fail_write=false;assert(c.park()&&!c.resident&&c.handle);assert(c.get()&&!strcmp(c.resident->value,"changed, still unsaved"));corrupt_read=true;assert(!c.park()&&c.resident);corrupt_read=false;assert(c.park());corrupt_read=true;assert(!c.get()&&!c.resident);corrupt_read=false;assert(c.get());}
    {text *p=nullptr;pico_document_cache<text> c;{pico_document_access<text> access(c,p);assert(p);strcpy(p->value,"retained on return");}assert(!p);{pico_document_access<text> access(c,p);assert(!strcmp(p->value,"retained on return"));}}
    {present=false;pico_document_cache<text> c;assert(c.get());assert(!c.park()&&c.resident);present=true;assert(c.park());}
    {double a[900],b[900];for(int i=0;i<900;++i)a[i]=i/7.0;pico_sample_cache c;assert(c.store(a)&&c.restore(b)&&!memcmp(a,b,sizeof a));corrupt_read=true;assert(!c.restore(b));corrupt_read=false;assert(!c.restore(b));assert(c.store(a)&&c.restore(b)&&!memcmp(a,b,sizeof a));}
    {char a[16384],b[16384];memset(a,'a',sizeof a);a[16383]=0;pico_source_backing c;assert(c.store(a)&&c.restore(b)&&!memcmp(a,b,sizeof a));}
    for(bool external:{false,true}){present=external;char source[512]="",clip[257]="";pico_text_editor e{source,sizeof source,0,0};pico_editor_tools tools(e,clip);assert(tools.ready());for(int i=0;i<20;++i){char ch='A'+i;tools.break_group();assert(tools.insert(&ch,1));}for(int i=0;i<8;++i)assert(tools.undo());assert(!tools.undo()&&e.length==12);for(int i=0;i<8;++i)assert(tools.redo());assert(e.length==20);}
    present=true;assert(!pico_bulk_used());
    {pico_workspace_lease editor(pico_workspace_owner::editor,16384);assert(editor.data);pico_workspace_lease conflict(pico_workspace_owner::plot,7200);assert(!conflict.data);editor.release();pico_workspace_lease plot(pico_workspace_owner::plot,7200);assert(plot.data);assert(!editor.acquire(pico_workspace_owner::editor,16384));}assert(!pico_workspace_acquire(pico_workspace_owner::none,1));assert(!pico_workspace_acquire(pico_workspace_owner::source,16385));
    printf("PASS: PSRAM ownership, parking/recovery, failed writes, corruption, fallback, source/graph backing, undo/redo and exclusive workspace; host document payload %zu bytes\n",sizeof(pico_notebook)+sizeof(pico_form_document)+sizeof(pico_data_table)+sizeof(pico_path_scene)+sizeof(pico_ode_scene));
}
