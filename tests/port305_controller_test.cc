// Production controllers with host I/O; only device/storage boundaries are adapted.
#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <glob.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../pico/target.h"
extern "C" {
void pico_ui_host_refresh();int pico_ui_host_disk_ok();
FRESULT pico_ui_host_f_read(FIL *,void *,UINT,UINT *);FRESULT pico_ui_host_f_write(FIL *,const void *,UINT,UINT *);FRESULT pico_ui_host_f_close(FIL *);FRESULT pico_ui_host_f_lseek(FIL *,FSIZE_t);FRESULT pico_ui_host_f_sync(FIL *);FRESULT pico_ui_host_f_readdir(DIR *,FILINFO *);FRESULT pico_ui_host_f_closedir(DIR *);
}
void pico_ui_host_mark_dirty(unsigned);void pico_ui_host_mark_dirty(int,int,int,int);
#define PICO_UI_HOST 1
#define PICO_PROGRAM_NAME "LumaRPL"
#define PICO_PROGRAM_VERSION_STRING "0.10.0-port30.5"
#define lcd_refresh pico_ui_host_refresh
#define mark_dirty pico_ui_host_mark_dirty
#define sys_disk_ok pico_ui_host_disk_ok
#define f_read pico_ui_host_f_read
#define f_write pico_ui_host_f_write
#define f_close pico_ui_host_f_close
#define f_lseek pico_ui_host_f_lseek
#define f_sync pico_ui_host_f_sync
#define f_readdir pico_ui_host_f_readdir
#define f_closedir pico_ui_host_f_closedir
int pico_ui_host_read_key();
#include "../pico/desktop.cc"
#include "../pico/stack_menu.h"
#include "../pico/reference_input.h"
#include "../pico/calculator_shortcuts.h"
#include "../pico/ui/reference_font.h"
#include "../pico/ui/softmenu.h"
#include "../pico/ui/calculator_frame.h"
#include "stack.h"
#include "compare.h"
#undef lcd_refresh
#undef mark_dirty
#undef sys_disk_ok
[[noreturn]] static void test_abort(int line){fprintf(stderr,"FAIL: production controller check at line %d, screen %s\n",line,pico_ui::visible_screen);std::exit(1);}
static bool host_sd=true,host_fail_save=false;static unsigned host_write_opens=0;
static std::string host_rates_csv;
static int host_theme=0;
static std::string host_variant="normal";
static const char *host_after_key_variant=nullptr;static int host_variant_key=0;
static unsigned host_keys=0,host_frames=0,host_polls=0,host_transitions=0;
static std::string host_title;
static std::vector<int> forced_keys;static size_t forced_index=0;
static void force(std::initializer_list<int> keys){forced_keys=keys;forced_index=0;}
static std::string host_screen;
static std::map<std::string,unsigned> visits,next_item;
static bool host_dirty_rows[320];static std::map<std::string,std::map<unsigned,unsigned>> host_transfers;
static const char *const *host_items=nullptr;static int host_count=0,host_selected=0;
static std::map<FIL *,FILE *> host_files;
struct HostDirectory {std::vector<std::string> names;size_t index=0;std::string path;};
static std::map<DIR *,HostDirectory> host_directories;
static unsigned char host_psram[8*1024*1024];
static std::string path(const char *name){std::string p=name;while(!p.empty()&&p[0]=='/')p.erase(0,1);if(p.rfind("db48x/",0)==0)p.erase(0,6);return p;}
void pico_ui_host_mark_dirty(unsigned y){if(y<320)host_dirty_rows[y]=true;}
void pico_ui_host_mark_dirty(int,int y1,int,int y2){for(int y=std::max(0,y1);y<=y2&&y<320;++y)host_dirty_rows[y]=true;}
const uint32_t pico_memory_profile[16]={8392,0,0,0,0,572,4256,sizeof(pico_matrix_document),sizeof(pico_finance_document),sizeof(pico_matrix_document),sizeof(pico_complex_document),sizeof(pico_geometry_document),sizeof(pico_math_lab),sizeof(pico_advanced_math),sizeof(pico_science_document),sizeof(pico_research_document)};
extern "C" {
int pico_ui_host_disk_ok(){return host_sd;}
int file_exists(const char *name){struct stat st;return !stat(path(name).c_str(),&st);}

void pico_ui_host_refresh()
{
    ++host_frames;
    std::string id=pico_ui::visible_screen;visits[id]++;
    unsigned rows=0;for(auto &row:host_dirty_rows){rows+=row;row=false;}host_transfers[id][rows]++;
    static std::map<std::string,bool> saved;
    std::string snapshot=host_variant+"."+std::to_string(host_theme)+"."+id;
    if(!saved[snapshot])
    {
        saved[snapshot]=true;std::string filename="../../reports/port305-controllers/"+snapshot+".ppm";
        FILE *f=fopen(filename.c_str(),"wb");if(f){fprintf(f,"P6\n320 320\n255\n");for(int y=0;y<320;++y)for(int x=0;x<320;++x){auto c=::Screen.pixel_color(x,y);unsigned char rgb[]={c.red(),c.green(),c.blue()};fwrite(rgb,1,3,f);}fclose(f);}
    }
}
int pico_keyboard_poll(){return -1;}
int pico_keyboard_event(int *state){if(++host_polls>3000){*state=1;host_polls=0;return 27;}return -1;}
void pico_keyboard_reset(){host_polls=0;}
void pico_keyboard_transition(){++host_transitions;}
void pico_ui_input_begin(){}void pico_ui_cursor_complete(){}void pico_ui_performance_snapshot(pico_ui_performance *p){*p={};}
void pico_mark_dirty(unsigned y){pico_ui_host_mark_dirty(y);}
void pico_ui_host_stop_buzzer(){}
void pico_ui_host_start_buzzer(uint32_t){}
void pico_battery_snapshot(pico_battery_state *s){*s={};s->valid=true;s->percent=87;s->bios_version=0x12;s->valid_at=sys_current_ms();s->last_reply=(87<<8)|0x0b;s->write_result=1;s->read_result=2;}
int pico_psram_init(){return 1;}int pico_psram_available(){return 1;}uint32_t pico_psram_capacity(){return sizeof host_psram;}
int pico_psram_read(uint32_t at,void *out,size_t n){if(at>sizeof host_psram||n>sizeof host_psram-at)return 0;memcpy(out,host_psram+at,n);return 1;}
int pico_psram_write(uint32_t at,const void *in,size_t n){if(at>sizeof host_psram||n>sizeof host_psram-at)return 0;memcpy(host_psram+at,in,n);return 1;}
void pico_io_reset(){}int pico_io_failed(){return host_fail_save;}
void pico_storage_verified(const char *){}void pico_storage_recovered(const char *,const char *){}
FRESULT pico_f_open(FIL *f,const char *name,BYTE mode)
{
    if(mode&FA_WRITE)++host_write_opens;if(!host_sd)return FR_NOT_READY;if(host_fail_save&&(mode&FA_WRITE))return FR_DISK_ERR;
    auto p=path(name);FILE *file=fopen(p.c_str(),mode&FA_WRITE?(mode&FA_CREATE_ALWAYS?"wb+":"rb+"):"rb");if(!file)return FR_NO_FILE;
    host_files[f]=file;fseek(file,0,SEEK_END);f->obj.objsize=ftell(file);fseek(file,0,SEEK_SET);return FR_OK;
}
FRESULT pico_f_read(FIL *f,void *out,UINT n,UINT *got){auto it=host_files.find(f);if(it==host_files.end())return FR_INVALID_OBJECT;*got=fread(out,1,n,it->second);return ferror(it->second)?FR_DISK_ERR:FR_OK;}
FRESULT pico_f_write(FIL *f,const void *in,UINT n,UINT *done){auto it=host_files.find(f);if(host_fail_save||it==host_files.end())return FR_DISK_ERR;*done=fwrite(in,1,n,it->second);f->obj.objsize=std::max<FSIZE_t>(f->obj.objsize,ftell(it->second));return *done==n?FR_OK:FR_DISK_ERR;}
FRESULT pico_f_close(FIL *f){auto it=host_files.find(f);if(it==host_files.end())return FR_INVALID_OBJECT;int rc=fclose(it->second);host_files.erase(it);return rc?FR_DISK_ERR:FR_OK;}
FRESULT pico_ui_host_f_read(FIL *f,void *p,UINT n,UINT *g){return pico_f_read(f,p,n,g);}FRESULT pico_ui_host_f_write(FIL *f,const void *p,UINT n,UINT *g){return pico_f_write(f,p,n,g);}FRESULT pico_ui_host_f_close(FIL *f){return pico_f_close(f);}
FRESULT pico_ui_host_f_lseek(FIL *f,FSIZE_t n){auto it=host_files.find(f);return it!=host_files.end()&&!fseek(it->second,n,SEEK_SET)?FR_OK:FR_DISK_ERR;}
FRESULT pico_ui_host_f_sync(FIL *f){auto it=host_files.find(f);return it!=host_files.end()&&!fflush(it->second)?FR_OK:FR_DISK_ERR;}
FRESULT pico_f_stat(const char *name,FILINFO *info){struct stat st;auto p=path(name);if(stat(p.c_str(),&st))return FR_NO_FILE;if(info){memset(info,0,sizeof *info);info->fsize=st.st_size;info->fattrib=S_ISDIR(st.st_mode)?AM_DIR:0;}return FR_OK;}
FRESULT pico_f_mkdir(const char *name){auto p=path(name);return !mkdir(p.c_str(),0700)?FR_OK:FR_EXIST;}
FRESULT pico_f_unlink(const char *name){return !remove(path(name).c_str())?FR_OK:FR_NO_FILE;}
int pico_commit_state(const char *a,const char *b){return !host_fail_save&&!rename(path(a).c_str(),path(b).c_str());}
FRESULT pico_f_opendir(DIR *dir,const char *name){if(!host_sd)return FR_NOT_READY;auto p=path(name);struct stat st;if(stat(p.c_str(),&st))return FR_NO_PATH;HostDirectory d;d.path=p;glob_t g{};glob((p+"/*").c_str(),0,nullptr,&g);for(size_t i=0;i<g.gl_pathc;++i){std::string n=g.gl_pathv[i];d.names.push_back(n.substr(n.find_last_of('/')+1));}globfree(&g);host_directories[dir]=d;return FR_OK;}
FRESULT pico_ui_host_f_readdir(DIR *dir,FILINFO *info){auto it=host_directories.find(dir);if(it==host_directories.end())return FR_INVALID_OBJECT;memset(info,0,sizeof *info);auto &d=it->second;if(d.index==d.names.size())return FR_OK;auto &n=d.names[d.index++];strncpy(info->fname,n.c_str(),sizeof info->fname-1);struct stat st;if(!stat((d.path+"/"+n).c_str(),&st)){info->fsize=st.st_size;info->fattrib=S_ISDIR(st.st_mode)?AM_DIR:0;}return FR_OK;}
FRESULT pico_ui_host_f_closedir(DIR *dir){host_directories.erase(dir);return FR_OK;}
pico_network_status pico_network_get_status(){return {};}
void pico_network_time_cancel(){}
void pico_network_credentials_saved(const char *,const char *){}
static bool host_no_network_attempts=false;
void pico_network_get_diagnostics(pico_network_diagnostics *out){*out={};if(host_no_network_attempts)return;out->count=3;out->clock_pending=1;out->next_retry_ms=30000;out->driver_heap_bytes=1600;for(unsigned i=0;i<3;++i){auto &a=out->attempts[i];a.manual=i==2;a.attempt=i+1;a.stage=i==2?PICO_NET_SUCCESS:PICO_NET_FAILED;a.link_status=i==2?3:-3;a.elapsed_ms=3027;a.event_valid=1;a.event=46;a.status=4;a.reason=15;}}
int pico_network_rates_dated(const char *,const char *,int64_t,const char *,char *body,size_t capacity,pico_network_result *r,pico_network_cancel_fn,pico_network_progress_fn progress,void *context){*r={};if(progress)progress(context,"Connecting to Wi-Fi");if(!host_rates_csv.empty()){if(host_rates_csv.size()>=capacity)return 0;strcpy(body,host_rates_csv.c_str());strcpy(r->message,"Host test: downloaded");return 1;}strcpy(r->message,"Host test: network unavailable");return 0;}
}
pico_timezone pico_get_timezone(){return {};}
void pico_set_timezone(const pico_timezone &){}void pico_clock_sync(int64_t){}int64_t pico_clock_utc(){return 1793404800;}
bool pico_handle_shortcut(int key){if(pico_calculator_menu_shortcut(ui,key,pico_open_calculator_menu))return true;if(key==PICO_EXIT)return pico_prepare_exit();return true;}
static void menu_observer(const char *id,const char *const *items,int count,int selected)
{
    int visible=0;for(int i=0;i<count;++i)if(strcmp(items[i],"Back")&&strcmp(items[i],"Cancel")&&strcmp(items[i],"Back to plot"))++visible;
    bool collection=!strcmp(id,"editor.examples")||!strcmp(id,"desktop.edit_file.choose1")||!strcmp(id,"math.finance.flows")||(!strcmp(id,"reference.topics")||!strcmp(id,"reference.subjects"))||!strcmp(id,"conversion_ui.currency_pick.choose1")||!strcmp(id,"conversion_ui.conversion_pick_unit.choose1")||!strcmp(id,"conversion_ui.conversion_pick_unit.choose2")||!strcmp(id,"conversion_ui.timezone_menu.choose1")||!strcmp(id,"notebook_ui.notebook_cells.choose1");
    if(visible>8&&!collection){fprintf(stderr,"FAIL: hidden action page in %s (%d rows)\n",id,visible);test_abort(__LINE__);}
    host_items=items;host_count=count;host_selected=selected;
}
int pico_ui_host_read_key()
{
    if(++host_keys>60000){fprintf(stderr,"UI route budget exceeded at %s\n",pico_ui::visible_screen);std::exit(2);}
    std::string id=pico_ui::visible_screen;
    if(forced_index<forced_keys.size()){host_items=nullptr;int key=forced_keys[forced_index++];if(host_after_key_variant&&key==host_variant_key){host_variant=host_after_key_variant;host_after_key_variant=nullptr;}return key;}
    if(host_title=="Details"){host_items=nullptr;return 0x85;}
    if(host_items)
    {
        int count=host_count;auto items=host_items;host_items=nullptr;unsigned &next=next_item[id];
        int visible=0;for(int i=0;i<count;++i)if(strcmp(items[i],"Back")&&strcmp(items[i],"Cancel")&&strcmp(items[i],"Back to plot"))++visible;
        if(next>=unsigned(visible))return 0x85;
        unsigned choice=next++;
        if(int(choice/8)!=host_selected/8){--next;return choice/8>unsigned(host_selected/8)?0xb7:0xb4;}
        return '1'+choice%8;
    }
    if(id.find("file_browser.page")!=std::string::npos||id.find("development_files.page")!=std::string::npos){unsigned &n=next_item[id];return n++==0?0x81:n==2?0x82:0x85;}
    if(id=="dialog.prompt"||id.find("formula_entry")!=std::string::npos)return 27;
    return 0x85;
}
font_p picocalc_header_font(){return pico_ui::compact();}
static void calculator_frame(const char *id,const char *title)
{
    pico_ui::ScreenScope scope(id);pico_draw_calculator_frame(true);pico_ui::seen(title);pico_ui_host_refresh();
}

void pico_ui_host_redraw(bool force){pico_draw_calculator_frame(force);}

#include "advanced_settings_cases.inc"

// Form a pointer to the inherited protected field without casting the live UI
// to a derived type or modifying production layout/accessors.
struct help_position_probe:user_interface
{static unsigned user_interface::*position(){return &help_position_probe::help;}};

static std::vector<unsigned> editor_pixels(){std::vector<unsigned> pixels;for(int y=0;y<320;++y)for(int x=0;x<320;++x){auto c=::Screen.pixel_color(x,y);pixels.push_back((c.red()<<16)|(c.green()<<8)|c.blue());}return pixels;}
static void compact_source_cases()
{
 pico_ui::ScreenScope editor_scope("editor.source");
 for(int theme=0;theme<int(pico_themes::count);++theme){host_theme=theme;apply_theme(theme);
  for(auto language:{source_language::rpl,source_language::lua,source_language::assembly}){
   char text[2048],clip[257]={};const char *example=language==source_language::rpl?"« DUP * » 'SQUARE' STO\n12 SQUARE\n@ Unicode: π √ → « »\n":language==source_language::lua?"local function square(x)\n  return x*x\nend\nprint(square(12))\n-- comment 0123456789\n":"CLS\nLD V0, 5\nLD F, V0\nLD V1, 10\nLD V2, 8\nDRW V1, V2, 5\nLD V3, K\nEXIT\n";
   strcpy(text,example);pico_text_editor e{text,sizeof text,strlen(text),0};pico_editor_tools tools(e,clip);size_t scroll=0;e.cursor=0;
   host_variant=std::string("compact-editor-")+(language==source_language::rpl?"rpl":language==source_language::lua?"lua":"chip8");pico_ui::ScreenScope scope("editor.source");render_editor(e,"EXAMPLE",scroll,language,tools);flush_display();
   unsigned before=host_frames;if(!move_editor_cursor(e,scroll,tools,0xb4)||host_frames!=before)test_abort(__LINE__);
   if(!move_editor_cursor(e,scroll,tools,0xb7))test_abort(__LINE__);auto moved=editor_pixels();render_editor(e,"EXAMPLE",scroll,language,tools);if(moved!=editor_pixels())test_abort(__LINE__);
   if(!move_editor_cursor(e,scroll,tools,0xb4))test_abort(__LINE__);auto back=editor_pixels();render_editor(e,"EXAMPLE",scroll,language,tools);if(back!=editor_pixels())test_abort(__LINE__);
   tools.anchor=0;e.cursor=e.length;host_variant+="-selected";render_editor(e,"EXAMPLE",scroll,language,tools);flush_display();
  }
  char text[2048],clip[257]={};memset(text,'5',40);strcpy(text+40,"ABC");pico_text_editor e{text,sizeof text,strlen(text),0};pico_editor_tools tools(e,clip);e.cursor=39;size_t scroll=0;render_editor(e,"WRAP",scroll,source_language::rpl,tools);if(!move_editor_cursor(e,scroll,tools,0xb7))test_abort(__LINE__);size_t row,col;e.position(e.cursor,row,col,source_layout::columns);if(row!=1||col!=0)test_abort(__LINE__);auto moved=editor_pixels();render_editor(e,"WRAP",scroll,source_language::rpl,tools);if(moved!=editor_pixels())test_abort(__LINE__);
  strcpy(text,"");for(int i=0;i<20;++i)strcat(text,"line 0123456789\n");e.length=strlen(text);e.cursor=e.length;scroll=0;render_editor(e,"SCROLL",scroll,source_language::lua,tools);e.position(e.cursor,row,col,source_layout::columns);if(row<scroll||row>=scroll+source_layout::rows)test_abort(__LINE__);
 }
 host_theme=0;apply_theme(0);host_variant="normal";puts("PASS: compact source font, all three languages/eight themes, selection, 40-column wrap, 16-row scrolling, no-op keys and exact cursor pixel restoration");
}

static char host_reset_state[128]="state/Session-colour.48S";
extern "C" char *pico_ui_host_reset_state(){return host_reset_state;}
static void editor_assistance_cases()
{
 for(const auto &command:pico_editor_commands::entries)if(command.language==0){size_t n=strlen(command.text);if(!::command::lookup(utf8(command.text),n)){fprintf(stderr,"Unknown insertion command: %s\n",command.name);test_abort(__LINE__);}}
 for(int theme=0;theme<int(pico_themes::count);++theme){host_theme=theme;apply_theme(theme);
  for(auto language:{source_language::rpl,source_language::lua,source_language::assembly}){
   char text[1024]="original",clipboard[257]={};pico_text_editor editor{text,sizeof text,strlen(text),strlen(text),false};pico_editor_tools tools(editor,clipboard);
   if(!tools.insert("!",1))test_abort(__LINE__);host_variant=std::string("insert-command-")+std::to_string(unsigned(language));
   if(language==source_language::rpl)force({0x84,'D','u','p',13,'1',0x81});
   else if(language==source_language::lua)force({0x84,'s','c','r','e','e','n','.','t','e','x','t',13,'1',0x81});
   else force({0x84,'D','R','W',13,'1',0x81});
   editor_insert_command(language,tools);if(forced_index!=forced_keys.size()||!editor.dirty||!tools.undo()||strcmp(text,"original!")||!tools.undo()||strcmp(text,"original"))test_abort(__LINE__);
   strcpy(text,"AAZZ");editor.length=4;editor.cursor=4;tools.anchor=2;
   if(language==source_language::rpl)force({0x84,'D','u','p',13,'1',0x81});
   else if(language==source_language::lua)force({0x84,'s','c','r','e','e','n','.','t','e','x','t',13,'1',0x81});
   else force({0x84,'D','R','W',13,'1',0x81});
   editor_insert_command(language,tools);if(strncmp(text,"AA",2)||!tools.undo()||strcmp(text,"AAZZ"))test_abort(__LINE__);
   strcpy(text,"original");editor.length=editor.cursor=8;tools.anchor=pico_editor_tools::none;
   force({0x81,0x85,0x85});size_t cursor=editor.cursor;editor_insert_command(language,tools);if(strcmp(text,"original")||editor.cursor!=cursor)test_abort(__LINE__);
  }
  {char text[32]="original",clipboard[257]={};pico_text_editor editor{text,sizeof text,8,8,true};pico_editor_tools tools(editor,clipboard);
   host_variant="insert-search-cancel";force({0x83,0x84,'D','R','W',0x85,0x81,0x85,0x85});editor_insert_command(source_language::assembly,tools);
   if(strcmp(text,"original")||tools.applied||editor.cursor!=8||forced_index!=forced_keys.size())test_abort(__LINE__);}
  {char text[9]="original",clipboard[257]={};pico_text_editor editor{text,sizeof text,8,8,true};pico_editor_tools tools(editor,clipboard);
   host_variant="insert-full";force({0x84,'D','R','W',13,'1',0x81,0x85,0x85});editor_insert_command(source_language::assembly,tools);
   if(strcmp(text,"original")||tools.applied||editor.cursor!=8||forced_index!=forced_keys.size())test_abort(__LINE__);}
  {FILE *f=fopen("lua/INSERT.lua","wb");fputs("-- keep\n",f);fclose(f);pico_ui::focus("desktop.edit_file.choose1").selected=0;
   host_variant="insert-editor-route";force({' ',0x84,'3',0x84,'s','c','r','e','e','n','.','t','e','x','t',13,'1',0x81,26,26,0x81,0x85});edit_file("lua/INSERT.lua","INSERT",false,source_language::lua);
   char text[80]={};f=fopen("lua/INSERT.lua","rb");fread(text,1,sizeof text-1,f);fclose(f);if(strcmp(text,"-- keep\n")||forced_index!=forced_keys.size())test_abort(__LINE__);}
  {char text[80]="CLS\nLD V0, 999\n";pico_text_editor editor{text,sizeof text,strlen(text),0,true};host_variant="assembly-error-jump";force({0x81});if(assemble_file(text,"chip8/ERROR.asm",&editor)||editor.cursor!=4||!editor.dirty)test_abort(__LINE__);}
  {char text[80]="CLS\nRET\n";pico_text_editor editor{text,sizeof text,strlen(text),0,true};host_variant="assembly-runtime-jump";force({0x81});if(assemble_file(text,"chip8/ERROR.asm",&editor)||editor.cursor!=4)test_abort(__LINE__);}
  {char text[80]="«\n\"unfinished";pico_text_editor editor{text,sizeof text,strlen(text),0,true};object_g parsed=parse_source(text);if(parsed)test_abort(__LINE__);host_variant="rpl-syntax-jump";force({0x81});if(!source_error(text,editor)||editor.cursor<3||!editor.dirty)test_abort(__LINE__);}
  {char text[80]="«\n1 0 /\n»";pico_text_editor editor{text,sizeof text,strlen(text),0,true};object_g parsed=parse_source(text);if(!parsed||run_selected(parsed)==object::OK)test_abort(__LINE__);host_variant="rpl-runtime-find";force({0x81});if(!source_error(text,editor)||editor.cursor<3)test_abort(__LINE__);return_to_calculator=false;}
  {char text[80]="«\n1 0 / 2 /\n»";pico_text_editor editor{text,sizeof text,strlen(text),0,true};rt.reset();object_g parsed=parse_source(text);if(!parsed||run_selected(parsed)==object::OK)test_abort(__LINE__);force({0x85});if(source_error(text,editor)||editor.cursor)test_abort(__LINE__);return_to_calculator=false;}
  {FILE *f=fopen("lua/ERROR.lua","wb");if(!f)test_abort(__LINE__);fputs("print(1)\nerror(\"test\")\n",f);fclose(f);host_variant="lua-error-jump";force({0x81});if(show_lua_file("lua/ERROR.lua",true)!=2)test_abort(__LINE__);
   host_after_key_variant="lua-error-editor-restored";host_variant_key=0x81;
   force({' ',0x82,0x81,26,0x81,0x85});edit_file("lua/ERROR.lua","ERROR",false,source_language::lua);
   char data[80]={};f=fopen("lua/ERROR.lua","rb");fread(data,1,sizeof data-1,f);fclose(f);if(strcmp(data,"print(1)\nerror(\"test\")\n")||forced_index!=forced_keys.size())test_abort(__LINE__);}
 }
 host_variant="normal";host_theme=0;apply_theme(0);rt.reset();return_to_calculator=false;
 puts("PASS: command insertion/cancel/undo in all languages; real RPL syntax/runtime search, Lua runtime line+undo recovery, assembly syntax/runtime PC mapping in eight themes");
}

static void editor_example_cases()
{
 for(int theme=0;theme<int(pico_themes::count);++theme){host_theme=theme;apply_theme(theme);
  for(auto language:{source_language::rpl,source_language::lua,source_language::assembly}){
   const char *directory=language==source_language::rpl?"data":language==source_language::lua?"lua":"chip8";
   const char *extension=language==source_language::rpl?"48s":language==source_language::lua?"lua":"asm";
   const char *original=language==source_language::rpl?"« 2 3 + »":language==source_language::lua?"print(7)\n":"CLS\n";
   char path[96],example[96];snprintf(path,sizeof path,"%s/KEEP.%s",directory,extension);snprintf(example,sizeof example,"%s/EXAMPLE.%s",directory,extension);remove(example);
   FILE *f=fopen(path,"wb");if(!f)test_abort(__LINE__);fputs(original,f);fclose(f);
   pico_ui::focus("desktop.edit_file.choose1").selected=0;host_variant=std::string("editor-examples-")+directory;
   std::string opened=std::string("editor-example-opened-")+directory;host_after_key_variant=opened.c_str();host_variant_key=13;
   force({0xb7,' ',11,0xb7,0x84,'1',0x85,0x84,'2','1',0x81,'E','X','A','M','P','L','E',13,0x81,0x85,3,0x85,0x85,26,0x81,0x85});
   edit_file(path,"KEEP",false,language);
   if(!*editor_clipboard||strlen(editor_clipboard)!=1||forced_index!=forced_keys.size()||!file_exists(example))test_abort(__LINE__);
   char readback[256]={};f=fopen(path,"rb");if(!f)test_abort(__LINE__);fread(readback,1,sizeof readback-1,f);fclose(f);if(strcmp(readback,original))test_abort(__LINE__);
   f=fopen(example,"rb");memset(readback,0,sizeof readback);fread(readback,1,sizeof readback-1,f);fclose(f);
   const pico_editor_examples::Example *first=nullptr;for(const auto &e:pico_editor_examples::entries)if(e.language==unsigned(language)){first=&e;break;}
   if(!first||strcmp(readback,first->source))test_abort(__LINE__);
   if(language==source_language::rpl){
    remove("data/RUNEX.48s");rt.reset();pico_ui::focus("desktop.edit_file.choose1").selected=0;
    force({0x84,'2','1',0x81,'R','U','N','E','X',13,0x82,0x85});edit_file(path,"KEEP",false,language);
    if(forced_index!=forced_keys.size()||rt.depth()!=1||rt.top()->as_int32(0,false)!=5)test_abort(__LINE__);
   }
   pico_ui::focus("desktop.edit_file.choose1").selected=0;
   force({0x84,'2','1',0x81,'E','X','A','M','P','L','E',13,0x85,0x85});edit_file(path,"KEEP",false,language);
   if(forced_index!=forced_keys.size())test_abort(__LINE__); // Existing file rejected.
   host_sd=false;pico_ui::focus("desktop.edit_file.choose1").selected=0;
   force({0x84,'2','1',0x81,0x85,0x85,0x83});edit_file(path,"KEEP",true,language,original);
   host_sd=true;if(forced_index!=forced_keys.size())test_abort(__LINE__);
   host_fail_save=true;remove("lua/FAIL.lua");remove("data/FAIL.48s");remove("chip8/FAIL.asm");pico_ui::focus("desktop.edit_file.choose1").selected=0;
   force({0x84,'2','1',0x81,'F','A','I','L',13,0x81,0x85,0x85,0x85,0x85,0x83,0x85});edit_file(path,"KEEP",false,language);
   host_fail_save=false;if(forced_index!=forced_keys.size()||file_exists("lua/FAIL.lua")||file_exists("data/FAIL.48s")||file_exists("chip8/FAIL.asm"))test_abort(__LINE__);
   // Preview cancellation never replaces source or creates a file.
   pico_ui::focus("desktop.edit_file.choose1").selected=0;
   force({0x84,'2','1',0x85,0x85});edit_file(path,"KEEP",false,language);if(forced_index!=5)test_abort(__LINE__);
  }
 }
 host_theme=0;apply_theme(0);host_variant="normal";
 pico_ui::focus("desktop.edit_file.choose1").selected=0;
 puts("PASS: RPL/Lua/CHIP-8 editor references and separate example save/open/cancel; original dirty source and undo preserved in eight themes");
}

static void clean_header_cases()
{
 char original[128];snprintf(original,sizeof original,"%s",host_reset_state);
 for(const char *path:{"state/Session-colour.48S","state/Session.48S"}){snprintf(host_reset_state,sizeof host_reset_state,"%s",path);if(*state_name())test_abort(__LINE__);}
 strcpy(host_reset_state,"state/Named.48S");if(strcmp(state_name(),"Named"))test_abort(__LINE__);
 strcpy(host_reset_state,"state/Session-colour.48S");
 for(int theme=0;theme<int(pico_themes::count);++theme){host_theme=theme;apply_theme(theme);host_variant="header-no-session";calculator_frame("calculator.stack","Calculator");}
 strcpy(host_reset_state,original);host_theme=0;apply_theme(0);host_variant="normal";
 puts("PASS: default session label removed, named sessions retained and header rendered in eight themes");
}

static void language_reference_cases()
{
 for(int theme=0;theme<int(pico_themes::count);++theme){host_theme=theme;apply_theme(theme);rt.reset();ui.clear_editor();ui.clear_menu();rt.push(integer::make(321));
  for(const auto *book:{&lua_book,&chip8_book}){
   pico_reference::store store;if(!store.open(book->index,book->text))test_abort(__LINE__);
   pico_reference::entry first;if(!store.read(0,first)||strcmp(first.title,book==&lua_book?"Run your first script":"Run your first CHIP-8 program"))test_abort(__LINE__);
   for(unsigned i=0;i<store.count;++i){pico_reference::entry e;if(!store.read(i,e))test_abort(__LINE__);host_variant=std::string(book==&lua_book?"lua-reference-":"chip8-reference-")+std::to_string(i);force({0xb7,0xb4,0x85});command_reference_topic(store,e);if(forced_index!=3||rt.error()||rt.depth()!=1||rt.top()->as_int32(0,false)!=321)test_abort(__LINE__);}
   host_variant=book==&lua_book?"lua-reference-navigation":"chip8-reference-navigation";force({'2',0xb7,'1',0x85,0x85,0x85});native_command_reference(nullptr,false,*book);if(forced_index!=6)test_abort(__LINE__);
  }
  host_variant="language-root-first";pico_ui::focus(lua_book.title).selected=0;pico_ui::focus(chip8_book.title).selected=0;force({0x85});native_command_reference(nullptr,false,lua_book);if(host_count!=8||strcmp(lua_book.categories[0].name,"Start here")||host_selected!=0)test_abort(__LINE__);
  pico_ui::focus(lua_book.title).selected=6;force({0x85});native_command_reference(nullptr,false,chip8_book);if(host_selected!=0||strcmp(lua_book.categories[0].name,"Start here"))test_abort(__LINE__);
  force({0x85});native_help();if(host_count!=7||host_count!=7)test_abort(__LINE__);
  host_variant="language-help-menu";force({'4',0x85,'5',0x85,0x85});native_help();if(forced_index!=5)test_abort(__LINE__);
  host_sd=false;host_variant="lua-reference-offline";force({0x85});native_command_reference(nullptr,false,lua_book);host_sd=true;
 }
 host_theme=0;apply_theme(0);host_variant="normal";
 puts("PASS: separate Lua/CHIP-8 Help routes, every language page and standard paging/Back controls, missing SD and stack preservation in eight themes");
}

static void command_reference_cases()
{
 pico_reference::store store;if(!store.open()||store.count!=1043)test_abort(__LINE__);
 for(auto sample:{std::pair<const char*,const char*>{"Fix","Fix"},{"FixedDisplay","Fix"},{"Grad","Grad"},{"Grads","Grad"},{"Prec","Precision"}}){pico_reference::entry e;if(!store.exact(sample.first,e)||strcmp(e.title,sample.second))test_abort(__LINE__);}
 char body[4097];pico_reference::entry e;for(unsigned i=0;i<store.count;++i)if(!store.read(i,e)||!store.topic(e,body,sizeof body))test_abort(__LINE__);
 for(int theme=0;theme<int(pico_themes::count);++theme)
 {
  host_theme=theme;apply_theme(theme);rt.reset();load_saved_keymap();Settings=settings();ui.clear_help();ui.clear_menu();ui.clear_editor();rt.push(integer::make(123));ui.insert(utf8("2+3"),user_interface::DIRECT);
  std::string entry(reinterpret_cast<const char*>(rt.editor()),rt.editing());auto settings=Settings;
  for(const char *topic:{"UIAlpha","EditorReplace","StackFont","BeepOn","Root","sin","Fix","Convert"}){host_variant=std::string("native-command-")+topic;force({0xb6,0xb7,0xb4,0xb5,0x85});ui.load_help(utf8(topic));if(forced_index!=5||ui.showing_help()||rt.error()||rt.depth()!=1||rt.top()->as_int32(0,false)!=123||std::string(reinterpret_cast<const char*>(rt.editor()),rt.editing())!=entry||Settings.Precision()!=settings.Precision())test_abort(__LINE__);}
  host_variant="native-command-browse";force({'2',0xb7,0x82,'1',0x85,0x85,0x85});native_command_reference();if(forced_index!=7)test_abort(__LINE__);
  host_variant="native-command-find";force({'R','o','o','t',13,'1',0x85,0x85});reference_find();if(forced_index!=8)test_abort(__LINE__);
  host_variant="native-command-missing";force({'N','O','S','U','C','H',13,0x85});reference_find();if(forced_index!=8)test_abort(__LINE__);
  host_sd=false;host_variant="native-command-offline";force({0x85});native_command_reference();host_sd=true;
  ui.clear_editor();ui.clear_menu();
  rt.push(text::make("sin"));unsigned depth=rt.depth();host_variant="native-command-rpl-help";force({0x85});if(object::static_object(object::ID_Help)->evaluate()!=object::OK||rt.depth()!=depth) test_abort(__LINE__);rt.drop();
  ui.menu(menu_p(object::static_object(object::ID_DisplayModesMenu)));unsigned digits=Settings.DisplayDigits();host_variant="native-command-held-key";ui.key(KEY_F2,false,false);force({0x85});ui.key(KEY_F2,true,false);ui.key(0,false,false);
  if(rt.error()||rt.depth()!=1||rt.top()->as_int32(0,false)!=123||Settings.DisplayDigits()!=digits)test_abort(__LINE__);
  ui.clear_menu();rt.reset();
 }
 host_theme=0;apply_theme(0);host_variant="normal";
 puts("PASS: all 1043 native reference records/CRC/UTF8, real engine Help aliases, standard arrows/page/Back controls, find/no-match/missing SD and stack/entry preservation in eight themes");
}

static unsigned softkey_count=0,softkey_total=0;
static void inspect_softkey(utf8 label,size_t length,font_p face,int left,int right,int inkLeft,int inkRight,bool scrolling)
{
    ++softkey_count;
    if(!scrolling&&(inkLeft<left||inkRight>right))test_abort(__LINE__);
    if(std::string(reinterpret_cast<const char *>(label),length)=="%Total")
    {
        if(scrolling||face!=pico_ui::compact())test_abort(__LINE__);
        ++softkey_total;
    }
}
static void compact_softkeys_cases()
{
    pico_ui::softmenu_observer=inspect_softkey;
    for(int theme=0;theme<int(pico_themes::count);++theme)for(bool square:{false,true})
    {
        host_theme=theme;Settings=settings();apply_theme(theme);Settings.SquareMenus(square);
        Settings.MenuAppearance(object::ID_ThreeRowsMenus);
        rt.reset();load_saved_keymap();ui.clear_menu();ui.clear_shift();
        ui.insert(utf8("%T"),user_interface::DIRECT);object::static_object(object::ID_Catalog)->evaluate();
        host_variant=std::string("softkeys-catalog")+(square?"-square":"-round");
        calculator_frame("calculator.softmenus","Compact command catalog");
        ui.clear_menu();if(rt.editing())ui.key(KEY_EXIT,false,false);ui.key(0,false,false);
        for(auto id:{object::ID_ModesMenu,object::ID_DisplayModesMenu,object::ID_UserInterfaceModesMenu,object::ID_SeparatorModesMenu,object::ID_MathModesMenu,object::ID_StackMenu})
        {
            ui.clear_menu();ui.clear_shift();object::static_object(id)->evaluate();
            unsigned pages=ui.pages();
            for(unsigned page=0;page<pages;++page)
            {
                ui.page(page);
                host_variant=std::string("softkeys-")+reinterpret_cast<const char *>(object::name(id))+"-"+std::to_string(page)+(square?"-square":"-round");
                calculator_frame("calculator.softmenus","Compact calculator soft keys");
            }
        }
        ui.clear_menu();ui.clear_shift();object::static_object(object::ID_Catalog)->evaluate();
        for(unsigned page=0,count=ui.pages();page<count;++page){ui.page(page);pico_draw_calculator_frame(true);}
    }
    pico_ui::softmenu_observer=nullptr;
    if(softkey_total<16||softkey_count<10000)test_abort(__LINE__);
    printf("PASS: %u production soft-key placements; %%Total stays complete and stationary in all themes and button styles\n",softkey_count);
    rt.reset();load_saved_keymap();Settings=settings();host_theme=0;apply_theme(0);host_variant="normal";ui.clear_menu();ui.clear_shift();
}

static void calculator_shortcuts_cases()
{
    if(pico_translate_key(1)!=PICO_CATALOG||pico_translate_key(18)!=PICO_RPL_SETTINGS){fprintf(stderr,"FAIL shortcut test line %d graphics=%d help=%d menu=%u depth=%u editor=%u\n",__LINE__,ui.showing_graphics(),ui.showing_help(),ui.menu()?unsigned(ui.menu()->type()):0,unsigned(rt.depth()),unsigned(rt.editing()));std::exit(1);}
    for(int theme=0;theme<int(pico_themes::count);++theme)
    {
        host_theme=theme;rt.reset();load_saved_keymap();Settings=settings();apply_theme(theme);ui.clear_help();ui.clear_menu();ui.clear_shift();
        rt.push(integer::make(123));ui.insert(utf8("2+3"),user_interface::DIRECT);
        std::string entry(reinterpret_cast<const char *>(rt.editor()),rt.editing());
        pico_handle_shortcut(pico_translate_key(1));
        if(!ui.menu()||ui.menu()->type()!=object::ID_Catalog||rt.depth()!=1||rt.top()->as_int32(0,false)!=123||std::string(reinterpret_cast<const char *>(rt.editor()),rt.editing())!=entry){fprintf(stderr,"FAIL shortcut test line %d graphics=%d help=%d menu=%u depth=%u editor=%u\n",__LINE__,ui.showing_graphics(),ui.showing_help(),ui.menu()?unsigned(ui.menu()->type()):0,unsigned(rt.depth()),unsigned(rt.editing()));std::exit(1);}
        host_variant="shortcut-catalog";calculator_frame("calculator.softmenus","Ctrl+A Command Catalogue");
        pico_handle_shortcut(pico_translate_key(18));
        if(!ui.menu()||ui.menu()->type()!=object::ID_ModesMenu||rt.depth()!=1||std::string(reinterpret_cast<const char *>(rt.editor()),rt.editing())!=entry){fprintf(stderr,"FAIL shortcut test line %d graphics=%d help=%d menu=%u depth=%u editor=%u\n",__LINE__,ui.showing_graphics(),ui.showing_help(),ui.menu()?unsigned(ui.menu()->type()):0,unsigned(rt.depth()),unsigned(rt.editing()));std::exit(1);}
        host_variant="shortcut-settings-entry";calculator_frame("calculator.softmenus","Ctrl+R preserves calculator entry");
        ui.clear_menu();ui.key(KEY_EXIT,false,false);ui.key(0,false,false);
        pico_handle_shortcut(pico_translate_key(18));
        auto press=[](int key){pico_stack_menu_key(ui,Stack,key,false,false);pico_stack_menu_key(ui,Stack,0,false,false);};
        press(28);bool beep=Settings.BeepOn();press(40);press(40);
        if(Settings.BeepOn()!=beep||ui.shift_plane()!=1||rt.depth()!=1){fprintf(stderr,"FAIL shortcut test line %d graphics=%d help=%d menu=%u depth=%u editor=%u\n",__LINE__,ui.showing_graphics(),ui.showing_help(),ui.menu()?unsigned(ui.menu()->type()):0,unsigned(rt.depth()),unsigned(rt.editing()));std::exit(1);}
        host_variant="shortcut-settings-row";calculator_frame("calculator.softmenus","Ctrl+R Tab and Beep");
        press(43);if(!ui.menu()||ui.menu()->type()!=object::ID_SeparatorModesMenu){fprintf(stderr,"FAIL shortcut test line %d graphics=%d help=%d menu=%u depth=%u editor=%u\n",__LINE__,ui.showing_graphics(),ui.showing_help(),ui.menu()?unsigned(ui.menu()->type()):0,unsigned(rt.depth()),unsigned(rt.editing()));std::exit(1);}
        host_variant="shortcut-settings-sixth";calculator_frame("calculator.softmenus","Ctrl+F5 sixth entry");
        force({1,18,0x85});ui.load_help(utf8("Overview"));
        if(ui.showing_help()||!ui.menu()||ui.menu()->type()!=object::ID_SeparatorModesMenu||forced_index!=3)test_abort(__LINE__);

    }
    rt.reset();load_saved_keymap();Settings=settings();host_theme=0;apply_theme(0);ui.clear_menu();ui.clear_shift();host_variant="normal";
    {FILE *f=fopen("data/SHORTCUTS.lua","wb");fputs("print(12)",f);fclose(f);}
    force({1,'x',0x81,0x85});edit_file("data/SHORTCUTS.lua","SHORTCUTS.lua",false);
    {FILE *f=fopen("data/SHORTCUTS.lua","rb");char text[40]={};fread(text,1,sizeof text-1,f);fclose(f);if(strcmp(text,"x")){fprintf(stderr,"FAIL shortcut test line %d graphics=%d help=%d menu=%u depth=%u editor=%u\n",__LINE__,ui.showing_graphics(),ui.showing_help(),ui.menu()?unsigned(ui.menu()->type()):0,unsigned(rt.depth()),unsigned(rt.editing()));std::exit(1);}}
    {FILE *f=fopen("data/SHORTCUTS.lua","wb");fputs("hello hello",f);fclose(f);}
    force({18,'h','e','l','l','o',13,'b','y','e',13,'2',0x81,0x85});edit_file("data/SHORTCUTS.lua","SHORTCUTS.lua",false);
    {FILE *f=fopen("data/SHORTCUTS.lua","rb");char text[40]={};fread(text,1,sizeof text-1,f);fclose(f);if(strcmp(text,"bye bye")){fprintf(stderr,"FAIL shortcut test line %d graphics=%d help=%d menu=%u depth=%u editor=%u\n",__LINE__,ui.showing_graphics(),ui.showing_help(),ui.menu()?unsigned(ui.menu()->type()):0,unsigned(rt.depth()),unsigned(rt.editing()));std::exit(1);}}
    puts("PASS: real Ctrl+A/Ctrl+R dispatcher preserves stack and entry, Tab/Beep/sixth entry and help; source editor keeps Select all and Replace");
}

static void menu_back_cases()
{
    auto press=[](int key,bool repeating=false){pico_stack_menu_key(ui,Stack,key,repeating,false);pico_stack_menu_key(ui,Stack,0,false,false);};
    auto check=[](bool ok,int line){if(!ok){fprintf(stderr,"FAIL menu Back line %d menu=%u page=%u editing=%u error=%s\n",line,ui.menu()?unsigned(ui.menu()->type()):0,ui.page(),unsigned(rt.editing()),rt.error()?cstring(rt.error()):"none");std::exit(1);}};
    for(int theme=0;theme<int(pico_themes::count);++theme)
    {
        rt.reset();load_saved_keymap();Settings=settings();host_theme=theme;apply_theme(theme);ui.clear_help();ui.show_graphics(false);Stack.interactive=false;rt.push(integer::make(123));
        pico_open_calculator_menu(true);press(28);press(39);
        check(ui.menu()&&ui.menu()->type()==object::ID_AnglesMenu,__LINE__);
        host_variant="back-angles";calculator_frame("calculator.softmenus","Angles submenu");
        press(KEY_EXIT,true);check(ui.menu()&&ui.menu()->type()==object::ID_AnglesMenu,__LINE__);
        press(KEY_EXIT);check(ui.menu()&&ui.menu()->type()==object::ID_ModesMenu,__LINE__);
        host_variant="back-parent";calculator_frame("calculator.softmenus","Esc returns to Advanced RPL settings");
        press(KEY_EXIT);check(!ui.menu()&&rt.depth()==1&&rt.top()->as_int32(0,false)==123,__LINE__);
        host_variant="back-root-clear";calculator_frame("calculator.stack","Esc clears top menu");
        for(bool clear:{false,true})
        {
            Settings.ExitClearsMenu(clear);pico_open_calculator_menu(true);press(KEY_EXIT);check(!ui.menu(),__LINE__);
        }
        pico_open_calculator_menu(true);object::static_object(object::ID_UserInterfaceModesMenu)->evaluate();
        check(ui.pages()>1,__LINE__);ui.page(1);object::static_object(object::ID_AnglesMenu)->evaluate();press(KEY_EXIT);
        check(ui.menu()&&ui.menu()->type()==object::ID_UserInterfaceModesMenu&&ui.page()==1,__LINE__);
        host_variant="back-page-restored";calculator_frame("calculator.softmenus","Parent page restored");
        press(KEY_EXIT);check(ui.menu()&&ui.menu()->type()==object::ID_ModesMenu,__LINE__);press(KEY_EXIT);check(!ui.menu(),__LINE__);
        pico_open_calculator_menu(true);object::static_object(object::ID_AnglesMenu)->evaluate();pico_open_calculator_menu(false);press(KEY_EXIT);
        check(!ui.menu(),__LINE__);pico_open_calculator_menu(true);press(KEY_EXIT);check(!ui.menu(),__LINE__);
        pico_open_calculator_menu(true);object::static_object(object::ID_AnglesMenu)->evaluate();
        for(bool automatic:{false,true})
        {
            Settings.NoNeedToClearErrors(automatic);rt.missing_argument_error();press(KEY_EXIT);
            check(!rt.error()&&ui.menu()&&ui.menu()->type()==object::ID_AnglesMenu,__LINE__);
        }
        ui.insert(utf8("2+3"),user_interface::DIRECT);
        std::string entry(reinterpret_cast<const char *>(rt.editor()),rt.editing());
        press(KEY_EXIT);check(ui.menu()&&ui.menu()->type()==object::ID_ModesMenu&&std::string(reinterpret_cast<const char *>(rt.editor()),rt.editing())==entry,__LINE__);
        host_variant="back-entry-preserved";calculator_frame("calculator.softmenus","Back preserves unfinished input");
        press(KEY_EXIT);check(!ui.menu()&&std::string(reinterpret_cast<const char *>(rt.editor()),rt.editing())==entry,__LINE__);
        press(KEY_EXIT);check(!rt.editing()&&rt.depth()==1,__LINE__);
        pico_open_calculator_menu(true);object::static_object(object::ID_AnglesMenu)->evaluate();
        force({0x85});ui.load_help(utf8("Overview"));check(!ui.showing_help()&&ui.menu()&&ui.menu()->type()==object::ID_AnglesMenu,__LINE__);
        press(KEY_EXIT);check(ui.menu()&&ui.menu()->type()==object::ID_ModesMenu,__LINE__);press(KEY_EXIT);check(!ui.menu(),__LINE__);
        pico_open_calculator_menu(true);
        for(unsigned n=0;n<20;++n)object::static_object(n%2?object::ID_AnglesMenu:object::ID_DisplayModesMenu)->evaluate();
        unsigned backs=0;while(ui.menu()&&backs<9){press(KEY_EXIT);++backs;}
        check(!ui.menu()&&backs<=8&&rt.depth()==1&&rt.top()->as_int32(0,false)==123,__LINE__);
    }
    ui.menu_root();rt.reset();load_saved_keymap();Settings=settings();host_theme=0;apply_theme(0);host_variant="normal";
    puts("PASS: real Esc Angles-to-Modes-to-clear, parent page restoration, fresh shortcut roots, error/edit/Help priority, repeat suppression and bounded history exhaustion in eight themes");
}

#define port21_check port302_check
#include "port21_feature_cases.inc"
#define port201_check port302_check
#include "matrix_arithmetic_cases.inc"
#undef port201_check
#include "port21_new_cases.inc"
#undef port21_check

#include "math_lab_cases.inc"
#include "advanced_math_cases.inc"
#include "science_cases.inc"
#include "research_cases.inc"

#include "legacy_evaluator_cases.inc"
static void bases_cases(){
 auto saved=Settings.Base();
 for(const char *literal:{"#FFD2h","#177722o","#65490d","#1111111111010010b","#123456789ABCDEF0123456789ABCDEF0h","#11001100110011001100110011001100110011001100110011001100110011001b","#12345670123456701234567012345670o","#123456789012345678901234567890d"}){
  rt.reset();if(!execute(literal)||!execute("BinaryToReal"))test_abort(__LINE__);char expected[400]{};rt.top()->render(expected,sizeof expected);
  for(const char *command:{"Bin","Oct","Dec","Hex","Hex"}){rt.reset();if(!execute("123")||!execute(literal)||!execute(command)||rt.depth()!=2||!execute("BinaryToReal") )test_abort(__LINE__);char actual[400]{};rt.top()->render(actual,sizeof actual);if(strcmp(actual,expected)){fprintf(stderr,"Base %s %s: %s expected %s\n",literal,command,actual,expected);test_abort(__LINE__);}rt.drop();if(rt.top()->as_int32(0,false)!=123)test_abort(__LINE__);}
 }
 rt.reset();if(!execute("Bin")||rt.depth()||Settings.Base()!=2||!execute("42 Oct")||rt.top()->type()!=object::ID_based_integer||rt.top()->as_int32(0,false)!=42)test_abort(__LINE__);
 for(const char *literal:{"0","42","65490","123456789012345678901234567890","-42"}){
  for(const char *command:{"Bin","Oct","Dec","Hex"}){
   rt.reset();if(!execute(literal)||!execute("RealToBinary")||!execute(command)||!execute("BinaryToReal"))test_abort(__LINE__);char expected[400]{};rt.top()->render(expected,sizeof expected);
   rt.reset();if(!execute("123")||!execute(literal)||!execute(command)||rt.depth()!=2||!execute("BinaryToReal"))test_abort(__LINE__);char actual[400]{};rt.top()->render(actual,sizeof actual);if(strcmp(actual,expected)){fprintf(stderr,"Decimal %s %s: actual %s expected %s\n",literal,command,actual,expected);test_abort(__LINE__);}rt.drop();if(rt.top()->as_int32(0,false)!=123)test_abort(__LINE__);
  }
 }
 rt.reset();if(!execute("1.5"))test_abort(__LINE__);object_g fraction=rt.top();if(!execute("Hex")||rt.top()!=+fraction)test_abort(__LINE__);
 for(int theme=0;theme<8;++theme){host_theme=theme;apply_theme(theme);host_variant="bases-conversion";rt.reset();load_saved_keymap();ui.clear_menu();ui.clear_shift();if(!execute("65490"))test_abort(__LINE__);
 if(pico_translate_key(25)!=PICO_BASES||!pico_calculator_menu_shortcut(ui,PICO_BASES,pico_open_calculator_menu)||!ui.menu()||ui.menu()->type()!=object::ID_BasesMenu)test_abort(__LINE__);
 pico_draw_calculator_frame(true);
 // Base row is Tab-selected; F2/F3/F4/F5 are Bin/Oct/Dec/Hex.
 pico_stack_menu_key(ui,Stack,28,false,false);pico_stack_menu_key(ui,Stack,0,false,false);
 for(auto item:{std::pair<int,unsigned>{39,2},{40,8},{41,10},{42,16}}){host_variant="bases-"+std::to_string(item.second);pico_stack_menu_key(ui,Stack,item.first,false,false);pico_stack_menu_key(ui,Stack,0,false,false);if(Settings.Base()!=item.second||ui.shift_plane()!=1||rt.top()->type()!=object::ID_based_integer)test_abort(__LINE__);pico_draw_calculator_frame(true);}
 if(!execute("BinaryToReal")||rt.top()->as_int32(0,false)!=65490)test_abort(__LINE__);
 host_variant="bases-command-help";force({0x85});native_command_reference("Bin");
 }
 Settings.Base(saved);rt.reset();return_to_calculator=false;host_variant="normal";host_theme=0;apply_theme(0);ui.clear_menu();ui.clear_shift();puts("PASS: fixed-base integers/bignums convert exactly through typed and actual soft-key Bin/Oct/Dec/Hex; Ctrl+Y Bases, sticky row and stack preservation in eight themes");
}
static void cursor_error_recovery_cases()
{
    auto text=[](const char *s){while(*s){pico_calculator_text_key(ui,Stack,unsigned(*s++),false);pico_stack_menu_key(ui,Stack,0,false,false);}};
    auto enter=[](){pico_calculator_key(ui,Stack,13,false,false);pico_stack_menu_key(ui,Stack,0,false,false);};
    auto editor=[](){return std::string(reinterpret_cast<const char *>(rt.editor()),rt.editing());};
    auto reset=[](){rt.clear_error();ui.clear_editor();while(rt.depth())rt.drop();};
    for(int theme=0;theme<int(pico_themes::count);++theme)
    {
        reset();Settings=settings();ui.clear_menu();ui.clear_help();ui.clear_shift();host_theme=theme;apply_theme(theme);
        for(char op:std::string("!?^+-*/"))for(unsigned cursor:{0U,1U})
        {
            reset();text("99");enter();text("12");ui.cursor_position(cursor);
            pico_calculator_text_key(ui,Stack,op,false);pico_stack_menu_key(ui,Stack,0,false,false);
            std::string expected="12";expected.insert(cursor,1,op);
            if(rt.error()||rt.depth()!=1||rt.top()->as_int32(0,false)!=99||editor()!=expected||ui.cursor_position()!=cursor+1)test_abort(__LINE__);
        }
        reset();text("99");enter();text("((2+3)");enter();
        if(!rt.error()||editor()!="((2+3)"||ui.cursor_position()!=rt.editing()||rt.depth()!=1||rt.top()->as_int32(0,false)!=99)test_abort(__LINE__);
        host_variant="recover-unclosed-group";calculator_frame("calculator.stack","Missing parenthesis retains original formula");
        // Enter dismisses an existing error without wrapping the formula again.
        enter();if(rt.error()||editor()!="((2+3)")test_abort(__LINE__);
        enter();if(!rt.error()||editor()!="((2+3)")test_abort(__LINE__);
        pico_calculator_key(ui,Stack,33,false,false);pico_stack_menu_key(ui,Stack,0,false,false);
        if(rt.error()||editor()!="((2+3)")test_abort(__LINE__);
        text(")");enter();
        if(rt.error()||rt.editing()||rt.depth()!=2||rt.top()->as_int32(0,false)!=5||rt.stack(1)->as_int32(0,false)!=99)test_abort(__LINE__);
        reset();text("(1/0)");enter();
        if(!rt.error()||editor()!="(1/0)"||ui.cursor_position()!=3||rt.source()!=rt.editor()+3)test_abort(__LINE__);
        host_variant="recover-divide-error";calculator_frame("calculator.stack","Divide-by-zero cursor points into original formula");
        pico_calculator_key(ui,Stack,33,false,false);pico_stack_menu_key(ui,Stack,0,false,false);
        if(rt.error()||editor()!="(1/0)")test_abort(__LINE__);
        ui.remove(3,1);ui.cursor_position(3);text("2");enter();
        if(rt.error()||rt.editing()||rt.depth()!=1||!execute("2 *")||!rt.top()->is_one(false))test_abort(__LINE__);
        reset();if(!execute("2 'ERRX' STO"))test_abort(__LINE__);
        text("(1/(ERRX-ERRX))");enter();
        if(!rt.error()||editor()!="(1/(ERRX-ERRX))"||ui.cursor_position()>rt.editing())test_abort(__LINE__);
        reset();if(!execute("'ERRX' PURGE"))test_abort(__LINE__);
        for(const char *formula:{" (2+*3) ","((2^3+4)-)","(1+2))"})
        {
            reset();text(formula);enter();
            if(!rt.error()||editor()!=formula||ui.cursor_position()>rt.editing()||rt.source()<rt.editor()||rt.source()>rt.editor()+rt.editing())test_abort(__LINE__);
        }
    }
    reset();ui.clear_shift();ui.clear_menu();host_theme=0;apply_theme(0);host_variant="normal";
    puts("PASS: middle/start cursor operators only edit; original formula, mapped error positions, repeated errors and successful repair in 8 themes");
}

static void grouped_entry_cursor_cases()
{
    auto text=[](const char *s){while(*s){pico_calculator_text_key(ui,Stack,unsigned(*s++),false);pico_stack_menu_key(ui,Stack,0,false,false);}};
    auto enter=[](){pico_calculator_key(ui,Stack,13,false,false);pico_stack_menu_key(ui,Stack,0,false,false);};
    for(int theme=0;theme<int(pico_themes::count);++theme)
    {
        while(rt.depth())rt.drop();rt.clear_error();Settings=settings();ui.clear_menu();ui.clear_help();ui.clear_shift();host_theme=theme;apply_theme(theme);
        struct Case{const char *source;int value;};
        for(auto c:{Case{"((2^3+4)-2)",10},Case{"(2+3)!",120},Case{"(2+4)?",21},Case{"(2+1)^3",27},Case{"(6)",6},Case{"(2*(3+4))",14},Case{"(2^3)+4-2",10}})
        {
            while(rt.depth())rt.drop();text(c.source);
            if(rt.error()||rt.depth()||!rt.editing())test_abort(__LINE__);
            enter();if(rt.error()||rt.editing()||rt.depth()!=1||rt.top()->as_int32(-99,false)!=c.value){fprintf(stderr,"Grouped entry %s error %s\n",c.source,rt.error()?cstring(rt.error()):"none");test_abort(__LINE__);}
        }
        host_variant="grouped-result";calculator_frame("calculator.stack","Grouped formula evaluates to 10");
        for(const char *source:{"(1;3)","(1 3)"})
        {
            while(rt.depth())rt.drop();text(source);enter();
            if(rt.error()||rt.depth()!=1||!rt.top()->is_complex())test_abort(__LINE__);
        }
        while(rt.depth())rt.drop();text("((2^3+4)-2)");
        pico_draw_calculator_frame(true);pico_ui_host_refresh();
        std::string formula(reinterpret_cast<const char *>(rt.editor()),rt.editing());
        for(int key:{PICO_LEFT,PICO_LEFT,PICO_RIGHT,PICO_RIGHT})
        {
            unsigned before=ui.cursor_position();
            if(pico_calculator_shortcut_full_redraw(key)||!pico_calculator_cursor_shortcut(ui,key))test_abort(__LINE__);
            if(ui.cursor_position()!=(key==PICO_LEFT?before-1:before+1))test_abort(__LINE__);
            std::fill(std::begin(host_dirty_rows),std::end(host_dirty_rows),false);
            pico_draw_calculator_frame(pico_calculator_shortcut_full_redraw(key));
            unsigned rows=0;for(bool row:host_dirty_rows)rows+=row;
            if(!rows||rows==320){fprintf(stderr,"Cursor dirty rows: %u\n",rows);test_abort(__LINE__);}
            std::vector<unsigned> pixels;
            for(int y=40;y<320;++y)for(int x=0;x<320;++x){auto p=Screen.pixel_color(x,y);pixels.push_back((p.red()<<16)|(p.green()<<8)|p.blue());}
            pico_draw_calculator_frame(true);
            size_t at=0;for(int y=40;y<320;++y)for(int x=0;x<320;++x){auto p=Screen.pixel_color(x,y);if(pixels[at++]!=unsigned((p.red()<<16)|(p.green()<<8)|p.blue()))test_abort(__LINE__);}
            if(std::string(reinterpret_cast<const char *>(rt.editor()),rt.editing())!=formula)test_abort(__LINE__);
        }
        host_variant="grouped-cursor";calculator_frame("calculator.stack","Formula cursor preserves grouped input");
        enter();if(rt.error()||rt.depth()!=1||rt.top()->as_int32(0,false)!=10)test_abort(__LINE__);
    }
    while(rt.depth())rt.drop();ui.clear_shift();ui.clear_menu();host_theme=0;apply_theme(0);host_variant="normal";
    puts("PASS: grouped formulas, preserved complex literals, cursor movement without full-screen transfers and pixel-identical partial redraw in 8 themes");
}

static void postfix_entry_cases()
{
    auto text=[](const char *s){while(*s){pico_calculator_text_key(ui,Stack,unsigned(*s++),false);pico_stack_menu_key(ui,Stack,0,false,false);}};
    auto enter=[](){pico_calculator_key(ui,Stack,13,false,false);pico_stack_menu_key(ui,Stack,0,false,false);};
    for(int theme=0;theme<int(pico_themes::count);++theme)
    {
        while(rt.depth())rt.drop();rt.clear_error();Settings=settings();ui.clear_menu();ui.clear_help();ui.clear_shift();host_theme=theme;apply_theme(theme);
        text("99");enter();text("6");enter();text("!");
        if(rt.error()||rt.editing()||rt.depth()!=2||rt.top()->as_int32(0,false)!=720||rt.stack(1)->as_int32(0,false)!=99)test_abort(__LINE__);
        host_variant="immediate-factorial";calculator_frame("calculator.stack","6 Enter ! = 720");rt.drop();
        text("6");enter();text("?");
        if(rt.error()||rt.editing()||rt.depth()!=2||rt.top()->as_int32(0,false)!=21||rt.stack(1)->as_int32(0,false)!=99)test_abort(__LINE__);
        host_variant="immediate-termial";calculator_frame("calculator.stack","6 Enter ? = 21");
        text("?");if(rt.error()||rt.editing()||rt.depth()!=2||rt.top()->as_int32(0,false)!=231)test_abort(__LINE__);
        for(const char *source:{"6!","6?"})
        {
            while(rt.depth())rt.drop();text(source);
            if(rt.error()||rt.editing()||rt.depth()!=1||rt.top()->as_int32(0,false)!=(source[1]=='!'?720:21))test_abort(__LINE__);
        }
        while(rt.depth())rt.drop();text("99");enter();text("6!");
        if(rt.error()||rt.editing()||rt.depth()!=2||rt.top()->as_int32(0,false)!=720||rt.stack(1)->as_int32(0,false)!=99)test_abort(__LINE__);
        while(rt.depth())rt.drop();text("0!");
        if(rt.error()||rt.editing()||rt.depth()!=1||!rt.top()->is_one(false))test_abort(__LINE__);
        while(rt.depth())rt.drop();text("6.0?");
        if(rt.error()||rt.editing()||rt.depth()!=1||rt.top()->as_int32(0,false)!=21)test_abort(__LINE__);
        while(rt.depth())rt.drop();text("i");enter();text("!");
        if(rt.error()||rt.editing()||rt.depth()!=1||!rt.top()->is_complex())test_abort(__LINE__);
        while(rt.depth())rt.drop();text("-1");enter();text("?");
        if(!rt.error()||rt.depth()!=1||rt.top()->as_int32(0,false)!=-1)test_abort(__LINE__);
        rt.clear_error();ui.clear_editor();while(rt.depth())rt.drop();
        for(const char *op:{"!","?"})
        {
            text(op);if(!rt.error()||rt.depth())test_abort(__LINE__);
            rt.clear_error();ui.clear_editor();
        }
        for(const char *prefix:{"'N","« 6 ","READY"," "})
        {
            ui.insert(utf8(prefix),user_interface::TEXT);ui.update_mode();
            text("!?");
            std::string expected=std::string(prefix)+"!?";
            if(rt.error()||rt.depth()||std::string(reinterpret_cast<const char *>(rt.editor()),rt.editing())!=expected)test_abort(__LINE__);
            ui.clear_editor();
        }
    }
    ui.clear_shift();ui.clear_menu();host_theme=0;apply_theme(0);host_variant="normal";
    puts("PASS: immediate factorial/Termial, lower stack preservation, repeated postfix, immediate numeric entries, complex factorial, errors and unchanged edited text in 8 themes");
}

static void power_entry_cases()
{
    auto text=[](const char *s){while(*s){pico_calculator_text_key(ui,Stack,unsigned(*s++),false);pico_stack_menu_key(ui,Stack,0,false,false);}};
    auto enter=[](){pico_calculator_key(ui,Stack,13,false,false);pico_stack_menu_key(ui,Stack,0,false,false);};
    for(int theme=0;theme<int(pico_themes::count);++theme)
    {
        Settings=settings();ui.clear_help();ui.clear_menu();ui.clear_shift();host_theme=theme;apply_theme(theme);
        struct Case{const char *input;int expected;};
        for(auto c:{Case{"0^0",1},Case{"2^3",8},Case{"0^3",0},Case{"3^0",1},Case{"-2^3",-8},Case{"0.0^0.0",1},Case{"2e1^2",400},Case{"2^+3",8}})
        {
            while(rt.depth())rt.drop();rt.clear_error();
            text(c.input);enter();
            if(rt.error()||rt.depth()!=1||rt.top()->as_int32(-99,false)!=c.expected)test_abort(__LINE__);
        }
        while(rt.depth())rt.drop();text("2^-3");enter();
        if(rt.error()||rt.depth()!=1||!execute("8 *")||!rt.top()->is_one(false))test_abort(__LINE__);
        while(rt.depth())rt.drop();text("0^0");enter();
        host_variant="zero-power-zero";calculator_frame("calculator.stack","0^0 = 1");
        while(rt.depth())rt.drop();text("2^3");enter();
        host_variant="inline-power";calculator_frame("calculator.stack","2^3 = 8");
        while(rt.depth())rt.drop();text("2");enter();text("3");enter();text("^");
        if(rt.error()||rt.depth()!=1||rt.top()->as_int32(0,false)!=8)test_abort(__LINE__);
        while(rt.depth())rt.drop();text("2");enter();text("3");text("^");
        if(rt.error()||rt.depth()!=1||rt.top()->as_int32(0,false)!=8||rt.editing())test_abort(__LINE__);
        while(rt.depth())rt.drop();text("2 3");text("^");
        if(rt.error()||rt.depth()!=1||rt.top()->as_int32(0,false)!=8||rt.editing())test_abort(__LINE__);
        for(const char *source:{"'0^0'","« 2 3 ^ »","'A^B'","0 0 ^","2^3 4"})
        {
            ui.insert(utf8(source),user_interface::TEXT);ui.update_mode();
            std::string before(reinterpret_cast<const char *>(rt.editor()),rt.editing());
            if(!pico_calculator_prepare_entry(ui)||std::string(reinterpret_cast<const char *>(rt.editor()),rt.editing())!=before)test_abort(__LINE__);
            ui.remove(0,rt.editing());ui.cursor_position(0);ui.update_mode();
        }
    }
    while(rt.depth())rt.drop();ui.clear_menu();ui.clear_shift();host_theme=0;apply_theme(0);host_variant="normal";
    puts("PASS: inline numeric powers, zero powers, signed/scientific operands, RPN and preserved program/algebraic syntax in 8 themes");
}

static void termial_entry_cases()
{
    auto press=[](int key){pico_stack_menu_key(ui,Stack,key,false,false);pico_stack_menu_key(ui,Stack,0,false,false);};
    auto enter=[&](){pico_calculator_key(ui,Stack,13,false,false);press(0);};
    auto text=[&](const char *value){while(*value){pico_calculator_text_key(ui,Stack,unsigned(*value++),false);press(0);}};
    for(int theme=0;theme<int(pico_themes::count);++theme)
    {
        while(rt.depth())rt.drop();rt.clear_error();Settings=settings();ui.clear_menu();ui.clear_help();ui.clear_shift();host_theme=theme;apply_theme(theme);
        text("6?");
        if(rt.error()||rt.depth()!=1||rt.top()->as_int32(0,false)!=21)test_abort(__LINE__);
        host_variant="termial-result";calculator_frame("calculator.stack","6? = 21");
        rt.drop();text("6 Termial");enter();
        if(rt.error()||rt.depth()!=1||rt.top()->as_int32(0,false)!=21)test_abort(__LINE__);rt.drop();
        ui.insert(utf8("'N?'"),user_interface::TEXT);ui.update_mode();enter();
        if(rt.error()||rt.depth()!=1)test_abort(__LINE__);
        host_variant="termial-symbolic";calculator_frame("calculator.stack","Symbolic postfix Termial");rt.drop();
        ui.insert(utf8("'6'"),user_interface::TEXT);ui.cursor_position(2);ui.update_mode();
        object::static_object(object::ID_ProbabilitiesMenu)->evaluate();
        bool found=false;
        for(unsigned page=0,count=ui.pages();page<count&&!found;++page){
            ui.page(page);
            for(unsigned plane=0;plane<3&&!found;++plane)for(unsigned col=0;col<6&&!found;++col){
                ui.shift_plane(plane);object_p item=ui.object_for_key(38+col);
                if(item&&item->type()==object::ID_Termial){press(38+col);found=true;}
            }
        }
        if(!found||rt.error()||std::string(reinterpret_cast<const char *>(rt.editor()),rt.editing()).find("6?")==std::string::npos)test_abort(__LINE__);
        host_variant="termial-menu";calculator_frame("calculator.softmenus","Termial in Probability menu");
        enter();if(!execute("ToDecimal")||rt.error()||rt.top()->as_int32(0,false)!=21)test_abort(__LINE__);rt.drop();
        ui.insert(utf8("6"),user_interface::TEXT);ui.update_mode();object::static_object(object::ID_Termial)->insert();
        if(std::string(reinterpret_cast<const char *>(rt.editor()),rt.editing()).find("Termial")==std::string::npos)test_abort(__LINE__);
        enter();if(rt.error()||rt.top()->as_int32(0,false)!=21)test_abort(__LINE__);rt.drop();
    }
    ui.clear_menu();ui.clear_shift();host_theme=0;apply_theme(0);host_variant="normal";
    puts("PASS: native 6? and 6 Termial, exact result, symbolic postfix display, Probability menu insertion and RPN insertion in 8 themes");
}

static void math_menu_shortcut_cases()
{
    if(pico_translate_key(15)!=PICO_MATH_MENU)test_abort(__LINE__);
    auto press=[](int key){pico_stack_menu_key(ui,Stack,key,false,false);pico_stack_menu_key(ui,Stack,0,false,false);};
    auto enter=[&](){pico_calculator_key(ui,Stack,13,false,false);press(0);};
    auto select=[&](const char *name){
        for(unsigned page=0,page_count=ui.pages();page<page_count;++page){
            ui.page(page);
            for(unsigned plane=0;plane<3;++plane)for(unsigned col=0;col<6;++col){
                symbol_p label=ui.label(plane*6+col);cstring raw=ui.label_text(plane*6+col);
                if(label?label->matches(name):(raw&&!strcmp(raw,name))){ui.shift_plane(plane);press(38+col);return;}
            }
        }
        fprintf(stderr,"Missing math menu choice %s\n",name);test_abort(__LINE__);
    };
    auto select_id=[&](object::id wanted){
        for(unsigned page=0,page_count=ui.pages();page<page_count;++page){
            ui.page(page);
            for(unsigned plane=0;plane<3;++plane)for(unsigned col=0;col<6;++col){
                ui.shift_plane(plane);object_p item=ui.object_for_key(38+col);
                if(item&&item->type()==wanted){press(38+col);return;}
            }
        }
        test_abort(__LINE__);
    };
    for(int theme=0;theme<int(pico_themes::count);++theme)
    {
        while(rt.depth())rt.drop();rt.clear_error();Settings=settings();
        ui.clear_menu();ui.clear_help();ui.clear_shift();host_theme=theme;apply_theme(theme);
        if(!execute("7 'i' STO 8 'e' STO 123"))test_abort(__LINE__);
        ui.insert(utf8("'Γ(1+)'"),user_interface::TEXT);ui.cursor_position(6);ui.update_mode();
        std::string before(reinterpret_cast<const char *>(rt.editor()),rt.editing());
        pico_handle_shortcut(pico_translate_key(15));
        if(!ui.menu()||ui.menu()->type()!=object::ID_MathMenu||ui.cursor_position()!=6||rt.depth()!=1||rt.top()->as_int32(0,false)!=123||std::string(reinterpret_cast<const char *>(rt.editor()),rt.editing())!=before)test_abort(__LINE__);
        host_variant="math-shortcut";calculator_frame("calculator.softmenus","Ctrl+O Math menu");
        press(39);if(ui.menu()->type()!=object::ID_ComplexMenu)test_abort(__LINE__);
        press(38);if(rt.error()||rt.depth()!=1)test_abort(__LINE__);
        host_variant="math-imaginary-insert";calculator_frame("calculator.softmenus","Imaginary unit in expression");
        enter();if(!execute("ToDecimal")||rt.depth()!=2||!rt.top()->is_complex())test_abort(__LINE__);
        {complex_g z=complex_p(rt.top());double re=0,im=0;algebraic_g a=z->re(),b=z->im();if(!pico_conversion_real(a,re)||!pico_conversion_real(b,im)||fabs(re-0.498015668118356)>1e-12||fabs(im+0.154949828301811)>1e-12)test_abort(__LINE__);}
        rt.drop();
        if(!execute("i e +")||rt.top()->as_int32(0,false)!=15)test_abort(__LINE__);rt.drop();
        ui.insert(utf8("''"),user_interface::TEXT);ui.cursor_position(1);ui.update_mode();
        pico_handle_shortcut(PICO_MATH_MENU);press(41);
        if(ui.menu()->type()!=object::ID_PowersMenu)test_abort(__LINE__);
        select_id(object::ID_sqrt);ui.insert(unicode('9'),user_interface::DIRECT,false);
        host_variant="math-function-insert";calculator_frame("calculator.softmenus","Square-root notation");
        if(rt.error()||rt.depth()!=1)test_abort(__LINE__);enter();
        if(!execute("ToDecimal")||rt.depth()!=2||rt.top()->as_int32(0,false)!=3)test_abort(__LINE__);rt.drop();
        ui.insert(utf8("''"),user_interface::TEXT);ui.cursor_position(1);ui.update_mode();
        pico_handle_shortcut(PICO_MATH_MENU);select_id(object::ID_ConstantsMenu);
        if(!ui.menu()||ui.menu()->type()!=object::ID_ConstantsMenu)test_abort(__LINE__);
        Settings.ShowBuiltinConstants(true);ui.menu()->update(0);ui.clear_shift();select("Mathematics");select("e");
        host_variant="math-euler-insert";calculator_frame("calculator.softmenus","Explicit Euler constant");
        if(rt.error()||rt.depth()!=1)test_abort(__LINE__);enter();
        if(!execute("ToDecimal"))test_abort(__LINE__);
        {double value=0;algebraic_g number=rt.top()->as_algebraic();if(!pico_conversion_real(number,value)||fabs(value-2.718281828459045)>1e-12)test_abort(__LINE__);}rt.drop();
        ui.insert(utf8("'1+'"),user_interface::TEXT);ui.cursor_position(3);ui.update_mode();
        pico_handle_shortcut(PICO_MATH_MENU);select_id(object::ID_ConstantsMenu);select("Mathematics");select("ⅈ");
        host_variant="math-imaginary-constant";calculator_frame("calculator.softmenus","Explicit imaginary constant");
        if(rt.error()||rt.depth()!=1)test_abort(__LINE__);enter();
        if(!execute("ToDecimal")||rt.depth()!=2||!rt.top()->is_complex())test_abort(__LINE__);
        {complex_g z=complex_p(rt.top());if(z->re()->as_int32(0,false)!=1||z->im()->as_int32(0,false)!=1)test_abort(__LINE__);}rt.drop();
        ui.insert(utf8("'i+e'"),user_interface::DIRECT);unsigned cursor=ui.cursor_position();
        before.assign(reinterpret_cast<const char *>(rt.editor()),rt.editing());
        pico_handle_shortcut(PICO_MATH_MENU);press(33);
        if(ui.menu()||ui.cursor_position()!=cursor||std::string(reinterpret_cast<const char *>(rt.editor()),rt.editing())!=before||rt.depth()!=1)test_abort(__LINE__);
        press(33);if(rt.editing())press(33);
        if(rt.editing()||!execute("'i' PURGE 'e' PURGE"))test_abort(__LINE__);
        ui.show_graphics(true);pico_handle_shortcut(PICO_MATH_MENU);if(ui.menu()||!ui.showing_graphics())test_abort(__LINE__);ui.show_graphics(false);
    }
    while(rt.depth())rt.drop();rt.clear_error();Settings=settings();ui.clear_menu();ui.clear_shift();host_theme=0;apply_theme(0);host_variant="normal";
    puts("PASS: Ctrl+O reuses existing Math/Complex/Powers/Constants menus, inserts explicit constants and functions, preserves variables/stack/cursor, and cancels safely in 8 themes");
}

static void signed_entry_cases()
{
    for(int theme=0;theme<int(pico_themes::count);++theme)
    {
        while(rt.depth())rt.drop();rt.clear_error();Settings=settings();
        ui.clear_help();ui.clear_menu();ui.clear_shift();
        host_theme=theme;apply_theme(theme);
        auto text=[](const char *s){while(*s){pico_calculator_text_key(ui,Stack,unsigned(*s++),false);pico_stack_menu_key(ui,Stack,0,false,false);}};
        auto enter=[](){pico_calculator_key(ui,Stack,13,false,false);pico_stack_menu_key(ui,Stack,0,false,false);};
        text("-1");
        if(rt.error()||rt.depth()!=0||rt.editing()!=2||memcmp(rt.editor(),"-1",2))test_abort(__LINE__);
        host_variant="signed-minus-entry";pico_draw_calculator_frame(true);pico_ui_host_refresh();
        enter();
        if(rt.error()||rt.depth()!=1||rt.top()->as_int32(99,false)!=-1)test_abort(__LINE__);
        // Starting another signed number must not consume an existing stack value.
        text("-2");enter();
        if(rt.error()||rt.depth()!=2||rt.top()->as_int32(99,false)!=-2)test_abort(__LINE__);
        // Two stack values subtract immediately on the first minus key.
        text("-");
        if(rt.error()||rt.editing()||rt.depth()!=1||rt.top()->as_int32(99,false)!=1)test_abort(__LINE__);
        text("3");text("-");
        if(rt.error()||rt.depth()!=1||rt.top()->as_int32(99,false)!=-2){fprintf(stderr,"subtract depth=%u top=%d editor=%.*s error=%s\n",unsigned(rt.depth()),rt.top()?rt.top()->as_int32(99,false):999,int(rt.editing()),rt.editor(),rt.error()?cstring(rt.error()):"none");test_abort(__LINE__);}
        while(rt.depth())rt.drop();rt.clear_error();ui.clear_shift();text("i!");enter();
        if(rt.error()||rt.depth()!=1||!rt.top()->is_complex())test_abort(__LINE__);
        host_variant="imaginary-factorial";pico_draw_calculator_frame(true);pico_ui_host_refresh();
        while(rt.depth())rt.drop();text("i Factorial");enter();
        if(rt.error()||rt.depth()!=1||!rt.top()->is_complex())test_abort(__LINE__);
        while(rt.depth())rt.drop();text("i");enter();
        if(rt.error()||rt.depth()!=1||!rt.top()->is_complex())test_abort(__LINE__);
        while(rt.depth())rt.drop();
        ui.insert(utf8("I 1 10 'I^3' Σ"),user_interface::DIRECT);enter();
        if(rt.error()||rt.depth()!=1||rt.top()->as_int32(0,false)!=3025)test_abort(__LINE__);
        while(rt.depth())rt.drop();
        if(!execute("3 'i' STO"))test_abort(__LINE__);text("i!");enter();
        if(rt.error()||rt.depth()!=1||rt.top()->as_int32(0,false)!=6)test_abort(__LINE__);
        if(!execute("'i' PURGE"))test_abort(__LINE__);


        while(rt.depth())rt.drop();rt.clear_error();ui.clear_shift();text("(0;-1)!");enter();
        if(rt.error()||rt.depth()!=1||!rt.top()->is_complex())test_abort(__LINE__);
        while(rt.depth())rt.drop();rt.clear_error();ui.clear_shift();text("1e-3");enter();
        if(rt.error()||rt.depth()!=1)test_abort(__LINE__);
        algebraic_g actual=rt.top()->as_algebraic(),expected=decimal::make(1,-3);
        int cmp=0;if(!comparison::compare(&cmp,actual,expected)||cmp)test_abort(__LINE__);

    }
    while(rt.depth())rt.drop();rt.clear_error();ui.clear_menu();ui.clear_shift();Settings=settings();
    host_theme=0;apply_theme(0);host_variant="normal";
    puts("PASS: physical text-key path enters -1 and -2, preserves RPN subtraction and evaluates i! in all themes");
}

void port305_controller_cases()
{
    static blitter::pixword framebuffer[320*320*surface::BPP/32];::Screen=surface(framebuffer,320,320);
    pico_ui::menu_observer=menu_observer;pico_ui::observer=[](const char *,const char *title){host_title=title;apply_theme(host_theme);};

    if(getenv("LUMARPL_MINUS_ONLY")){signed_entry_cases();cursor_error_recovery_cases();grouped_entry_cursor_cases();postfix_entry_cases();power_entry_cases();termial_entry_cases();std::fflush(nullptr);std::_Exit(0);}
    if(getenv("LUMARPL_RECOVERY_ONLY")){cursor_error_recovery_cases();grouped_entry_cursor_cases();postfix_entry_cases();power_entry_cases();termial_entry_cases();std::fflush(nullptr);std::_Exit(0);}
    if(getenv("LUMARPL_GROUPED_ONLY")){grouped_entry_cursor_cases();postfix_entry_cases();power_entry_cases();termial_entry_cases();std::fflush(nullptr);std::_Exit(0);}
    if(getenv("LUMARPL_POSTFIX_ONLY")){postfix_entry_cases();power_entry_cases();termial_entry_cases();std::fflush(nullptr);std::_Exit(0);}
    if(getenv("LUMARPL_POWER_ONLY")){power_entry_cases();std::fflush(nullptr);std::_Exit(0);}
    if(getenv("LUMARPL_TERMIAL_ONLY")){termial_entry_cases();std::fflush(nullptr);std::_Exit(0);}
    if(getenv("LUMARPL_MATH_MENU_ONLY")){math_menu_shortcut_cases();std::fflush(nullptr);std::_Exit(0);}
    if(getenv("LUMARPL_SIGNED_ENTRY_ONLY")){signed_entry_cases();std::exit(0);}
    legacy_evaluator_cases();if(getenv("LUMARPL_LEGACY_ONLY"))std::exit(0);
    bases_cases();if(getenv("LUMARPL_BASES_ONLY"))std::exit(0);
    if(getenv("LUMARPL_RESEARCH_ONLY")){research_cases();std::exit(0);}
 if(getenv("LUMARPL_ADVANCED_ONLY")){advanced_math_cases();std::exit(0);}
 if(getenv("LUMARPL_LAB_ONLY")){math_lab_cases();advanced_math_cases();science_cases();research_cases();std::exit(0);}
    if(getenv("LUMARPL_MATRIX_NUMERIC_ONLY")){matrix_arithmetic_cases();port21_new_numeric_cases();std::exit(0);}
    if(getenv("LUMARPL_MATH_ONLY")){port21_feature_cases();matrix_arithmetic_cases();port21_new_cases();std::exit(0);}
    {FILINFO info{};strcpy(info.fname,"SOURCE.lua");if(!file_visible(info))test_abort(__LINE__);for(BYTE attribute:{BYTE(AM_DIR),BYTE(AM_HID),BYTE(AM_SYS)}){info.fattrib=attribute;if(file_visible(info))test_abort(__LINE__);}info.fattrib=0;strcpy(info.fname,"._SOURCE.lua");if(file_visible(info))test_abort(__LINE__);puts("PASS: shared browser policy hides metadata sidecars, hidden/system entries and directories");}
    for(const auto &menu:pico_ui_help::menus)
    {
        char description[1280];if(!pico_ui_help::describe(menu.id,0,"Selected action",description,sizeof description)||strlen(description)<20)test_abort(__LINE__);
    }
    {char description[1280];if(!pico_ui_help::describe("conversion_ui.currency_menu.choose1",2,"From: CAD",description,sizeof description)||!strstr(description,"source currency"))test_abort(__LINE__);}
    std::map<unsigned long long,bool> palettes;
    for(int theme=0;theme<int(pico_themes::count);++theme)
    {
        host_theme=theme;apply_theme(theme);
        using packed=blitter::pattern<blitter::mode::RGB_8BPP>;
        unsigned long long signature=(packed(pattern(Settings.Background())).bits&255)|((packed(pattern(Settings.HeaderBackground())).bits&255)<<8)|((packed(pattern(Settings.StackForeground())).bits&255)<<16);
        if(palettes[signature])test_abort(__LINE__);palettes[signature]=true;
        const std::pair<unsigned,unsigned> pairs[]={ {Settings.StackForeground(),Settings.Background()}, {Settings.HeaderForeground(),Settings.HeaderBackground()}, {Settings.DateForeground(),Settings.HeaderBackground()}, {Settings.TimeForeground(),Settings.HeaderBackground()}, {Settings.BatteryForeground(),Settings.HeaderBackground()}, {Settings.SelectionForeground(),Settings.SelectionBackground()}, {Settings.ResultForeground(),Settings.ResultBackground()}, {Settings.ErrorForeground(),Settings.ErrorBackground()}, {Settings.CursorForeground(),Settings.CursorBackground()} };
        for(auto pair:pairs)if(!pico_ui::contrast(pattern(pair.first),pattern(pair.second)))test_abort(__LINE__);
        host_variant="details-focus";pico_ui::focus("conversion_ui.currency_menu.choose1").selected=2;
        force({0x84,0x85,13});const char *rows[]={"Convert cached rates","Amount: 100","From: CAD","To: USD"};
        unsigned depth=rt.depth();if(screen_menu("conversion_ui.currency_menu.choose1","Currency (cached rates)",rows,4)!=2||rt.depth()!=depth)test_abort(__LINE__);
        host_variant="reference-offline";host_sd=false;force({'1',0x85,0xb7,'1',0x85,0x85});calculator_reference();host_sd=true;
        if(forced_index!=6||return_to_calculator)test_abort(__LINE__);
        host_variant="reference-topics";
        unsigned topic_index=0;for(const auto &topic:pico_ui_help::topics){host_variant="reference-topic-"+std::to_string(topic_index++);force({0xb7,0xb4,0x85});screen_text("reference.topic",topic.title,topic.text,strlen(topic.text));}
        unsigned help_index=0;for(const auto &help:pico_ui_help::tools){host_variant="tool-help-"+std::to_string(help_index++);force({0xb7,0xb4,0x85});screen_text(help.id,help.title,pico_ui_help::tool_text(help.id),strlen(help.text));}
        host_variant="normal";
    }
    host_theme=0;apply_theme(0);
    puts("PASS: contextual Details retain selection/state; built-in reference works without SD; eight palettes meet quantized text contrast");
    for(int theme=0;theme<int(pico_themes::count);++theme)
    {
        host_theme=theme;apply_theme(theme);host_variant="paging-first";
        const char *rows[]={"1","2","3","4","5","6","7","8","9","10"};
        auto &state=pico_ui::focus("conversion_ui.currency_pick.choose1");state.selected=0;
        host_after_key_variant="paging-next";host_variant_key=0x83;force({0x83,13});
        if(screen_menu("conversion_ui.currency_pick.choose1","Choose currency",rows,10)!=8)test_abort(__LINE__);
        host_variant="paging-previous";force({0x82,13});if(screen_menu("conversion_ui.currency_pick.choose1","Choose currency",rows,10)!=0)test_abort(__LINE__);
        force({0xb6,0x82,13});if(screen_menu("conversion_ui.currency_pick.choose1","Choose currency",rows,10)!=1)test_abort(__LINE__);
    }
    mkdir("NAVIGATION",0700);for(int i=0;i<10;++i){char path[80];snprintf(path,sizeof path,"NAVIGATION/FILE%02d.workbench",i);FILE *file=fopen(path,"wb");if(!file)test_abort(__LINE__);fclose(file);}
    for(int theme=0;theme<int(pico_themes::count);++theme)
    {
        host_theme=theme;apply_theme(theme);host_variant="files-first-page";pico_ui::focus("NAVIGATION").selected=0;
        host_after_key_variant="files-next-page";host_variant_key=0xb7;force({0xb7,13});char path[320];
        if(!document_file("NAVIGATION",".workbench",path,sizeof path)||strcmp(path,"NAVIGATION/FILE08.workbench"))test_abort(__LINE__);
    }
    host_variant="normal";host_theme=0;apply_theme(0);
    puts("PASS: labelled F2/F3 pagination, row shortcuts and disabled Previous preserve focus; action menus have <=8 visible rows");
    {
        ui.clear_menu();rt.reset();load_saved_keymap();Settings=settings();apply_theme(7);ui.alpha_plane(0);ui.clear_shift();
        auto press=[](int key){pico_stack_menu_key(ui,Stack,key,false,false);pico_stack_menu_key(ui,Stack,0,false,false);};
        force({'2','3'});return_to_calculator=false;native_settings();
        if(!return_to_calculator||!ui.menu()||ui.menu()->type()!=object::ID_ModesMenu)test_abort(__LINE__);
        press(28);press(28);press(38);
        if(rt.error()||!rt.top()||!rt.top()->as_program())test_abort(__LINE__);
        unsigned depth=rt.depth();key_push(33);key_push(0);press(13);key_pop_all();
        if(rt.error()||ui.showing_graphics()||ui.showing_help()||ui.alpha_plane()||rt.depth()!=depth+1)
        {fprintf(stderr,"FAIL: Modes -> Enter invokes shifted Show/alpha instead of ordinary Enter\n");std::exit(1);}
        ui.clear_menu();rt.reset();load_saved_keymap();apply_theme(0);ui.clear_shift();
    }
    {
        grob_g card=grob::make(320,320);auto pixels=card->pixels();pixels.fill(grob::pattern::white);
        Screen.clip(Screen.area());Screen.fill(pattern::black);Screen.clip(rect(0,220,10,230));show_grob(card);
        Screen.clip(Screen.area());
        if(Screen.pixel_color(200,100).red()!=255)
        {fprintf(stderr,"FAIL: full-screen Show inherits a stale calculator clip\n");std::exit(1);}
        Screen.clip(Screen.area());
    }
    {
        rt.reset();load_saved_keymap();Settings=settings();
        if(!execute("2 3 +")||rt.top()->as_int32(0,false)!=5)test_abort(__LINE__);rt.drop();
        if(!execute("3 Fix")||Settings.DisplayMode()!=object::ID_Fix||Settings.DisplayDigits()!=3||rt.depth())test_abort(__LINE__);
        if(!execute("16 Precision")||Settings.Precision()!=16)test_abort(__LINE__);
        if(!execute("Grad Grad")||Settings.AngleMode()!=object::ID_Grad)test_abort(__LINE__);
        if(!execute("Rad")||Settings.AngleMode()!=object::ID_Rad)test_abort(__LINE__);
        if(!execute("BeepOn")||!Settings.BeepOn()||!execute("BeepOff")||Settings.BeepOn())test_abort(__LINE__);
        auto header=Settings.header_font();if(!execute("2 StackFont 9 HeaderFont")||Settings.StackFont()!=2||Settings.header_font()!=header)test_abort(__LINE__);
        if(!execute("'Precision' RCL")||!rt.top()||rt.top()->as_int32(0,false)!=16)test_abort(__LINE__);
        rt.reset();load_saved_keymap();Settings=settings();
        puts("PASS: documented RPL examples, display digits vs precision, repeated Grad selection, Beep typed states and compact HeaderFont compatibility execute as described");
    }
    menu_back_cases();
    calculator_shortcuts_cases();
    compact_softkeys_cases();
    command_reference_cases();
    language_reference_cases();
    clean_header_cases();
    editor_example_cases();
    editor_assistance_cases();
    math_lab_cases();advanced_math_cases();science_cases();research_cases();
    compact_source_cases();
    // Reproduce the real non-forced draw after an error overlays calculator menus.
    for(int theme=0;theme<int(pico_themes::count);++theme){host_theme=theme;ui.clear_menu();rt.reset();load_saved_keymap();apply_theme(theme);ui.clear_shift();object::static_object(object::ID_DisplayModesMenu)->evaluate();
     pico_draw_calculator_frame(true);object::static_object(object::ID_Fix)->evaluate();if(!rt.error())test_abort(__LINE__);
     pico_draw_calculator_frame(false);if(!pico_ui_calculator_redraw(false)||!pico_ui_calculator_redraw(false))test_abort(__LINE__);Screen.clip(rect(200,200,220,220));pico_stack_menu_key(ui,Stack,33,false,false);pico_stack_menu_key(ui,Stack,0,false,false);if(rt.error())test_abort(__LINE__);unsigned transitions=host_transitions;pico_draw_calculator_frame(false);if(host_transitions<=transitions)test_abort(__LINE__);transitions=host_transitions;pico_draw_calculator_frame(false);if(host_transitions!=transitions)test_abort(__LINE__);
     host_variant="error-dismiss";{pico_ui::ScreenScope scope("calculator.softmenus");pico_ui::seen("After dismissing FixedDisplay error");pico_ui_host_refresh();}
     std::vector<blitter::pixword> recovered(framebuffer,framebuffer+sizeof framebuffer/sizeof *framebuffer);
     pico_draw_calculator_frame(true);bool equal=true;for(int y=28;y<320;++y)for(int x=0;x<320;++x){if(y>=320-int(MenuFont->height()+5-Settings.SquareMenus())*3 && (x>=260 || y<320-int(MenuFont->height()+5-Settings.SquareMenus())))continue;surface previous(recovered.data(),320,320);if(Screen.pixel_color(x,y).red()!=previous.pixel_color(x,y).red()||Screen.pixel_color(x,y).green()!=previous.pixel_color(x,y).green()||Screen.pixel_color(x,y).blue()!=previous.pixel_color(x,y).blue())equal=false;}
     if(!equal){fprintf(stderr,"FAIL: calculator caches leave error/footer pixels after dismissal\n");std::exit(1);}host_variant="normal";}
    for(int theme=0;theme<int(pico_themes::count);++theme)for(bool square:{false,true})
    {
        host_theme=theme;apply_theme(theme);rt.reset();load_saved_keymap();ui.clear_shift();Settings.SquareMenus(square);
        for(unsigned plane=0;plane<3;++plane)
        {
            Settings.MenuAppearance(object::ID_ThreeRowsMenus);object::static_object(object::ID_ModesMenu)->evaluate();ui.shift_plane(plane);pico_draw_calculator_frame(true);
            int height=MenuFont->height()+5-square;std::vector<blitter::pixword> full(framebuffer,framebuffer+sizeof framebuffer/sizeof *framebuffer);surface reference(full.data(),320,320);
            Settings.MenuAppearance(object::ID_SingleRowMenus);ui.menu_refresh();pico_draw_calculator_frame(false);
            int column=plane==2?1:2;for(int dy=3;dy<height-3;++dy)for(int x=column*53+5;x<column*53+47;++x){auto a=Screen.pixel_color(x,320-height+dy),b=reference.pixel_color(x,320-(plane+1)*height+dy);if(a.red()!=b.red()||a.green()!=b.green()||a.blue()!=b.blue()){fprintf(stderr,"FAIL: single-row plane %u renders wrong labels\n",plane);std::exit(1);}}
            host_variant="single-row-"+std::to_string(plane)+(square?"-square":"-round");{pico_ui::ScreenScope scope("calculator.softmenus");pico_ui::seen("Single-row RPL settings");pico_ui_host_refresh();}
        }
        Settings.MenuAppearance(object::ID_SingleRowMenus);object::static_object(object::ID_ModesMenu)->evaluate();ui.clear_shift();auto press=[](int key){pico_stack_menu_key(ui,Stack,key,false,false);pico_stack_menu_key(ui,Stack,0,false,false);};press(28);bool before=Settings.BeepOn();press(40);if(Settings.BeepOn()==before||ui.shift_plane()!=1)test_abort(__LINE__);press(40);if(Settings.BeepOn()!=before||ui.shift_plane()!=1)test_abort(__LINE__);
        ui.clear_shift();Settings.HideEmptyMenu(true);ui.clear_menu();pico_draw_calculator_frame(false);blitter::pixword expected_word=0;surface expected_surface(&expected_word,1,1);expected_surface.fill(pattern(Settings.StackBackground()));auto expected=expected_surface.pixel_color(0,0);for(int y=280;y<320;++y)for(int x=0;x<320;++x){auto pixel=Screen.pixel_color(x,y);/* Compare actual RGB332 background. */if(pixel.red()!=expected.red()||pixel.green()!=expected.green()||pixel.blue()!=expected.blue()){fprintf(stderr,"FAIL: removed menu/hint leaves stale pixels\n");std::exit(1);}}
    }
    {auto mode=Settings.DisplayMode();auto digits=Settings.DisplayDigits();rt.reset();load_saved_keymap();ui.clear_shift();object::static_object(object::ID_DisplayModesMenu)->evaluate();if(!execute("3"))test_abort(__LINE__);pico_stack_menu_key(ui,Stack,39,false,false);pico_stack_menu_key(ui,Stack,0,false,false);if(rt.error()||Settings.DisplayMode()!=object::ID_Fix||Settings.DisplayDigits()!=3)test_abort(__LINE__);Settings.DisplayMode(mode);Settings.DisplayDigits(digits);puts("PASS: FixedDisplay consumes its digit argument; Beep toggles independently without stack arguments");}
    host_variant="normal";host_theme=0;apply_theme(0);Settings.MenuAppearance(object::ID_ThreeRowsMenus);Settings.SquareMenus(false);
    puts("PASS: real frame error dismissal and SingleRow Tab planes/Beep/empty-menu cleanup in all themes and square/round styles");
    // Runtime resets below rebuild the keymap, matching production session loading.
    for(const char *source:{"« 2 3 + »","2 3 +"})
    {rt.reset();load_saved_keymap();object_g code=parse_source(source);if(!code||run_selected(code)!=object::OK||rt.depth()!=1||rt.top()->as_int32(0,false)!=5||!return_to_calculator)test_abort(__LINE__);}
    rt.reset();load_saved_keymap();{object_g code=parse_source("« « 2 3 + » »");if(run_selected(code)!=object::OK||!rt.top()->as_program())test_abort(__LINE__);}
    puts("PASS: selected RPL body runs and returns to stack; nested programs stay quoted");
    for(int theme=0;theme<int(pico_themes::count);++theme)
    {
        host_theme=theme;
        for(const auto &sample : {std::pair<const char *,int>{"« CASE 1 THEN 42 END 99 END »",42},
              {"« 42 → x « CASE 1 THEN x END 99 END » »",42},
              {"« 32 MaxCallRecursion « P 1 + » 'P' STO IFERR P THEN 1024 MaxCallRecursion 2 3 + END »",5}})
        {
            rt.reset();load_saved_keymap();ui.clear_menu();ui.clear_shift();apply_theme(theme);
            object_g code=parse_source(sample.first);
            if(!code||run_selected(code)!=object::OK||rt.error()||rt.depth()!=1||
               rt.top()->as_int32(0,false)!=sample.second||rt.call_depth())test_abort(__LINE__);
            host_variant=sample.second==5?"rpl-frame-recovery":strstr(sample.first,"→")?"rpl-frame-local":"rpl-frame-case";
            calculator_frame("calculator.stack","RPL conditional frame result");
        }
    }
    host_variant="normal";host_theme=0;apply_theme(0);
    puts("PASS: production Run CASE/local cleanup and frame-exhaustion recovery leave correct results and empty return stack in eight themes");
    {
        rt.reset();load_saved_keymap();auto limit=Settings.MaxCallRecursion();Settings.MaxCallRecursion(1);
        unsigned accepted=0;
        while(accepted<256&&rt.run_push(nullptr,nullptr))++accepted;
        if(accepted==256||!rt.error())test_abort(__LINE__);
        rt.clear_error();
        if(rt.run_next(0)||rt.error()||rt.call_depth())test_abort(__LINE__);
        Settings.MaxCallRecursion(limit);
        if(!execute("2 3 +")||rt.depth()!=1||rt.top()->as_int32(0,false)!=5)test_abort(__LINE__);
        printf("PASS: empty conditional frame allocation reports exhaustion after %u accepted frames, drains cleanly and recovers arithmetic\n",accepted);
    }


    rt.reset();load_saved_keymap();if(!execute("« 2 3 + » 'EQUATION' STO"))test_abort(__LINE__);force({0x82});return_to_calculator=false;home_browser();if(!return_to_calculator||rt.top()->as_int32(0,false)!=5)test_abort(__LINE__);
    rt.reset();load_saved_keymap();force({0x82});return_to_calculator=false;edit_file("data/UI.48s","UI.48s",false);if(!return_to_calculator||rt.top()->as_int32(0,false)!=5)test_abort(__LINE__);
    rt.reset();load_saved_keymap();return_to_calculator=false;run_file("data/UI.48s");if(!return_to_calculator||rt.top()->as_int32(0,false)!=5)test_abort(__LINE__);
    rt.reset();load_saved_keymap();if(!execute("« 2 3 + » 'EQUATION' STO"))test_abort(__LINE__);force({'4','1',0x82});pico_setup_menu();if(forced_index!=3||rt.top()->as_int32(0,false)!=5||!return_to_calculator)test_abort(__LINE__);
    puts("PASS: full Home > Programming > HOME > F2 unwinds all menus to calculator results");
    rt.reset();load_saved_keymap();{object_g code=parse_source("« DUP * »");if(!execute("7")||run_selected(code)!=object::OK||rt.top()->as_int32(0,false)!=49)test_abort(__LINE__);}
    rt.reset();load_saved_keymap();{object_g code=parse_source("« 1 0 / »");if(run_selected(code)!=object::ERROR||!rt.error())test_abort(__LINE__);rt.clear_error();}
    rt.reset();load_saved_keymap();{files_g disk=files::make("");text_g name=text::make("data/BINARY.48b");object_g code=parse_source("« 2 3 + »");if(!disk->store_binary(name,code))test_abort(__LINE__);run_file("data/BINARY.48b");if(!rt.top()||rt.top()->as_int32(0,false)!=5)test_abort(__LINE__);}
    puts("PASS: selected RPL preserves stack arguments, reports runtime error and executes upstream binary format");
    rt.reset();load_saved_keymap();{object_g code=parse_source("« 1 2 + »");program::halted=false;program::count_interrupted=32;key_push(33);key_pop();key_push(33);/* Simulator key_tail peeks previous slot; seed it as well. */if(program::run(code,true)!=object::OK||!program::halted||rt.depth()!=0)test_abort(__LINE__);finish_selected(object::OK);if(program::halted||rt.call_depth())test_abort(__LINE__);key_pop_all();rt.clear_error();}
    puts("PASS: upstream RPL cancellation interrupts execution and recovers for subsequent Run");

    {pico_sprite saved;bool recovered=false;if(!pico_sprite_load("sprites/DEMO.sprite",saved,recovered))test_abort(__LINE__);if(!pico_sprite_save("sprites/TEST.sprite",saved))test_abort(__LINE__);pico_sprite changed=saved;changed.pixels[0]^=255;host_fail_save=true;if(pico_sprite_save("sprites/TEST.sprite",changed))test_abort(__LINE__);host_fail_save=false;pico_sprite restored;if(!pico_sprite_load("sprites/TEST.sprite",restored,recovered)||memcmp(&saved,&restored,sizeof saved))test_abort(__LINE__);
     uint8_t data[pico_sprite::bytes];saved.encode(data);{FILE *f=fopen("sprites/RECOVER.sprite.bak","wb");if(!f)test_abort(__LINE__);fwrite(data,1,sizeof data,f);fclose(f);data[12]^=1;f=fopen("sprites/RECOVER.sprite","wb");if(!f)test_abort(__LINE__);fwrite(data,1,sizeof data,f);fclose(f);}if(!pico_sprite_load("sprites/RECOVER.sprite",restored,recovered)||!recovered||memcmp(&saved,&restored,sizeof saved))test_abort(__LINE__);
     puts("PASS: sprite transactional save failure retains previous valid drawing; corrupt primary recovers checked backup");}

    puts("PASS: actual HOME F2, SD Run and editor Save & run produce numeric stack results");
    {pico_sprite reference;bool backup=false;if(!pico_sprite_load("sprites/DEMO.sprite",reference,backup))test_abort(__LINE__);sprite_document=pico_sprite{};sprite_dirty=false;force({0x84,'1',0x81,' ',0x85,0x83});sprite_editor();if(memcmp(&reference,&sprite_document,sizeof reference)||sprite_dirty)test_abort(__LINE__);
     force({' ',0x84,'1',0x81,13,0x85,0xb7,' ',0x85,0x83});sprite_editor();pico_sprite saved;if(!pico_sprite_load("sprites/LAST.sprite",saved,backup)||memcmp(&saved,&sprite_document,sizeof saved)||sprite_dirty)test_abort(__LINE__);puts("PASS: sprite Discard restores the loaded/saved drawing, including Save before a cancelled Open");}

    {pico_network_settings settings;strcpy(settings.ssid,"Host");strcpy(settings.password,"test-password");currency_update_options options;
     const char *codes="AED AUD BRL CAD CHF CNY CZK DKK GBP HKD HUF IDR ILS INR JPY KRW MXN MYR NOK NZD PHP PLN SAR SEK SGD THB TRY USD ZAR";
     host_rates_csv="date,base,quote,rate\n";for(const char *p=codes;*p;p+=4){host_rates_csv+="2026-10-30,EUR,";host_rates_csv.append(p,3);host_rates_csv+=",2\n";if(!p[3])break;}
     force({0x85});if(!currency_refresh(settings,currency_active_rates,options)||currency_active_rates.count!=30)test_abort(__LINE__);currency_initialized=true;pico_currency_preferences pair;strcpy(pair.from,"EUR");strcpy(pair.to,"CAD");currency_preferences_remember(pair);
     for(int theme=0;theme<int(pico_themes::count);++theme)
     {
         host_theme=theme;apply_theme(theme);pico_ui::focus("conversion_ui.currency_menu.choose1").selected=0;
         host_variant="currency-before-refresh";host_after_key_variant="currency-after-refresh";host_variant_key=0x85;
         unsigned depth=rt.depth();force({'6',0x85,0x81,0x81,0x85,0x85});currency_menu(settings);
         if(forced_index!=6||rt.depth()!=depth+1||!rt.top()||rt.top()->as_int32(0,false)!=200||pico_ui::focus("conversion_ui.currency_menu.choose1").selected!=0){fprintf(stderr,"Currency idx=%zu depth=%zu old=%u top=%d focus=%d theme=%d error=%s\n",forced_index,rt.depth(),depth,rt.top()?rt.top()->as_int32(0,false):-999,pico_ui::focus("conversion_ui.currency_menu.choose1").selected,theme,rt.error()?cstring(rt.error()):"none");test_abort(__LINE__);}rt.drop();
         host_variant="currency-settings";force({'7',0x85,0x85});currency_menu(settings);
     }
     host_variant="normal";host_theme=0;apply_theme(0);
     puts("PASS: actual currency Refresh returns to visible Convert and immediately copies correct conversion; Manage Back restores main controls in all themes");
     {object_g value=pico_convert_currency("100",currency_active_rates,currency_active_rates.index("EUR"),currency_active_rates.index("CAD"));if(!value||value->as_int32(0,false)!=200)test_abort(__LINE__);}
     pico_currency_rates disk;bool backup=false;if(!currency_load(disk,backup))test_abort(__LINE__);
     host_fail_save=true;force({0x85});if(!currency_refresh(settings,currency_active_rates,options))test_abort(__LINE__);host_fail_save=false;
     {pico_currency_rates reordered=currency_active_rates;unsigned cad=reordered.index("CAD"),usd=reordered.index("USD");std::swap(reordered.rows[cad],reordered.rows[usd]);if(strcmp(reordered.rows[reordered.index("CAD")].code,"CAD")||strcmp(reordered.rows[reordered.index("USD")].code,"USD")||reordered.index("ZZZ",1)!=1||reordered.index("ZZZ",100)!=0)test_abort(__LINE__);}
     pico_currency_rates previous=currency_active_rates;host_rates_csv="broken";force({0x85});if(currency_refresh(settings,currency_active_rates,options)||memcmp(&previous,&currency_active_rates,sizeof previous))test_abort(__LINE__);host_rates_csv.clear();
     host_sd=false;force({0x85});currency_menu(settings);host_sd=true;if(memcmp(&previous,&currency_active_rates,sizeof previous))test_abort(__LINE__);
     puts("PASS: immediate currency conversion, persistence, failed save, invalid update and reopening without SD");}

    {auto press=[](int key){pico_stack_menu_key(ui,Stack,key,false,false);pico_stack_menu_key(ui,Stack,0,false,false);};
     rt.reset();load_saved_keymap();ui.clear_shift();object::static_object(object::ID_ModesMenu)->evaluate();press(28);if(ui.shift_plane()!=1)test_abort(__LINE__);bool beep=Settings.BeepOn();press(40);if(Settings.BeepOn()==beep||ui.shift_plane()!=1)test_abort(__LINE__);press(40);if(Settings.BeepOn()!=beep||ui.shift_plane()!=1)test_abort(__LINE__);
     ui.insert(utf8("« 1 2 "),user_interface::DIRECT);object::static_object(object::ID_Catalog)->evaluate();ui.clear_shift();press(28);press(28);unsigned plane=ui.shift_plane();char selected[40]={};ui.object_for_key(38)->render(selected,sizeof selected);if(strcmp(selected,"<"))test_abort(__LINE__);press(38);if(ui.shift_plane()!=plane||!rt.editing()||std::string(reinterpret_cast<const char *>(rt.editor()),rt.editing()).find('<')==std::string::npos)test_abort(__LINE__);if(rt.editing())press(33);if(!ui.menu()||ui.menu()->type()!=object::ID_ModesMenu||ui.shift_plane()||!rt.editing())test_abort(__LINE__);press(33);if(ui.menu()||!rt.editing())test_abort(__LINE__);press(33);if(rt.editing())test_abort(__LINE__);rt.clear_error();ui.clear_shift();
     rt.reset();load_saved_keymap();ui.clear_shift();object::static_object(object::ID_ModesMenu)->evaluate();ui.shift_plane(0);press(43);if(!ui.menu()||ui.menu()->type()!=object::ID_UserInterfaceModesMenu)test_abort(__LINE__);
     puts("PASS: sixth RPL soft key dispatches UI settings; physical Ctrl+F5 translation remains unchanged");
     puts("PASS: actual Modes/Beep and Catalog F1 preserve Tab row across dispatch and release");}

    {font::glyph_info g;pico_ui::compact()->glyph('5',g);if(g.w<4||g.x+int(g.w)>int(g.advance))test_abort(__LINE__);printf("PASS: complete compact digit 5 bitmap width %u, advance %u\n",g.w,g.advance);}
    for(int theme=0;theme<int(pico_themes::count);++theme)
    {
        host_theme=theme;apply_theme(theme);next_item.clear();
        return_to_calculator=false;
        for(int pass=0;pass<10;++pass)pico_setup_menu();
        for(int pass=0;pass<8;++pass){next_item.erase("conversion_ui.pico_conversions_menu.choose1");next_item.erase("conversion_ui.currency_menu.choose1");next_item.erase("form_ui.pico_forms_menu.choose1");next_item.erase("path_ui.pico_path_menu.choose1");pico_graph_menu();pico_workbench_menu();pico_data_menu();pico_forms_menu();pico_notebooks_menu();pico_program_browser();pico_conversions_menu();pico_wifi_menu();pico_path_menu();pico_ode_menu();pico_about_menu();pico_settings_menu();native_help();native_files();}
        {bool recovered=false;if(!pico_sprite_load("sprites/DEMO.sprite",sprite_document,recovered))test_abort(__LINE__);sprite_dirty=false;}
        host_variant="sprite-colour";force({0x82,0xb7,13,' ',0x84,'7',0x85,0x85,0x83});sprite_editor();
        host_variant="glyph-boundaries";force({0x85});screen_text("lua.output","Compact digits","0123456789 55555 5.5 (5) 5%\n5555555555\nStatus: 5 | Ln 5 Col 5",strlen("0123456789 55555 5.5 (5) 5%\n5555555555\nStatus: 5 | Ln 5 Col 5"));
        host_variant="sprite-lua";{pico_lua_result result;const char *source="screen.clear('black');assert(screen.sprite('DEMO.sprite',40,60));assert(screen.sprite('DEMO.sprite',-8,310));local ok=screen.sprite('../DEMO.sprite',0,0);assert(ok==nil);screen.text(8,8,'0123456789 55555 (5) 5%');screen.show()";if(!pico_lua_run(source,strlen(source),&result)||!result.graphics)test_abort(__LINE__);auto middle=Screen.pixel_color(47,67),transparent=Screen.pixel_color(40,60),edge=Screen.pixel_color(0,317);if(middle.red()!=109||middle.green()!=109||middle.blue()!=170||transparent.red()||transparent.green()||transparent.blue()||edge.red()!=146)test_abort(__LINE__);}
        host_variant="normal";
        char formula[160]="X^2";force({0x81,0x85,0x82,0x85,0x84,0x85,0x85});formula_entry("Formula",formula);
        force({0x85});formula_preview("X^2");
        pico_ui::focus("desktop.edit_file.choose1").selected=0;force({0x84,0xb7,'2',0x85,0x85,0x85});edit_file("data/UI.48s","UI.48s",false);
        host_variant="editor-help";pico_ui::focus("desktop.edit_file.choose1").selected=0;force({0x84,0xb7,'2','1',0x85,0x85});edit_file("data/UI.48s","UI.48s",false);if(forced_index!=6)test_abort(__LINE__);host_variant="normal";
        pico_ui::focus("desktop.edit_file.choose1").selected=0;force({0x84,'5','p','r','i','n','t',13,'e','c','h','o',13,0x85,0x85});edit_file("lua/HELLO.lua","HELLO.lua",false,source_language::lua);
        force({'x',0x85,0x85,0x81,0x85});edit_file("lua/UI_UNSAVED.lua","UI_UNSAVED.lua",true,source_language::lua);
        force({0x85});assemble_file("NOT_VALID\n","chip8/BAD.asm");
        {char broken[80]="«\n\"broken";pico_text_editor editor{broken,sizeof broken,strlen(broken),0,false};object_g parsed=parse_source(broken);if(!parsed){force({0x85});source_error(broken,editor);}rt.clear_error();}
        force({':','h','e','l','p',13,0x85,27});lua_console();
        force({0x85});show_lua_file("lua/HELLO.lua");
        force({0x85});sprite_editor();host_after_key_variant="sprite-dirty";host_variant_key=' ';force({' ',0x85,13,0x85,0x83});sprite_editor();host_variant="normal";force({0x85});variable_browser();
        {program_g parsed=program::parse(utf8("« 2 3 + »"),strlen("« 2 3 + »"));if(parsed){force({0x85});debug_program(object_p(+parsed));}}
        force({0x85});memory_status();force({0x85});screen_text("desktop.native_help.view_text1","Keyboard shortcuts","Ctrl+T Home\nF1 Open\nF5 Back",sizeof("Ctrl+T Home\nF1 Open\nF5 Back")-1);
        force({0x85});screen_text("desktop.native_help.view_text2","Credits / licenses",pico_credits::text,strlen(pico_credits::text));
        strcpy(workbench.single,"X+2=5");force({0x85});workbench_symbolic();
        force({13,13,0x85});workbench_interval();force({0x85});use_calculus_result(integer::make(42),"Derivative");
        force({0x81,0x85,0x82,'1',13,13,0x85,0x83,'1',0x85,0x85});multi_plot();
        force({0xb7,0xb4,0x85});multi_plot();force({'1',0xb7,0xb4,0x85,0x85});surface_menu();
        graph_revision.dirty=true;force({0x85});if(pico_prepare_exit())test_abort(__LINE__);
        workbench_revision.dirty=true;
        host_variant="exit-save-cancel";force({0x81,27,0x85});if(pico_prepare_exit()||!workbench_revision.dirty)test_abort(__LINE__);
        host_variant="exit-save-failed";
        {uint8_t data[2048],retained[2048];size_t length=workbench.encode(data,sizeof data);if(!length)test_abort(__LINE__);const char *file="workbenches/UIEXITFAIL.workbench";FILE *f=fopen(file,"wb");fwrite(data,1,length,f);fclose(f);
         host_fail_save=true;force({0x81,'U','I','E','X','I','T','F','A','I','L',13,0x81,0x85});if(pico_prepare_exit()||!workbench_revision.dirty)test_abort(__LINE__);host_fail_save=false;
         f=fopen(file,"rb");size_t actual=fread(retained,1,sizeof retained,f);fclose(f);if(actual!=length||memcmp(data,retained,length))test_abort(__LINE__);remove(file);}
        host_variant="exit-no-sd";host_sd=false;force({0x81,0x85,0x85});if(pico_prepare_exit()||!workbench_revision.dirty)test_abort(__LINE__);host_sd=true;host_variant="normal";
        {pico_network_settings settings;strcpy(settings.ssid,"Host test");strcpy(settings.password,"not-a-real-password");pico_currency_rates rates;currency_update_options options;force({0x84,0x85,0x85});currency_refresh(settings,rates,options);}
        host_variant="wifi-details";force({0x84,0x85,0x85});wifi_status();host_no_network_attempts=true;host_variant="wifi-empty";force({0x84,0x85,0x85});wifi_status();host_no_network_attempts=false;host_variant="normal";
        force({0x85});show_lua_file("lua/DRAW.lua");
        force({0x85});show_lua_file("lua/BALL.lua");
        force({0x81,0x83,0x85});show_lua_file("lua/TEXT.lua");
        {program_g parsed=program::parse(utf8("« 2 3 + »"),strlen("« 2 3 + »"));symbol_g name=symbol::make("UITEST");if(parsed&&name)directory::store_here(name,object_p(+parsed));symbol_g number=symbol::make("NUMBER");directory::store_here(number,integer::make(42));}
        force({0x81,0x85,0x85});home_browser();force({13,0x85,0x85});variable_browser();
        force({0x84,'2',0x85,0x85});development_files(source_language::assembly);
        {FILE *file=fopen("chip8/BAD.ch8","wb");unsigned char bad[]={0xff,0xff};fwrite(bad,1,2,file);fclose(file);force({0x85});run_chip_file("chip8/BAD.ch8");}
        {double values[]={1,1,2};force({0x85});workbench_use(values,3);force({0x85});workbench_use(values,3,1);}
        {pico_document_access<pico_notebook> access(notebook_cache,notebook);if(notebook){strcpy(notebook->cells[0].expression,"BAD_UNKNOWN_FUNCTION(1)");force({0x85});notebook_run(0);}}
        force({'3',13,13,0x85,0x85});calculus_menu();
        force({0x82,'2','1',13,13,0x85,0x85});multi_plot();
        {pico_document_access<pico_path_scene> access(path_scene_cache,path_scene);if(path_scene){force({0x81,0x85,0x83,'1',0x85,0x85});path_plot();}}
        {pico_document_access<pico_ode_scene> access(ode_scene_cache,ode_scene);if(ode_scene){force({'1',0x82,0x85,0x83,'1',0x85,0x85,0x85});pico_ode_menu();}}
        {pico_plot_request scene{};scene.active[0]=scene.active[1]=true;strcpy(scene.label[0],"X^2");strcpy(scene.label[1],"X");scene.window[0]=-2;scene.window[1]=2;
         plot_source source{};pico_plot_analysis::result overlay;bool shade=false;double lower=0,upper=1;char heading[100];pico_plot_explore::selection bounds;bounds.first=0;bounds.last=1;bounds.count=2;int other=-1;
         force({'2',0x85});analyse_curve(scene,source,0,0,overlay,shade,lower,upper,heading,sizeof heading,bounds,other);
         force({'1',0x85});analyse_curve(scene,source,0,0,overlay,shade,lower,upper,heading,sizeof heading,bounds,other);}
        host_variant="corrupt";{FILE *f=fopen("workbenches/000BAD.workbench","wb");fputs("bad",f);fclose(f);force({'2','1',0x84,0x85,0x85});workbench_saved();remove("workbenches/000BAD.workbench");}host_variant="normal";
        // Render the actual 320x320 calculator UI implementations as well.
        ui.clear_help();ui.show_graphics(false);ui.draw_start(true);Screen.clip(Screen.area());apply_theme(theme);
        calculator_frame("calculator.stack","Calculator");
        object::static_object(object::ID_MainMenu)->evaluate();calculator_frame("calculator.softmenus","RPL soft menus");
        ui.insert(utf8("1+2"),user_interface::DIRECT);calculator_frame("calculator.entry","Calculator entry");ui.end_edit();
        ui.load_help(utf8("Overview"));calculator_frame("calculator.help","Calculator reference");ui.clear_help();
        force({0x85});screen_text("files.details","File details","A very long Unicode filename αβ that remains fully accessible.lua",strlen("A very long Unicode filename αβ that remains fully accessible.lua"));
        {FILE *f=fopen("chip8/000BINARY.ch8","wb");if(!f)test_abort(__LINE__);unsigned char rom[]={0x00,0xe0};fwrite(rom,1,2,f);fclose(f);pico_ui::focus("files.chip8").selected=0;force({0x81,0x85,0x85});development_files(source_language::assembly);remove("chip8/000BINARY.ch8");}
        {FILE *f=fopen("data/000BINARY.48b","wb");fputs("fixture",f);fclose(f);force({0x81,0x85,0x85});file_browser();remove("data/000BINARY.48b");}
        host_variant="empty";force({0x85});lua_output_view("Lua finished","",0);
        host_variant="long-unicode";{pico_ui::ScreenScope scope("dialog.notice");const char *prose="Readable proportional text: α β γ. Long filenames and values remain available in Details. LongTokenABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ\nLine two\n";force({0xb7,0xb4,0x85});view_text("Long text",prose,strlen(prose));}host_variant="normal";
        {const char *code="'X^2' 'EQ' STO 'X' INDEP -10 10 XRNG -6 6 YRNG CLLCD DrawAxes DRAW";program_g parsed=program::parse(utf8(code),strlen(code));if(!parsed||program::run(object_p(+parsed))!=object::OK)test_abort(__LINE__);pico_ui::ScreenScope scope("calculator.graphics");pico_ui::seen("RPL graphics");pico_ui_host_refresh();ui.show_graphics(false);}
        host_variant="missing-sd";host_sd=false;force({0x85});pico_program_browser();force({0x85});pico_forms_menu();force({0x85});notice("SD unavailable","Insert SD to save; calculations remain available");host_sd=true;
        host_variant="failed-save";host_fail_save=true;force({'x',0x81,0x85,0x85,0x83});edit_file("lua/HELLO.lua","HELLO.lua",false,source_language::lua);host_fail_save=false;host_variant="normal";
        host_after_key_variant="columns";host_variant_key=0x83;force({0x83,0x85});lua_output_view("Lua table","X    Y\n1    2\n10   20",strlen("X    Y\n1    2\n10   20"));host_variant="normal";
        host_after_key_variant="memory-timings";host_variant_key=0xb7;force({0xb7,0x85});memory_status();host_variant="normal";
        host_variant="empty";mkdir("EMPTY",0700);{char path[320];force({0x85});if(document_file("EMPTY",".none",path,sizeof path))test_abort(__LINE__);}host_variant="normal";
        {ui.insert(utf8("12345"),user_interface::DIRECT);size_t bytes=rt.editing();force({0x85});pico_conversions_menu();if(rt.editing()!=bytes)test_abort(__LINE__);force({0x85});if(pico_prepare_session_action()||rt.editing()!=bytes)test_abort(__LINE__);ui.end_edit();}
        {const char *lines[]={"Long system messages now wrap safely inside the screen, keeping clear of the action bar.","Esc returns to the calculator."};ui.draw_message("System message",lines[0],lines[1]);rt.value_error();ui.draw_error();rt.clear_error();}
    }
    // Persist the real dropdown selections, then rebuild runtime state as on restart.
    for(int theme=0;theme<int(pico_themes::count);++theme)
    {
        host_theme=theme;apply_theme(theme);host_variant="pair-selecting";pico_network_settings preferences_network;
        for(const char *suffix:{"",".bak",".rollback",".tmp"})remove((std::string(pico_currency_preferences::path)+suffix).c_str());
        currency_preferences={};currency_preferences_loaded=currency_preferences_dirty=false;
        currency_active_rates={};currency_active_rates.count=3;currency_initialized=true;
        const char *codes[]={"EUR","CAD","USD"},*values[]={"1","2","4"};
        for(int i=0;i<3;++i){auto &r=currency_active_rates.rows[i];strcpy(r.code,codes[i]);strcpy(r.rate,values[i]);strcpy(r.date,"2026-10-30");}
        force({'3','2','4','3',0x85});currency_menu(preferences_network);
        pico_currency_preferences saved;if(!saved.load()||strcmp(saved.from,"CAD")||strcmp(saved.to,"USD")||currency_preferences_dirty||forced_index!=5)test_abort(__LINE__);
        unsigned writes=host_write_opens;force({'3','2',0x85});currency_menu(preferences_network);if(host_write_opens!=writes)test_abort(__LINE__);
        currency_preferences={};currency_preferences_loaded=currency_preferences_dirty=false;
        host_variant="pair-restored";force({0x85});currency_menu(preferences_network);
        if(strcmp(currency_preferences.from,"CAD")||strcmp(currency_preferences.to,"USD")||host_write_opens!=writes)test_abort(__LINE__);
        std::swap(currency_active_rates.rows[1],currency_active_rates.rows[2]);
        unsigned depth=rt.depth();force({'1',0x81,0x85,0x85});currency_menu(preferences_network);if(rt.depth()!=depth+1||rt.top()->as_int32(0,false)!=200)test_abort(__LINE__);rt.drop();
        currency_active_rates.count=2;host_variant="pair-missing-rate";force({'1',0x85,0x85});currency_menu(preferences_network);if(rt.depth()!=depth||strcmp(currency_preferences.from,"CAD"))test_abort(__LINE__);
        force({'7','2','C','A','D',13,'2',13,'2','0','2','6','-','1','0','-','3','0',13,0x85});currency_menu(preferences_network);
        force({'1',0x81,0x85,0x85});currency_menu(preferences_network);if(rt.depth()!=depth+1||rt.top()->as_int32(0,false)!=200)test_abort(__LINE__);rt.drop();
        force({'5',0x85});currency_menu(preferences_network);if(!saved.load()||strcmp(saved.from,"USD")||strcmp(saved.to,"CAD"))test_abort(__LINE__);
        // Failed autosave is non-modal and retains the last valid disk pair.
        host_fail_save=true;host_variant="pair-save-failed";force({'3','1',0x85});currency_menu(preferences_network);
        if(!currency_preferences_dirty||strcmp(currency_preferences.from,"EUR")||!saved.load()||strcmp(saved.from,"USD"))test_abort(__LINE__);
        host_variant="pair-save-status";force({0x85});currency_menu(preferences_network);
        host_fail_save=false;host_sd=false;host_variant="pair-no-sd";force({'4','2',0x85});currency_menu(preferences_network);
        if(!currency_preferences_dirty||strcmp(currency_preferences.to,"USD"))test_abort(__LINE__);
        host_sd=true;host_variant="pair-retried";force({0x85});currency_menu(preferences_network);
        if(currency_preferences_dirty||!saved.load()||strcmp(saved.from,"EUR")||strcmp(saved.to,"USD"))test_abort(__LINE__);
        writes=host_write_opens;force({'3',0x85,0x85});currency_menu(preferences_network);if(host_write_opens!=writes)test_abort(__LINE__);
    }
    host_variant="normal";
    puts("PASS: actual From/To autosave, unchanged selection avoids writes, reopen/restart, reordered/missing rates, Swap, failed-save session retention and silent retry in all themes");
    port21_feature_cases();
    matrix_arithmetic_cases();
    port21_new_cases();
    FILE *report=fopen("../../reports/port305-controller-visits.json","w");fprintf(report,"{\n");bool first=true;for(auto &v:visits){fprintf(report,"%s  \"%s\": %u",first?"":",\n",v.first.c_str(),v.second);first=false;}fprintf(report,"\n}\n");fclose(report);
    if(!host_transfers["desktop.plot_curves.page2"][261]||!host_transfers["desktop.draw_surface.page1"][223])test_abort(__LINE__);
    FILE *transfers=fopen("../../reports/port305-model-transfers.json","w");fprintf(transfers,"{\n");bool first_transfer=true;for(auto &screen:host_transfers){fprintf(transfers,"%s  \"%s\": {",first_transfer?"":",\n",screen.first.c_str());bool first_rows=true;for(auto &rows:screen.second){fprintf(transfers,"%s\"%u\": %u",first_rows?"":", ",rows.first,rows.second);first_rows=false;}fprintf(transfers,"}");first_transfer=false;}fprintf(transfers,"\n}\n");fclose(transfers);
    puts("PASS: exit Save all cancellation, failed replacement preserving valid previous bytes and missing SD all keep application open and document dirty");
    puts("PASS: production 2D trace refreshes 261 rows, 3D rotation 223; host/model counts, device timings not measured");
    printf("PASS: actual production controllers, %u keys and %u frames; %zu screen IDs\n",host_keys,host_frames,visits.size());fflush(stdout);std::exit(0);
}
