// Native PicoCalc menus, source editor, and graph controls (GPL-3.0).
#include "platform.h"
#include "credits.h"
#include "information_text.h"
#include "ui/typography.h"
#include "ui/legacy.h"
#include "ui/screen.h"
#include "ui/help_data.h"
#include "reference_store.h"
#include "ui/reference_data.h"
#include "ui/language_reference_data.h"
#include "ui/editor_examples.h"
#include "ui/editor_commands.h"
extern const unsigned char PicoSourceFont_sparse_font_data[];
#include "ui/themes.h"
#include "bulk_storage.h"
#include "workspace.h"
#include "sample_cache.h"
#include "source_backing.h"
#include "memory_profile.h"
#include "battery.h"
#include "battery_bus.h"
#include "target.h"
#include "settings.h"
#include "sysmenu.h"
#include "user_interface.h"
#include "program.h"
#include "stack.h"
#include "graphics.h"
#include "files.h"
#include "variables.h"
#include "text_editor.h"
#include "editor_tools.h"
#include "input.h"
#include "wireframe.h"
#include "graph_scene.h"
#include "plot_api.h"
#include "plot_engine.h"
#include "plot_analysis.h"
#include "plot_explore.h"
#include "formula_tools.h"
#include "calculus.h"
#include "workbench_engine.h"
#include "native_menu.h"
#include "native_layout.h"
#include "workbench_file.h"
#include "data_engine.h"
#include "data_file.h"
#include "data_curve.h"
#include "ode_engine.h"
#include "ode_file.h"
#include "graph_export.h"
#include "path_engine.h"
#include "path_file.h"
#include "form_engine.h"
#include "matrix_engine.h"
#include "matrix_advanced.h"
#include "conversion_engine.h"
#include "complex_engine.h"
#include "geometry_math.h"
#include "math_lab_engine.h"
#include "advanced_math_engine.h"
#include "science_engine.h"
#include <memory>
#include "research_document.h"
#include "research_engine.h"
#include "contour_layer.h"
#include "memory_observer.h"
#include "finance_extended.h"
#include "conversion_engine.h"
#include "currency_file.h"
#include "currency_preferences.h"
#include "unit_catalog.h"
#include "currency_csv.h"
#include "network.h"
#include "network_settings.h"
#include "network_time.h"
#include "timezone_file.h"
#include "form_file.h"
#include "notebook_engine.h"
#include "notebook_file.h"
#include "grob.h"
#include "surface_evaluator.h"
#include "chip8.h"
#include "sprite_file.h"
#include "chip8_assembler.h"
#include "lua_runner.h"
#include "source_style.h"
#include "pico/stdlib.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <strings.h>
#include <cctype>
#include <cmath>
#include <new>

extern const unsigned char PicoHeaderFont_sparse_font_data[];
#ifdef PICO_UI_HOST
int pico_ui_host_read_key();
#endif

namespace {
constexpr size_t source_capacity=16*1024;
enum class source_language { rpl,assembly,lua };
void editor_language_reference(source_language);
char editor_clipboard[257]={};
pico_graph_scene graph_scene;
pico_ui::Revision graph_revision;
int view_text(const char *title,const char *body,size_t length,pico_ui::TextRole role=pico_ui::TextRole::prose,pico_ui::Actions actions={{nullptr,nullptr,nullptr,nullptr,"Back"}});

int screen_menu(const char *,const char *,const char *const *,int,const char * =nullptr,const char * =nullptr);
int screen_text(const char *,const char *,const char *,size_t,pico_ui::TextRole=pico_ui::TextRole::prose,pico_ui::Actions={{nullptr,nullptr,nullptr,nullptr,"Back"}});
bool screen_result(const char *,object_p,const char *,const char * ="",bool=false,const char * ="Plot");
void screen_page(const char *,const char *,pico_ui::Actions={{nullptr,nullptr,nullptr,nullptr,"Back"}});

int read_key_raw()
{
#ifdef PICO_UI_HOST
    return pico_ui_host_read_key();
#else
    for (;;) { pico_poll_background();int c=pico_keyboard_poll(); if(c>=0){pico_ui_input_begin();return c;} sleep_ms(5); }
#endif
}
void flush_display()
{
    mark_dirty(0,0,319,319);lcd_refresh();
}
#include "ui/shell.inc"

#include "ui/dialogs.inc"
#include "ui/legacy.inc"

void formula_preview(const char *source)
{
    cleaner temporary;expression_g equation=pico_formula_parse(source);
    if(!equation){engine_error();return;}
    object_g display=object_p(+equation);
    screen_page("desktop.formula_preview.page1","Formula preview",pico_ui::Actions{{nullptr,nullptr,nullptr,nullptr,"Back"}});
    grapher graph(300,210,Settings.ResultFont(),grob::pattern::black,grob::pattern::white,false,true);
    graph.duration=1000;grob_g image;
    do{image=display->graph(graph);}while(!image&&!rt.error()&&graph.reduce_font());
    if(image)
    {
        auto pixels=image->pixels();int x=(320-pixels.width())/2,y=40+(210-pixels.height())/2;
        Screen.draw(pixels,x,y,pico_ui::text_colour());
        Screen.draw_background(pixels,x,y,pattern(Settings.Background()));
    }
    else{rt.clear_error();info_label(8,55,"Preview too large; source kept");}
    flush_display();read_key_raw();
}
bool formula_entry(const char *title,char *formula,bool empty=false,bool two_variables=false,bool equation_mode=false,const char *variables=nullptr,const char *independent="X")
{
    pico_ui::ScreenScope scope("desktop.formula_entry.page1");unsigned frame=~0u;
    char original[160];snprintf(original,sizeof original,"%s",formula);
    pico_text_editor editor{formula,160,strlen(formula),strlen(formula)};bool replace=true;char feedback[64]="Type to replace; arrows to edit";
    for(;;)
    {
        bool chrome=content_page(title,pico_ui::Actions{{"Functions","Templates","Check","Preview","Cancel"}},frame);
        info_label(8,32,variables?variables:equation_mode?"Equation or expression (max 159 bytes)":two_variables?"Formula in X and Y (no Z=)":"Formula in X (no Y= or quotes)");
        Screen.fill(3,61,316,61,pattern(Settings.CursorBackground()));Screen.fill(3,217,316,217,pattern(Settings.CursorBackground()));
        size_t row,col;editor.position(editor.cursor,row,col,28);size_t scroll=row>4?row-4:0,r=0,c=0;
        for(size_t n=0;n<editor.length;n=editor.next(n))
        {
            if(r>=scroll&&r<scroll+6)
            {
                char glyph[5]={};memcpy(glyph,formula+n,editor.next(n)-n);
                if(replace)Screen.fill(8+c*10,68+(r-scroll)*23,17+c*10,88+(r-scroll)*23,pattern(35,90,160));
                Screen.text(8+c*10,68+(r-scroll)*23,utf8(glyph),LibMonoFont10x17,replace?pattern(255,255,255):pico_ui::text_colour());
            }
            if(++c==28){++r;c=0;}
        }
        Screen.fill(8+col*10,68+(row-scroll)*23,9+col*10,87+(row-scroll)*23,pattern(Settings.CursorBackground()));
        ui_line(6,228,308,16,feedback,pico_ui::TextRole::status,pico_ui::text_colour());
        ui_line(6,250,308,16,"Tab next slot | F4 Math preview | Enter",pico_ui::TextRole::status,pico_ui::text_colour());
        content_refresh(chrome,frame);int key=read_key_raw();
        if(key==27||key==0xb1||key==0x85){snprintf(formula,160,"%s",original);return false;}
        if(key==10||key==13||key==0x83)
        {
            if(empty&&!editor.length){if(key!=0x83)return true;snprintf(feedback,sizeof feedback,"Empty: curve disabled");continue;}
            {cleaner temporary;expression_g test=equation_mode?pico_workbench_residual(formula):pico_formula_parse(formula);if(!test){snprintf(feedback,sizeof feedback,"%s",rt.error()?cstring(rt.error()):"Invalid formula");rt.clear_error();continue;}}
            if(key!=0x83)return true;snprintf(feedback,sizeof feedback,"Syntax OK | F4 for math preview");continue;
        }
        if(key==0x84){if(editor.length)formula_preview(formula);continue;}
        if(key==0x81||key==0x82)
        {
            const char *before=nullptr,*after="";
            if(key==0x81)
            {
                const char *names[]={"SIN( )","COS( )","TAN( )","LN( )","EXP( )","SQRT( )",independent,"Pi","Back"};
                const char *prefix[]={"SIN(","COS(","TAN(","LN(","EXP(","SQRT(",independent,"π"};
                int action=screen_menu("desktop.formula_entry.choose1","Insert function",names,9);if(action<0||action==8)continue;before=prefix[action];after=action<6?")":"";
            }
            else
            {
                static const char *const names[]={"Fraction ( )/( )","Power ( )^( )","Square ( )^2","Parentheses ( )","Absolute value ABS( )","Differentiate with respect to X","Antiderivative of current","Back"};
                static const char *const prefix[]={"(","(","(","(","ABS("};static const char *const suffix[]={")/()",")^()",")^2",")",")"};
                int action=screen_menu("desktop.formula_entry.choose2","Templates / calculus",names,8);if(action<0||action==7)continue;
                if(action>=5)
                {
                    expression_g transformed=pico_formula_calculus(formula,action==6,independent);char result[160];
                    if(!transformed){engine_error();continue;}
                    if(!pico_formula_source(transformed,result,sizeof result)){if(rt.error())engine_error();else notice("Formula","Result exceeds the 159-byte limit");continue;}
                    snprintf(formula,160,"%s",result);editor.length=editor.cursor=strlen(formula);replace=false;
                    snprintf(feedback,sizeof feedback,"%s | F4 previews",action==5?"Derivative inserted":"Primitive inserted (+ C implicit)");continue;
                }
                before=prefix[action];after=suffix[action];
            }
            if(!pico_formula_insert(editor,before,after,replace))snprintf(feedback,sizeof feedback,"Formula limit: 159 bytes");
            else{replace=false;snprintf(feedback,sizeof feedback,"Type argument; arrows move cursor");}continue;
        }
        if(key==0xb4){editor.cursor=editor.previous(editor.cursor);replace=false;}
        else if(key==0xb7){editor.cursor=editor.next(editor.cursor);replace=false;}
        else if(key==0xb5){editor.vertical(-1,28);replace=false;}
        else if(key==0xb6){editor.vertical(1,28);replace=false;}
        else if(key==9){pico_formula_next_slot(editor);replace=false;}
        else if(key==1)replace=true;
        else if(key==8||key==127){if(replace){formula[0]=0;editor.cursor=editor.length=0;replace=false;}else editor.backspace();}
        else if(key>=32&&key<127)
        {
            char character=key;if(!pico_formula_insert(editor,"","",replace)){continue;}replace=false;
            if(!editor.insert(&character,1))snprintf(feedback,sizeof feedback,"Formula limit: 159 bytes");
        }
    }
}
bool return_to_calculator=false;
object_p parse_source(const char *source)
{
    while(isspace(static_cast<unsigned char>(*source))) ++source;
    size_t length=strlen(source);
    while(length && isspace(static_cast<unsigned char>(source[length-1]))) --length;
    if(!length) { rt.syntax_error();return nullptr; }
    settings::SaveDecimalComma decimal_comma(false);
    program_g parsed=program::parse(utf8(source),length);
    if(!parsed) return nullptr;
    object_g only;
    for(object_p item : *parsed)
    {
        if(item->type()==object::ID_comment) continue;
        if(only) return parsed;
        only=item;
    }
    if(only && only->as_program()) return only;
    return parsed;
}
object::result finish_selected(object::result result)
{
    // Esc leaves a deferred upstream frame; native Run cancellation must not
    // resume it on a later action. Preserve explicit HALT/debug semantics.
    if(program::halted&&key_tail()==KEY_EXIT){object::static_object(object::ID_Kill)->evaluate();key_pop_all();}
    return_to_calculator=true;return result;
}
object::result run_selected(object_p code)
{
    key_pop_all();
    save<bool> no_halt(program::halted,false);
    object::result result=program::run(code,true);
    return finish_selected(result);
}
bool execute(const char *source)
{
    settings::SaveDecimalComma decimal_comma(false);
    program_g commands=program::parse(utf8(source),strlen(source));
    return commands && commands->run()==object::OK;
}
bool read_source(const char *path,char *buffer)
{
    FIL f={};UINT bytes=0;
    if(pico_f_open(&f,path,FA_READ)!=FR_OK) return false;
    bool ok=f_size(&f)<source_capacity;
    if(ok) ok=pico_f_read(&f,buffer,source_capacity-1,&bytes)==FR_OK;
    if(pico_f_close(&f)!=FR_OK) ok=false;
    buffer[bytes]=0;
    return ok && !memchr(buffer,0,bytes) && pico_text_editor::valid_utf8(buffer,bytes);
}
bool save_source(const char *path,const char *source)
{
    char temp[512];
    if(snprintf(temp,sizeof temp,"%s.tmp",path)>=int(sizeof temp)) return false;
    pico_io_reset();FIL f={};UINT bytes=0;size_t count=strlen(source);
    if(pico_f_open(&f,temp,FA_WRITE|FA_CREATE_ALWAYS)!=FR_OK) return false;
    bool ok=pico_f_write(&f,source,count,&bytes)==FR_OK && bytes==count;
    if(pico_f_close(&f)!=FR_OK) ok=false;
    return ok && !pico_io_failed() && pico_commit_state(temp,path);
}
#include "ui/editor.inc"
#include "ui/editor_actions.inc"

unsigned run_chip_file(const char *path,bool editor_context=false);
unsigned show_lua_file(const char *path,bool editor_context=false);
void debug_program(object_p code);
bool source_error(const char *source,pico_text_editor &editor)
{
    uintptr_t at=reinterpret_cast<uintptr_t>(rt.source()),base=reinterpret_cast<uintptr_t>(source);size_t offset=0;unsigned line=0;
    bool exact=at>=base&&at<=base+editor.length;
    if(exact){offset=at-base;line=1;for(size_t n=0;n<offset;++n)if(source[n]=='\n')++line;}
    else if(object_p command=rt.command()){
        // A runtime command has no source map. Only offer a clearly labelled
        // search when its rendered token occurs once outside strings/comments.
        size_t found=0;unsigned matches=0;pico_source_style style;
        for(size_t n=0;n<editor.length;++n){auto kind=style.next(source[n],source[n+1]);if(kind==pico_source_style::string||kind==pico_source_style::comment||pico_text_editor::continuation(static_cast<unsigned char>(source[n])))continue;
            if(n&&(isalnum((unsigned char)source[n-1])||source[n-1]=='_'))continue;
            size_t length=editor.length-n;
            if(::command::lookup(utf8(source+n),length)==command->type()){found=n;++matches;}}
        if(matches==1){offset=found;line=1;for(size_t n=0;n<offset;++n)if(source[n]=='\n')++line;}
    }
    const char *error=rt.error()?cstring(rt.error()):"RPL error";
    bool jump=editor_error("RPL source error",error,line,exact?"Edit at line":"Find");if(jump)editor.cursor=offset;
    rt.clear_error();return jump;
}
bool assemble_file(const char *source,const char *path,pico_text_editor *editor=nullptr)
{
    pico_chip8::assembler compiler;uint8_t bytes[3584];size_t length=0;
    if(!compiler.assemble(source,bytes,sizeof bytes,length))
    {
        char message[80];snprintf(message,sizeof message,"Line %u: %s",compiler.line,compiler.error);
        if(editor){if(editor_error("Assembler error",compiler.error,compiler.line))editor_jump(*editor,compiler.line);}
        else screen_text("desktop.assemble_file.view_text1","Assembler error",message,strlen(message));return false;
    }
    char rom[300],temp[310];snprintf(rom,sizeof rom,"%s",path);
    char *ext=strrchr(rom,'.');if(!ext)return false;strcpy(ext,".ch8");snprintf(temp,sizeof temp,"%s.tmp",rom);
    FIL f={};UINT written=0;pico_io_reset();
    bool ok=pico_f_open(&f,temp,FA_WRITE|FA_CREATE_ALWAYS)==FR_OK;
    if(ok){ok=pico_f_write(&f,bytes,length,&written)==FR_OK && written==length;if(pico_f_close(&f)!=FR_OK)ok=false;}
    if(!ok || pico_io_failed() || !pico_commit_state(temp,rom)){notice("Assemble failed","Previous ROM retained");return false;}
    unsigned pc=run_chip_file(rom,editor!=nullptr);if(editor&&pc){if(pc!=~0u){unsigned line=0;if(compiler.assemble(source,bytes,sizeof bytes,length,pc,&line))editor_jump(*editor,line);}return false;}return true;
}
// True means a saved program ran; browser should return to calculator results.
bool edit_file(const char *path,const char *name,bool is_new,source_language language=source_language::rpl,const char *initial=nullptr,bool example_document=false)
{
    pico_ui::ScreenScope screen_scope("editor.source");
    pico_workspace_lease workspace(pico_workspace_owner::editor,source_capacity);
    char *buffer=static_cast<char *>(workspace.data);
    if(!buffer) { notice("Editor","Not enough editor memory");return false; }
    if(is_new)
    {
        if(initial)snprintf(buffer,source_capacity,"%s",initial);
        else if(language==source_language::rpl)strcpy(buffer,"«\n\n»");
        else if(language==source_language::assembly)strcpy(buffer,"start:\nCLS\nLD V0, 10\nLD V1, 10\nLD I, sprite\nDRW V0, V1, 5\nwait:\nJP wait\nsprite:\nDB 0xF0, 0x90, 0x90, 0x90, 0xF0\n");
        else strcpy(buffer,"print(\"Hello PicoCalc\")\nprint(calc.eval(\"1/3\"))\n");
    }
    else if(!read_source(path,buffer))
    { workspace.release();notice("Cannot edit","Use a UTF-8 source file under 16 KiB");return false; }
    pico_text_editor editor{buffer,source_capacity,strlen(buffer),0,is_new};
    if(is_new && language==source_language::rpl) editor.cursor=3;
    auto *tools=new(std::nothrow)pico_editor_tools(editor,editor_clipboard);
    if(!tools||!tools->ready()){delete tools;tools=nullptr;workspace.release();notice("Editor","Not enough undo memory");return false;}
    char *owned_buffer=nullptr;
    size_t scroll=0;bool ran=false;char find_text[80]="";unsigned editor_frame=~0u;bool shown_dirty=editor.dirty;
    auto resume_editor=[&](size_t cursor,pico_source_backing &backing,bool parked,const char *fallback)
    {
        workspace.acquire(pico_workspace_owner::editor,source_capacity);buffer=static_cast<char *>(workspace.data);
        if(!buffer){notice("Editor","Saved source kept; working buffer unavailable");return false;}
        bool restored=parked&&backing.restore(buffer);
        if(!restored&&fallback){memcpy(buffer,fallback,editor.length+1);restored=true;}
        if(!restored&&!read_source(path,buffer)){notice("Editor","Saved source kept; reopen to edit");return false;}
        editor.text=buffer;editor.capacity=source_capacity;editor.length=strlen(buffer);editor.cursor=std::min(cursor,editor.length);
        while(editor.cursor&&pico_text_editor::continuation(static_cast<unsigned char>(buffer[editor.cursor])))--editor.cursor;
        editor.dirty=false;
        // Keep checked undo records with the identical PSRAM text snapshot.
        // Reloading a potentially changed SD file starts fresh history.
        if(!restored){delete tools;tools=new(std::nothrow)pico_editor_tools(editor,editor_clipboard);}
        if(!tools||!tools->ready()){delete tools;tools=nullptr;notice("Editor","Undo unavailable; saved source kept");return false;}
        return true;
    };
    for (;;)
    {
        bool chrome=editor_frame!=pico_ui::page_generation;render_editor(editor,name,scroll,language,*tools,chrome);
        if(!chrome&&shown_dirty!=editor.dirty){char title[80];snprintf(title,sizeof title,"%s%s",editor.dirty?"* ":"",name);Screen.fill(0,0,319,27,pattern(Settings.HeaderBackground()));ui_line(8,3,304,23,title,pico_ui::TextRole::title,pattern(255,255,255));mark_dirty(0,0,319,27);}
        if(chrome)flush_display();else{mark_dirty(0,28,319,285);lcd_refresh();}
        shown_dirty=editor.dirty;editor_frame=pico_ui::page_generation;int c;
        do{c=read_key_raw();}while(move_editor_cursor(editor,scroll,*tools,c));
        if(c==0x84||c==15)
        {
            static const char *const items[]={"Language reference","Open example","Insert command","Find","Replace","Go to line","Undo","Redo","Select all","Help / file details","Debug"};
            for(;;)
            {
                int selected=screen_menu("desktop.edit_file.choose1","Editor tools",items,language==source_language::rpl?11:10);if(selected<0){c=0;break;}
                if(selected==9){static const char *const help[]={"Shortcuts","File details"};int choice=screen_menu("editor.help_details","Editor help / details",help,2);if(choice<0)continue;c=choice==0?0x90:0x91;}
                else{const int keys[]={0x92,0x93,0x94,6,18,7,26,25,1,0,0x84};c=keys[selected];}
                break;
            }
            if(!c)continue;
        }
        if(c==0x94){editor_insert_command(language,*tools);continue;}
        if(c==0x92){tools->break_group();editor_language_reference(language);continue;}
        if(c==0x93)
        {
            tools->break_group();
            if(example_document){notice("Open example","Close this example to return to your original document before opening another.");continue;}
            const pico_editor_examples::Example *examples[48];const char *titles[48];int count=0;
            for(const auto &e:pico_editor_examples::entries)if(e.language==unsigned(language)&&count<48){examples[count]=&e;titles[count++]=e.title;}
            int choice=screen_menu("editor.examples","Open example",titles,count,nullptr,language==source_language::rpl?"examples.rpl":language==source_language::lua?"examples.lua":"examples.chip8");
            if(choice<0)continue;const auto &example=*examples[choice];
            char description[1024];snprintf(description,sizeof description,"%s\n\n%s\n\nF1 opens a separate, unsaved document. Your current source stays intact.%s",example.source,example.expected,language==source_language::assembly?" Choose SUPER-CHIP modern in the CHIP-8 file list before running these examples.":"");
            if(screen_text("editor.example_preview",example.title,description,strlen(description),pico_ui::TextRole::prose,{{"Open",nullptr,nullptr,nullptr,"Back"}})!=1)continue;
            if(!sys_disk_ok()){notice("Open example","Insert an SD card so the new document can be saved. Your current source is unchanged.");continue;}
            char example_name[40]="";if(!prompt("Example file name",example_name,sizeof example_name,true))continue;
            char example_path[128];const char *directory=language==source_language::rpl?"data":language==source_language::lua?"lua":"chip8";
            snprintf(example_path,sizeof example_path,"%s/%s.%s",directory,example_name,language==source_language::rpl?"48s":language==source_language::lua?"lua":"asm");
            if(file_exists(example_path)){notice("Already exists","Choose another name. The existing file and your source are unchanged.");continue;}
            // Only one nested example is allowed. Snapshot only the used source,
            // bounded by source_capacity; never sacrifice the current document.
            char *snapshot=static_cast<char *>(malloc(editor.length+1));
            if(!snapshot){notice("Open example","Not enough memory to keep your source safely. Close another document or try a smaller file.");continue;}
            memcpy(snapshot,buffer,editor.length+1);char clipboard[sizeof editor_clipboard];memcpy(clipboard,editor_clipboard,sizeof clipboard);
            bool previous_return=return_to_calculator;workspace.release();
            edit_file(example_path,example_name,true,language,example.source,true);
            return_to_calculator=previous_return;memcpy(editor_clipboard,clipboard,sizeof clipboard);
            // If the shared workspace cannot be reacquired, the checked heap
            // snapshot itself remains editable; it is freed when this editor closes.
            if(workspace.acquire(pico_workspace_owner::editor,source_capacity))
            {buffer=static_cast<char *>(workspace.data);memcpy(buffer,snapshot,editor.length+1);editor.capacity=source_capacity;free(snapshot);}
            else{free(owned_buffer);owned_buffer=snapshot;buffer=snapshot;editor.capacity=editor.length+1;}
            editor.text=buffer;continue;
        }
        if(c==0x91){screen_text("files.details","File details",path,strlen(path));continue;}
        if(c<32||c>=127)tools->break_group();
        if(!tools->ready()&&c!=0x81&&c!=19&&c!=0x82&&c!=0x84&&c!=0x85&&c!=27&&c!=0xb1)
        {notice("Editor storage error","Save the source, then close and reopen");continue;}
        if((c==27||c==0xb1)&&tools->anchor!=pico_editor_tools::none){tools->anchor=pico_editor_tools::none;continue;}
        if(c==0x85 || c==27 || c==0xb1)
        {
            if(!editor.dirty)break;
            int action=unsaved_source();
            if(action==2)break;
            if(action==1){if(save_source(path,buffer)){editor.dirty=false;break;}notice("Save failed","Source remains open; previous saved file retained");}
        }
        else if(c==0x81 || c==19 || c==0x82 || (c==0x84 && language==source_language::rpl))
        {
            object_g parsed;
            if(language==source_language::rpl)
            { parsed=parse_source(buffer);if(!parsed){if(source_error(buffer,editor))tools->anchor=pico_editor_tools::none;continue;} }
            if(!save_source(path,buffer)) { notice("Save failed","Previous file retained");continue; }
            editor.dirty=false;
            if(c==0x84) { delete tools;tools=nullptr;workspace.release();debug_program(object_p(parsed));ran=true;break; }
            if(c==0x82)
            {
                if(language==source_language::assembly){if(assemble_file(buffer,path,&editor)){ran=true;break;}continue;}
                size_t cursor=editor.cursor;pico_source_backing backing;bool parked=backing.store(buffer);
                char *copy=parked?nullptr:static_cast<char *>(malloc(editor.length+1));
                if(!parked&&!copy){notice("Cannot run","Not enough memory to retain source and undo safely. Your source remains open.");continue;}
                if(copy)memcpy(copy,buffer,editor.length+1);
                workspace.release();buffer=nullptr;
                if(language==source_language::lua)
                {
                    unsigned line=show_lua_file(path,true);
                    bool resumed=resume_editor(cursor,backing,parked,copy);free(copy);if(!resumed)break;
                    if(line){tools->anchor=pico_editor_tools::none;editor_jump(editor,line);}
                    continue;
                }
                if(run_selected(object_p(parsed))==object::OK){free(copy);ran=true;break;}
                return_to_calculator=false;
                bool resumed=resume_editor(cursor,backing,parked,copy);free(copy);if(!resumed){rt.clear_error();break;}
                if(source_error(buffer,editor))tools->anchor=pico_editor_tools::none;
                continue;
            }
        }
        else if(c==8 || c==127){if(!tools->backspace())notice("Editor","Selection exceeds 256-byte edit limit");}
        else if(c==0xb4)editor.cursor=editor.previous(editor.cursor);
        else if(c==0xb7)editor.cursor=editor.next(editor.cursor);
        else if(c==0xb5)editor.vertical(-1,source_layout::columns);
        else if(c==0xb6)editor.vertical(1,source_layout::columns);
        else if(c==10 || c==13){if(!tools->newline())notice("Editor full","Source capacity reached");}
        else if(c==9){if(!tools->insert("  ",2))notice("Editor full","Source capacity reached");}
        else if(c==2)tools->insert("«",2);
        else if(c==5)tools->insert("»",2);
        else if(c==11)tools->anchor=tools->anchor==pico_editor_tools::none?editor.cursor:pico_editor_tools::none;
        else if(c==1){tools->anchor=0;editor.cursor=editor.length;}
        else if(c==3||c==24){if(!tools->copy(c==24))notice("Clipboard","Copy/cut is limited to 256 bytes");}
        else if(c==22){if(!tools->insert(editor_clipboard,strlen(editor_clipboard)))notice("Paste failed","Source full or selection too large");}
        else if(c==26)tools->undo();
        else if(c==25)tools->redo();
        else if(c==21)tools->unindent();
        else if(c==6)
        {if(prompt("Find text",find_text,sizeof find_text)&&!tools->find(find_text))notice("Find","Text not found");}
        else if(c==18)
        {
            char replacement[80]="";
            if(prompt("Find text",find_text,sizeof find_text)&&prompt("Replace with",replacement,sizeof replacement,false,true))
            {
                static const char *const choices[]={"Replace next","Replace all","Cancel"};int choice=screen_menu("desktop.edit_file.choose2","Replace",choices,3);
                if(choice==0){if(tools->find(find_text))tools->insert(replacement,strlen(replacement));else notice("Replace","Text not found");}
                else if(choice==1){int n=tools->replace_all(find_text,replacement);if(n<0)notice("Replace failed","Replacement exceeds source capacity");}
            }
        }
        else if(c==7)
        {
            char number[16]="1";if(prompt("Go to line",number,sizeof number)){char *end;unsigned long line=strtoul(number,&end,10);if(!*end&&line){editor.cursor=0;while(--line&&editor.cursor<editor.length){while(editor.cursor<editor.length&&editor.text[editor.cursor]!='\n')editor.cursor=editor.next(editor.cursor);if(editor.cursor<editor.length)++editor.cursor;}tools->anchor=pico_editor_tools::none;}}
        }
        else if(c==0x90)
        {screen_text("desktop.edit_file.view_text1","Editor shortcuts",pico_ui_help::tool_text("desktop.edit_file.view_text1"),strlen(pico_ui_help::tool_text("desktop.edit_file.view_text1")));}
        else if(c>=32 && c<127)
        {char ch=char(c);if(!tools->insert(&ch,1,true))notice("Editor full","Source full or selection exceeds 256 bytes");}
    }
    delete tools;workspace.release();free(owned_buffer);key_pop_all();return ran;
}
bool supported(const char *name)
{
    const char *ext=strrchr(name,'.');
    return ext && (!strcasecmp(ext,".48s")||!strcasecmp(ext,".48b")||!strcasecmp(ext,".txt"));
}
bool file_visible(const FILINFO &info)
{return *info.fname&&info.fname[0]!='.'&&!(info.fattrib&(AM_DIR|AM_HID|AM_SYS));}
int program_page(int offset,char names[8][256])
{
    DIR directory;FILINFO info;int count=0;
    if(pico_f_opendir(&directory,"data")!=FR_OK) return -1;
    FRESULT result;
    while((result=f_readdir(&directory,&info))==FR_OK && info.fname[0])
    {
        if(!file_visible(info) || !supported(info.fname)) continue;
        if(count>=offset && count<offset+8) snprintf(names[count-offset],256,"%s",info.fname);
        ++count;
    }
    f_closedir(&directory);return result==FR_OK?count:-1;
}
void run_file(const char *path)
{
    const char *ext=strrchr(path,'.');object_g code;
    if(ext && !strcasecmp(ext,".48b"))
    {
        files_g disk=files::make("");text_g name=text::make(path);
        if(disk && name) code=disk->recall_binary(name);
    }
    else
    {
        pico_workspace_lease workspace(pico_workspace_owner::source,source_capacity);
        char *buffer=static_cast<char *>(workspace.data);
        if(!buffer) { notice("Programs","Not enough source memory");return; }
        if(read_source(path,buffer)) code=parse_source(buffer);
        else notice("Cannot run","Source must be UTF-8 and under 16 KiB");
        workspace.release();
    }
    if(code) { if(run_selected(object_p(code))!=object::OK) engine_error(); }
    else if(rt.error()) engine_error();
}
void apply_theme(int theme)
{
    const auto &palette=pico_themes::get(theme);
    settings defaults;
    Settings.Background(defaults.Background());
    Settings.Foreground(defaults.Foreground());
    Settings.StackBackground(defaults.StackBackground());
    Settings.StackForeground(defaults.StackForeground());
    Settings.StackLevelBackground(defaults.StackLevelBackground());
    Settings.StackLevelForeground(defaults.StackLevelForeground());
    Settings.StackLineForeground(defaults.StackLineForeground());
    Settings.ResultBackground(defaults.ResultBackground());
    Settings.ResultForeground(defaults.ResultForeground());
    Settings.EditorBackground(defaults.EditorBackground());
    Settings.EditorForeground(defaults.EditorForeground());
    Settings.EditorLineForeground(defaults.EditorLineForeground());
    Settings.ErrorBackground(defaults.ErrorBackground());
    Settings.ErrorForeground(defaults.ErrorForeground());
    Settings.PlotErrorBackground(defaults.PlotErrorBackground());
    Settings.SelectionBackground(defaults.SelectionBackground());
    Settings.SelectionForeground(defaults.SelectionForeground());
    Settings.CursorSelBackground(defaults.CursorSelBackground());
    Settings.CursorBackground(defaults.CursorBackground());
    Settings.CursorForeground(defaults.CursorForeground());
    Settings.CursorAlphaBackground(defaults.CursorAlphaBackground());
    Settings.CursorAlphaForeground(defaults.CursorAlphaForeground());
    Settings.SearchBackground(defaults.SearchBackground());
    Settings.SearchForeground(defaults.SearchForeground());
    Settings.CommandBackground(defaults.CommandBackground());
    Settings.CommandForeground(defaults.CommandForeground());
    Settings.UserCommandBackground(defaults.UserCommandBackground());
    Settings.UserCommandForeground(defaults.UserCommandForeground());
    Settings.MenuBackground(defaults.MenuBackground());
    Settings.RoundMenuBackground(defaults.RoundMenuBackground());
    Settings.RoundMenuForeground(defaults.RoundMenuForeground());
    Settings.SquareMenuBackground(defaults.SquareMenuBackground());
    Settings.SquareMenuForeground(defaults.SquareMenuForeground());
    Settings.SelectedMenuForeground(defaults.SelectedMenuForeground());
    Settings.SkippedMenuBackground(defaults.SkippedMenuBackground());
    Settings.FolderCornerForeground(defaults.FolderCornerForeground());
    Settings.UnimplementedForeground(defaults.UnimplementedForeground());
    Settings.HeaderBackground(defaults.HeaderBackground());
    Settings.HeaderForeground(defaults.HeaderForeground());
    Settings.DateForeground(defaults.DateForeground());
    Settings.TimeForeground(defaults.TimeForeground());
    Settings.StateNameForeground(defaults.StateNameForeground());
    Settings.RunningIconForeground(defaults.RunningIconForeground());
    Settings.SteppingIconForeground(defaults.SteppingIconForeground());
    Settings.HaltedIconForeground(defaults.HaltedIconForeground());
    Settings.GCIconForeground(defaults.GCIconForeground());
    Settings.LeftShiftBackground(defaults.LeftShiftBackground());
    Settings.LeftShiftForeground(defaults.LeftShiftForeground());
    Settings.RightShiftBackground(defaults.RightShiftBackground());
    Settings.RightShiftForeground(defaults.RightShiftForeground());
    Settings.AlphaForeground(defaults.AlphaForeground());
    Settings.LowerAlphaForeground(defaults.LowerAlphaForeground());
    Settings.BatteryForeground(defaults.BatteryForeground());
    Settings.BatteryBackground(defaults.BatteryBackground());
    Settings.VoltageForeground(defaults.VoltageForeground());
    Settings.BatteryLevelForeground(defaults.BatteryLevelForeground());
    Settings.HalfBatteryForeground(defaults.HalfBatteryForeground());
    Settings.LowBatteryForeground(defaults.LowBatteryForeground());
    Settings.ChargingForeground(defaults.ChargingForeground());

    pattern bg=pico_themes::colour(palette.background);
    pattern fg=pico_ui::readable(pico_themes::colour(palette.foreground),bg);
    Settings.Background(bg.bits);
    Settings.Foreground(fg.bits);
    Settings.StackBackground(bg.bits);
    Settings.StackForeground(fg.bits);
    Settings.StackLevelBackground(bg.bits);
    Settings.StackLevelForeground(fg.bits);
    Settings.StackLineForeground(fg.bits);
    Settings.ResultBackground(bg.bits);
    Settings.ResultForeground(fg.bits);
    Settings.EditorBackground(bg.bits);
    Settings.EditorForeground(fg.bits);
    Settings.EditorLineForeground(fg.bits);
    Settings.ErrorBackground(bg.bits);
    Settings.ErrorForeground(fg.bits);
    Settings.PlotErrorBackground(bg.bits);
    Settings.SelectionBackground(bg.bits);
    Settings.SelectionForeground(fg.bits);
    Settings.CursorSelBackground(bg.bits);
    Settings.CursorBackground(bg.bits);
    Settings.CursorForeground(fg.bits);
    Settings.CursorAlphaBackground(bg.bits);
    Settings.CursorAlphaForeground(fg.bits);
    Settings.SearchBackground(bg.bits);
    Settings.SearchForeground(fg.bits);
    Settings.CommandBackground(bg.bits);
    Settings.CommandForeground(fg.bits);
    Settings.UserCommandBackground(bg.bits);
    Settings.UserCommandForeground(fg.bits);
    Settings.MenuBackground(bg.bits);
    Settings.RoundMenuBackground(bg.bits);
    Settings.RoundMenuForeground(fg.bits);
    Settings.SquareMenuBackground(bg.bits);
    Settings.SquareMenuForeground(fg.bits);
    Settings.SelectedMenuForeground(fg.bits);
    Settings.SkippedMenuBackground(bg.bits);
    Settings.FolderCornerForeground(fg.bits);
    Settings.UnimplementedForeground(fg.bits);
    Settings.HeaderBackground(bg.bits);
    Settings.HeaderForeground(fg.bits);
    Settings.DateForeground(fg.bits);
    Settings.TimeForeground(fg.bits);
    Settings.StateNameForeground(fg.bits);
    Settings.RunningIconForeground(fg.bits);
    Settings.SteppingIconForeground(fg.bits);
    Settings.HaltedIconForeground(fg.bits);
    Settings.GCIconForeground(fg.bits);
    Settings.LeftShiftBackground(bg.bits);
    Settings.LeftShiftForeground(fg.bits);
    Settings.RightShiftBackground(bg.bits);
    Settings.RightShiftForeground(fg.bits);
    Settings.AlphaForeground(fg.bits);
    Settings.LowerAlphaForeground(fg.bits);
    Settings.BatteryForeground(fg.bits);
    Settings.BatteryBackground(bg.bits);
    Settings.VoltageForeground(fg.bits);
    Settings.BatteryLevelForeground(fg.bits);
    Settings.HalfBatteryForeground(fg.bits);
    Settings.LowBatteryForeground(fg.bits);
    Settings.ChargingForeground(fg.bits);
    pattern header=pico_themes::colour(palette.header),selection=pico_themes::colour(palette.selection);
    pattern accent=pico_ui::readable(pico_themes::colour(palette.accent),bg);
    Settings.HeaderBackground(header.bits);
    auto header_ink=pico_ui::readable(pattern::white,header).bits;
    Settings.HeaderForeground(header_ink);
    Settings.DateForeground(header_ink);Settings.TimeForeground(header_ink);
    Settings.StateNameForeground(header_ink);Settings.BatteryForeground(header_ink);
    Settings.VoltageForeground(header_ink);Settings.BatteryLevelForeground(header_ink);
    Settings.HalfBatteryForeground(header_ink);Settings.LowBatteryForeground(header_ink);
    Settings.ChargingForeground(header_ink);Settings.RunningIconForeground(header_ink);
    Settings.SteppingIconForeground(header_ink);Settings.HaltedIconForeground(header_ink);
    Settings.GCIconForeground(header_ink);
    Settings.SelectionBackground(selection.bits);
    Settings.SelectionForeground(pico_ui::readable(pattern::white,selection).bits);
    Settings.CursorBackground(accent.bits);
    Settings.CursorForeground(pico_ui::readable(bg,accent).bits);
    Settings.ResultForeground(pico_ui::readable(pico_themes::colour(palette.result),bg).bits);
    Settings.ErrorBackground(pattern(90,20,20).bits);
    Settings.ErrorForeground(pico_ui::readable(pattern(255,230,210),pattern(90,20,20)).bits);

}
} // namespace

void pico_load_startup_session()
{
    {pico_timezone zone;if(sys_disk_ok()&&pico_timezone_load(zone))pico_set_timezone(zone);}
#ifdef CONFIG_COLOR
    bool has_colour=file_exists("state/Session-colour.48S") || file_exists("state/Session-colour.48S.bak") || file_exists("state/Session-colour.48S.rollback");
    if(has_colour) load_system_state();
    else
    {
        bool old=file_exists("state/Session.48S") || file_exists("state/Session.48S.bak") || file_exists("state/Session.48S.rollback");
        if(old)
        {
            set_reset_state_file("state/Session.48S");
            bool loaded=load_system_state();
            set_reset_state_file("state/Session-colour.48S");
            if(!loaded)
            {
                rt.reset();Settings=settings();
                notice("Session import failed","Original monochrome files retained");
            }
        }
        apply_theme(0);
    }
#else
    load_system_state();
#endif
}

static void file_browser()
{
    return_to_calculator=false;
    if(rt.editing()) { notice("Programs","Finish or cancel the calculator entry");return; }
    if(!sys_disk_ok()) { notice("Programs","Insert an SD card and restart");return; }
    pico_f_mkdir("data");int &selected=pico_ui::focus("files.rpl").selected;
    for (;;)
    {
        char names[8][256]={};int offset=selected/8*8,total=program_page(offset,names);
        if(total<0) { notice("Programs","Could not read program directory");return; }
        if(selected>=total && total) { selected=total-1;continue; }
        screen_page("desktop.pico_load_startup_session.page1","Programs: /db48x/data",pico_ui::Actions{{total?"Open":nullptr,total?"Run":nullptr,"New",total?"Actions":nullptr,"Back"}});
        for(int n=0;n<8 && n+offset<total;++n) file_row_label(n,names[n],n+offset==selected);
        list_page_hint(selected,total);
        if(!total) info_label(8,70,"No programs yet. Press F3.");
        flush_display();int key=read_key_raw();file_row_key(key,selected,total);
        if(key==0x85 || key==27 || key==0xb1) break;
        if(key==0xb5 && total) selected=(selected+total-1)%total;
        else if(key==0xb6 && total) selected=(selected+1)%total;
        else if(key==0xb4 && total) selected=selected>=8?selected-8:0;
        else if(key==0xb7 && total) selected=selected+8<total?selected+8:total-1;
        else if(key==0x84&&total){const char *name=names[selected-offset];screen_text("files.details","File details",name,strlen(name));}
        else if(key==0x83)
        {
            char name[40]="";if(!prompt("New program name",name,sizeof name,true)) continue;
            char path[96];snprintf(path,sizeof path,"data/%s.48s",name);
            if(file_exists(path)) { notice("Already exists","Select the existing file and Edit");continue; }
            if(edit_file(path,name,true)) return;
        }
        else if(total && (key==0x81 || key==10 || key==13 || key==0x82))
        {
            const char *name=names[selected-offset];char path[280];snprintf(path,sizeof path,"data/%s",name);
            if(key!=0x82)
            {
                if(!strcasecmp(strrchr(name,'.'),".48b")){int action=screen_text("files.binary_rpl","Binary RPL program",name,strlen(name),pico_ui::TextRole::prose,pico_ui::Actions{{nullptr,"Run",nullptr,nullptr,"Back"}});if(action==2){run_file(path);return;}}
                else if(edit_file(path,name,false)) return;
            }
            else { run_file(path);return; }
        }
    }
    key_pop_all();
}

namespace {
struct home_page
{
    int offset,total=0;
    char labels[8][128]={};
    object_g names[8],programs[8];
};
bool collect_program(object_p name,object_p value,void *context)
{
    if(!value->as_program()) return true;
    home_page &p=*static_cast<home_page *>(context);
    int index=p.total++-p.offset;
    if(index>=0 && index<8)
    {
        p.names[index]=name;p.programs[index]=value;
        name->render(p.labels[index],sizeof p.labels[index]);
        p.labels[index][sizeof p.labels[index]-1]=0;
    }
    return true;
}
void home_browser()
{
    int &selected=pico_ui::focus("programs.home").selected;
    for (;;)
    {
        home_page p{selected/8*8};rt.homedir()->enumerate(collect_program,&p);
        if(p.total&&selected>=p.total){selected=p.total-1;continue;}
        screen_page("desktop.home_browser.page1","HOME programs",pico_ui::Actions{{p.total?"Open":nullptr,p.total?"Run":nullptr,p.total?"Export":nullptr,p.total?"Debug":nullptr,"Back"}});
        for(int n=0;n<8 && n+p.offset<p.total;++n) file_row_label(n,p.labels[n],n+p.offset==selected);
        list_page_hint(selected,p.total);
        if(!p.total) info_label(8,70,"No HOME programs. Use STO to save one.");
        flush_display();int key=read_key_raw();file_row_key(key,selected,p.total);
        if(key==0x85 || key==27 || key==0xb1) return;
        if(key==0xb5 && p.total) selected=(selected+p.total-1)%p.total;
        else if(key==0xb6 && p.total) selected=(selected+1)%p.total;
        else if(p.total && key==0x82)
        {
            if(run_selected(object_p(p.programs[selected-p.offset]))!=object::OK) engine_error();
            return;
        }
        else if(p.total&&(key==0x81||key==10||key==13))
        {
            text_g text=p.programs[selected-p.offset]->as_text(true);if(!text){engine_error();continue;}
            size_t n;utf8 bytes=text->value(&n);char source[2048];n=std::min(n,sizeof source-1);while(n&&!pico_text_editor::valid_utf8(reinterpret_cast<const char *>(bytes),n))--n;memcpy(source,bytes,n);source[n]=0;
            int action=screen_text("desktop.home_browser.view_text1","HOME program",source,n,pico_ui::TextRole::code,pico_ui::Actions{{nullptr,"Run",nullptr,nullptr,"Back"}});
            if(action==2){if(run_selected(object_p(p.programs[selected-p.offset]))!=object::OK)engine_error();return;}
        }
        else if(key==0x84 && p.total) { debug_program(object_p(p.programs[selected-p.offset]));return; }
        else if(key==0x83 && p.total)
        {
            if(!sys_disk_ok()) { notice("Export","SD card unavailable");continue; }
            char name[40]="";if(!prompt("Export filename",name,sizeof name,true)) continue;
            char path[96];snprintf(path,sizeof path,"data/%s.48s",name);pico_f_mkdir("data");
            if(file_exists(path) && !confirm("Replace existing source file?")) continue;
            text_g source=p.programs[selected-p.offset]->as_text(true);
            if(!source) { engine_error();continue; }
            size_t length;utf8 bytes=source->value(&length);
            if(length>=source_capacity) { notice("Export","Program is larger than 16 KiB");continue; }
            char *buffer=static_cast<char *>(malloc(length+1));
            if(!buffer) { notice("Export","Not enough memory");continue; }
            memcpy(buffer,bytes,length);buffer[length]=0;
            bool ok=save_source(path,buffer);free(buffer);
            notice(ok?"Exported":"Export failed",ok?"Open SD programs to edit the copy":"Previous file retained");
        }
    }
}
} // namespace

namespace {
void render_object_line(int y,const char *prefix,object_p object,bool selected=false,pico_ui::TextRole role=pico_ui::TextRole::status)
{
    char text[128],value[96]={};object_g rooted=object;
    if(rooted)rooted->render(value,sizeof value);value[sizeof value-1]=0;
    snprintf(text,sizeof text,"%s%s",prefix,value);if(role==pico_ui::TextRole::menu)label(8,y,text,selected);else ui_line(8,y,304,16,text,role,pico_ui::text_colour());
}
struct variable_page {
    int offset,total=0;object_g names[8],values[8];
};
bool collect_variable(object_p name,object_p value,void *context)
{
    auto &p=*static_cast<variable_page *>(context);int n=p.total++-p.offset;
    if(n>=0&&n<8){p.names[n]=name;p.values[n]=value;}return true;
}
int view_text(const char *title,const char *body,size_t length,pico_ui::TextRole role,pico_ui::Actions actions);
void variable_browser()
{
    int &selected=pico_ui::focus("variables.home").selected;
    for(;;)
    {
        int offset=selected/8*8;variable_page p{offset};rt.homedir()->enumerate(collect_variable,&p);
        if(p.total&&selected>=p.total){selected=p.total-1;continue;}
        screen_page("desktop.variable_browser.page1","HOME variables",pico_ui::Actions{{p.total?"Open":nullptr,nullptr,nullptr,p.total?"Details":nullptr,"Back"}});
        for(int n=0;n<8&&n+offset<p.total;++n)
        {
            char name[48]={};p.names[n]->render(name,sizeof name-3);name[sizeof name-3]=0;strcat(name," = ");
            render_object_line(38+n*28,name,p.values[n],n+offset==selected,pico_ui::TextRole::menu);
        }
        if(!p.total)info_label(8,70,"No HOME variables. Store a value with STO.");
        list_page_hint(selected,p.total);flush_display();int k=read_key_raw();file_row_key(k,selected,p.total);
        if(k==27||k==0xb1||k==0x85)break;
        if(k==0xb5 && p.total)selected=(selected+p.total-1)%p.total;
        else if(k==0xb6 && p.total)selected=(selected+1)%p.total;
        else if(k==0xb7 && selected+8<p.total)selected+=8;
        else if(k==0xb4 && selected>=8)selected-=8;
        else if((k==10||k==13||k==0x81||k==0x84)&&p.total)
        {
            char text[2048]={};size_t n=p.values[selected-offset]->render(text,sizeof text);
            text[n<sizeof text?n:sizeof text-1]=0;screen_text("desktop.variable_browser.view_text1","Variable value",text,strlen(text),pico_ui::TextRole::table);
        }
    }
    key_pop_all();
}
void debug_program(object_p code)
{
    if(rt.run_stepping()){notice("Debugger","Finish the current debug session first");return;}
    if(!rt.push(code)||object::static_object(object::ID_Debug)->evaluate()!=object::OK){engine_error();return;}
    for(;;)
    {
        screen_page("desktop.debug_program.page1","RPL debugger",pico_ui::Actions{{"Step","Over","Run","Vars","Stop"}});
        object_g next=rt.run_stepping();render_object_line(38,next?"Next: ":"Finished: ",next);
        info_label(8,68,"Stack (top first):");
        for(unsigned n=0;n<6 && n<rt.depth();++n)
        { char prefix[12];snprintf(prefix,sizeof prefix,"%u: ",n+1);render_object_line(98+n*28,prefix,rt.stack(n)); }
        flush_display();int k=read_key_raw();
        if(k==27||k==0xb1||k==0x85){object::static_object(object::ID_Kill)->evaluate();break;}
        if(k==0x84){variable_browser();continue;}
        if(next && k>=0x81 && k<=0x83)
        {
            object::id action=k==0x81?object::ID_SingleStep:k==0x82?object::ID_StepOver:object::ID_Continue;
            if(object::static_object(action)->evaluate()!=object::OK)engine_error();
        }
    }
    ui.show_graphics(false);key_pop_all();
}
pico_chip8::profile chip_profile=pico_chip8::schip;
int chip_cycles=20;
bool save_bytes(const char *path,const void *data,size_t length)
{return pico_resource_save(path,data,length);}
bool read_bytes(const char *path,void *data,size_t capacity,size_t &length)
{return pico_resource_read(path,data,capacity,length);}
void chip_display(const pico_chip8::machine &vm,bool paused)
{
    screen_page("desktop.chip_display.page1",paused?"CHIP-8 paused":"CHIP-8 / SUPER-CHIP",pico_ui::Actions{{"Pause","Restart","Speed",nullptr,"Back"}});
    for(int y=0;y<64;++y)for(int x=0;x<128;++x)
        Screen.fill(32+x*2,45+y*2,33+x*2,46+y*2,vm.pixel(x,y)?pattern(100,240,150):pattern(0,0,0));
    info_label(10,185,"Keypad: 1234 / QWER / ASDF / ZXCV");
    char info[96];snprintf(info,sizeof info,"%s  %d cycles/frame",vm.mode==pico_chip8::vip?"CHIP-8":vm.mode==pico_chip8::chip48?"CHIP-48":"SUPER-CHIP",chip_cycles);info_label(10,215,info);
    flush_display();
}
unsigned run_chip_file(const char *path,bool editor_context)
{
    uint8_t *rom=static_cast<uint8_t *>(malloc(3584));
    auto *vm=new(std::nothrow) pico_chip8::machine;
    if(!rom||!vm){free(rom);delete vm;notice("CHIP-8","Not enough memory");return editor_context?~0u:0;}
    size_t length=0;if(!read_bytes(path,rom,3584,length)||!length){free(rom);delete vm;notice("CHIP-8","ROM must be 1..3584 bytes");return editor_context?~0u:0;}
    char config[320],flags[320];snprintf(config,sizeof config,"%s.cfg",path);snprintf(flags,sizeof flags,"%s.flags",path);
    char saved=0;size_t count=0;auto profile=chip_profile;
    if(read_bytes(config,&saved,1,count)&&count==1&&saved>='0'&&saved<='3')profile=pico_chip8::profile(saved-'0');
    vm->load(rom,length,profile);read_bytes(flags,vm->flags,sizeof vm->flags,count);
    vm->random_state=sys_current_ms()|1;bool paused=false,buzzer=false;uint16_t keys=0;
    pico_chip8::frame_clock clock{to_us_since_boot(get_absolute_time())};pico_keyboard_reset();key_pop_all();
    for(;;)
    {
        int state=0,key=pico_keyboard_event(&state);
        if(key>=0)
        {
            if(state==1 && (key==27||key==0xb1||key==0x85))break;
            int index=pico_chip8::key_index(key);
            if(index>=0){if(state==1||state==2)keys|=1u<<index;else if(state==3)keys&=~(1u<<index);vm->set_keys(keys);}
            if(state==1 && key==0x81){paused=!paused;keys=0;vm->set_keys(0);vm->dirty=true;}
            if(state==1 && key==0x82){uint8_t kept[8];memcpy(kept,vm->flags,8);vm->load(rom,length,profile);memcpy(vm->flags,kept,8);keys=0;}
            if(state==1 && key==0x83){chip_cycles=chip_cycles==7?20:chip_cycles==20?50:chip_cycles==50?100:7;vm->dirty=true;}
        }
        uint64_t now=to_us_since_boot(get_absolute_time());
        unsigned frames=clock.due(now);
        if(frames)
        {
            if(!paused)for(unsigned f=0;f<frames;++f){vm->tick();for(int i=0;i<chip_cycles;++i)if(!vm->step())break;}
            bool sounding=!paused&&vm->sound;
            if(sounding!=buzzer){if(sounding)start_buzzer_freq(880000);else stop_buzzer();buzzer=sounding;}
            if(vm->dirty){chip_display(*vm,paused);vm->dirty=false;}
        }
        if(vm->error || vm->exited)break;
        sleep_ms(1);
    }
    stop_buzzer();pico_keyboard_reset();key_pop_all();
    unsigned stopped=0;if(vm->error){stopped=~0u;char error[100];snprintf(error,sizeof error,"PC %03X: %s",vm->last_pc,vm->error);if(editor_context){if(screen_text("editor.error","CHIP-8 stopped",error,strlen(error),pico_ui::TextRole::prose,{{"Edit source",nullptr,nullptr,nullptr,"Back"}})==1)stopped=vm->last_pc;}else screen_text("desktop.run_chip_file.view_text1","CHIP-8 stopped",error,strlen(error));}
    if(profile>=pico_chip8::schip && !save_bytes(flags,vm->flags,8))notice("Flag save failed","Previous flags retained");
    free(rom);delete vm;return stopped;
}
#include "ui/lua_output.inc"

void lua_result_view(const pico_lua_result &result,bool ok)
{
    const char *title=ok?"Lua finished":"Lua stopped";
    if(result.graphics)
    {
        pico_ui::ScreenScope scope("lua.completion");pico_ui::seen(title);key_pop_all();action_bar({{"Output",nullptr,nullptr,nullptr,"Back"}});flush_display();for(;;){int key=read_key_raw();if(key==0x81||key==13||key==10)break;if(key==0x85||key==27||key==0xb1)return;}
    }
    if(result.length || !result.graphics)lua_output_view(title,result.output,result.length);
}
void lua_console()
{
    pico_lua_result result;pico_lua_session *session=pico_lua_open(&result);
    if(!session){lua_output_view("Lua console",result.output,strlen(result.output));return;}
    char chunk[4096]="",previous[512]="";size_t used=0;
    for(;;)
    {
        char line[512]="";
        if(!prompt(used?"... continue Lua":"Lua > (Up recalls last line)",line,sizeof line,false,true,previous))
        {if(used){used=0;chunk[0]=0;continue;}break;}
        if(!used && !strcmp(line,":quit"))break;
        if(!used && !strcmp(line,":help"))
        {const char *help=pico_ui_help::tool_text("desktop.lua_console.view_text1");screen_text("desktop.lua_console.view_text1","Lua console help",help,strlen(help));continue;}
        if(!*line&&!used)continue;
        if(used+strlen(line)+2>=sizeof chunk){notice("Console","Input chunk exceeds 4095 bytes");used=0;continue;}
        snprintf(previous,sizeof previous,"%s",line);memcpy(chunk+used,line,strlen(line));used+=strlen(line);chunk[used++]='\n';chunk[used]=0;
        int status=pico_lua_submit(session,chunk,used,1,&result);
        if(status==2)continue;
        if(result.length||result.graphics||status!=1)lua_result_view(result,status==1);used=0;chunk[0]=0;
    }
    pico_lua_close(session);key_pop_all();
}
struct lua_file_source
{
    FIL file{};char buffer[512];bool open=false;
    ~lua_file_source(){if(open)pico_f_close(&file);}
    static const char *read(void *opaque,size_t *size)
    {
        auto &source=*static_cast<lua_file_source *>(opaque);UINT n=0;
        if(!source.open){*size=0;return nullptr;}
        if(pico_f_read(&source.file,source.buffer,sizeof source.buffer,&n)!=FR_OK){*size=SIZE_MAX;return nullptr;}
        if(!n){FRESULT result=pico_f_close(&source.file);source.open=false;*size=result==FR_OK?0:SIZE_MAX;return nullptr;}
        *size=n;return source.buffer;
    }
};
unsigned show_lua_file(const char *path,bool editor_context)
{
    lua_file_source source;source.open=pico_f_open(&source.file,path,FA_READ)==FR_OK;
    if(!source.open||f_size(&source.file)>=source_capacity){notice("Lua source","Use a saved script under 16 KiB");return 0;}
    pico_lua_result result;key_pop_all();pico_keyboard_reset();
    screen_page("desktop.show_lua_file.page1","Lua running",pico_ui::Actions{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});info_label(8,55,"Running script...");flush_display();
    bool ok=pico_lua_run_stream(lua_file_source::read,&source,&result);
    if(editor_context&&!ok&&!result.cancelled){return editor_error("Lua error",*result.error?result.error:result.output,result.error_line)?result.error_line:0;}
    lua_result_view(result,ok);return 0;
}
#include "ui/results.inc"
#include "ui/routes.inc"

void development_files(source_language language)
{
    const bool assembly=language==source_language::assembly;
    const char *directory=assembly?"chip8":"lua";
    if(!sys_disk_ok()){notice("Development","Insert an SD card and restart");return;}
    pico_f_mkdir(directory);int &selected=pico_ui::focus(assembly?"files.chip8":"files.lua").selected;
    for(;;)
    {
        char names[8][256]={};int offset=selected/8*8,total=0;DIR d;FILINFO info;
        if(pico_f_opendir(&d,directory)!=FR_OK){notice("Files","Directory unavailable");return;}
        FRESULT status;
        while((status=f_readdir(&d,&info))==FR_OK && info.fname[0])
        {
            const char *ext=strrchr(info.fname,'.');
            if(!file_visible(info) || !ext || (assembly?strcasecmp(ext,".ch8")&&strcasecmp(ext,".asm"):strcasecmp(ext,".lua")))continue;
            if(total>=offset&&total<offset+8)snprintf(names[total-offset],256,"%s",info.fname);++total;
        }
        f_closedir(&d);if(status!=FR_OK){notice("Files","Directory read failed");return;}
        if(total&&selected>=total){selected=total-1;continue;}
        screen_page("desktop.development_files.page1",assembly?"CHIP-8 / assembly":"Lua scripts",pico_ui::Actions{{total?"Open":nullptr,total?"Run":nullptr,"New","Actions","Back"}});
        for(int n=0;n<8&&n+offset<total;++n)file_row_label(n,names[n],n+offset==selected);
        list_page_hint(selected,total);
        if(!total)info_label(8,70,"No files yet. F3 creates one.");flush_display();int k=read_key_raw();file_row_key(k,selected,total);
        if(k==27||k==0xb1||k==0x85)break;
        if(k==0xb5&&total)selected=(selected+total-1)%total;
        else if(k==0xb6&&total)selected=(selected+1)%total;
        else if(k==0xb4&&total)selected=selected>=8?selected-8:0;
        else if(k==0xb7&&total)selected=selected+8<total?selected+8:total-1;
        else if(k==0x83)
        {
            char name[40]="";if(!prompt("New source name",name,sizeof name,true))continue;
            char path[128];snprintf(path,sizeof path,"%s/%s.%s",directory,name,assembly?"asm":"lua");
            if(file_exists(path)){notice("Already exists","Select the file and Edit");continue;}
            edit_file(path,name,true,language);
        }
        else if(k==0x84)
        {
            const char *actions[]={"File details",assembly?"Compatibility profile":"Reload list"};
            int action=screen_menu("desktop.development_files.actions","File actions",actions,2);
            if(action==0&&total){const char *name=names[selected-offset];screen_text("files.details","File details",name,strlen(name));continue;}
            if(action!=1||!assembly)continue;
            static const char *const profiles[]={"CHIP-8 (VIP)","CHIP-48 (HP48)","SUPER-CHIP modern","SUPER-CHIP HP48","Back"};int choice=screen_menu("desktop.development_files.choose1","Compatibility profile",profiles,5);
            if(choice>=0&&choice<4)
            {
                chip_profile=pico_chip8::profile(choice);
                if(total){char path[320];snprintf(path,sizeof path,"chip8/%s",names[selected-offset]);char *ext=strrchr(path,'.');strcpy(ext,".ch8.cfg");char v=char('0'+choice);if(!save_bytes(path,&v,1))notice("Profile save failed","Default changed for this launch");}
            }
        }
        else if(total&&(k==0x81||k==0x82||k==10||k==13))
        {
            const char *name=names[selected-offset];char path[300];snprintf(path,sizeof path,"%s/%s",directory,name);
            bool binary=assembly&&!strcasecmp(strrchr(name,'.'),".ch8");
            if(k!=0x82){if(binary){int action=screen_text("files.binary_chip8","CHIP-8 ROM",name,strlen(name),pico_ui::TextRole::prose,pico_ui::Actions{{nullptr,"Run",nullptr,nullptr,"Back"}});if(action==2)run_chip_file(path);}else edit_file(path,name,false,language);}
            else if(binary)run_chip_file(path);
            else
            {
                pico_workspace_lease workspace(pico_workspace_owner::source,source_capacity);
                char *source=static_cast<char *>(workspace.data);
                if(!source){notice("Source","Not enough memory");continue;}
                if(read_source(path,source)){if(assembly)assemble_file(source,path);else {workspace.release();source=nullptr;show_lua_file(path);}}else notice("Source","Use UTF-8 source under 16 KiB");workspace.release();
            }
        }
    }
    key_pop_all();
}

} // namespace


#include "sprite_ui.inc"

void pico_program_browser()
{
    return_to_calculator=false;
    if(rt.editing()) { notice("Programs","Finish or cancel the calculator entry");return; }
    for(;;)
    {
    static const char *const items[]={"HOME programs","RPL files / editor","CHIP-8 / assembly","Lua scripts","Sprite editor","HOME variables","Lua console","Back"};
    int selected=screen_menu("desktop.pico_program_browser.choose1","Programming",items,8);
    if(selected<0||selected==7)break;
    if(selected==0)home_browser();
    else if(selected==1)file_browser();
    else if(selected==2)development_files(source_language::assembly);
    else if(selected==3)development_files(source_language::lua);
    else if(selected==4)sprite_editor();
    else if(selected==5)variable_browser();
    else if(selected==6)lua_console();
    if(return_to_calculator)break;
    }
    key_pop_all();
}

namespace {
uint32_t angle_number(){return Settings.AngleMode()==object::ID_Deg?0:Settings.AngleMode()==object::ID_Rad?1:Settings.AngleMode()==object::ID_Grad?2:3;}
void set_scene_angle(unsigned n){Settings.AngleMode(n==0?object::ID_Deg:n==1?object::ID_Rad:n==2?object::ID_Grad:object::ID_PiRadians);}
struct plot_source {
    pico_plot_sample sample;
    void *context;
    pico_workbench *work=nullptr;
    const pico_data_table *scatter=nullptr;
    pico_plot_engine::path_function path_sample=nullptr;
    pico_path_scene *path=nullptr;
    pico_ode_context *ode=nullptr;
    unsigned *scatter_selection=nullptr;
    pico_geometry_document *geometry=nullptr;
    const pico_region_fill *fill=nullptr;
    pico_contour_layer *contour=nullptr;
    int value(int curve,double x,double y,double &z) const
    { return sample(context,curve,x,y,&z); }
};
void geometry_coordinates(pico_geometry_document &,unsigned);
void geometry_measurements(const pico_geometry_document &);
void geometry_menu();
void lab_launch(int);
void advanced_launch(int);
void science_launch(int);
void research_launch(int mode=0);
bool science_get_data_units(char *,char *,size_t);
bool sampling_poll(void *opaque,int row)
{
    if(row>=0&&opaque&&*static_cast<int *>(opaque)!=row)
    {
        *static_cast<int *>(opaque)=row;char text[64];snprintf(text,sizeof text,"Sampling row %d / %d",row+1,pico_wireframe::side);
        screen_page("desktop.sampling_poll.page1","3D surface",pico_ui::Actions{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});info_label(8,65,text);flush_display();
    }
    int key=pico_keyboard_poll();return key==27||key==0xb1||key==0x85;
}
bool analysis_number(const char *title,double &value)
{
    char input[48];snprintf(input,sizeof input,"%.17g",value);
    if(!prompt(title,input,sizeof input))return false;
    double number;
    if(!pico_formula_scalar(input,number)||std::fabs(number)>1e100)
    {if(rt.error())engine_error();else notice("Analysis","Use a finite value within +/-1e100");return false;}
    value=number;return true;
}
bool analysis_quantity(const char *title,double &value,const char *units){char input[96];snprintf(input,sizeof input,"%.17g",value);if(!prompt(title,input,sizeof input))return false;pico_workbench_scope scope;if(!scope.entered){engine_error();return false;}double next;if(!pico_quantity_input(input,units,next)){engine_error();return false;}value=next;return true;}

pico_plot_analysis::status analyse_curve(pico_plot_request &scene,const plot_source &source,int selected,double centre,
                                        pico_plot_analysis::result &overlay,bool &shade,double &lower,double &upper,char *heading,size_t capacity,
                                        const pico_plot_explore::selection &bounds,int &shade_other)
{
    using namespace pico_plot_analysis;
    static const char *const items[]={"Roots / zeros","Intersections","Minimum on interval","Maximum on interval","Slope at X (dY/dX)","Definite integral (signed)","Area (absolute)","Area between curves","Back"};
    int action=screen_menu("desktop.analysis_number.choose1","Analyse selected curve",items,9);if(action<0||action==8)return status::no_result;
    const operation operations[]={operation::roots,operation::intersections,operation::minimum,operation::maximum,operation::derivative,operation::integral,operation::area,operation::between};
    int other=selected;
    if(action==1||action==7)
    {
        const char *names[4];char labels[3][80];int indices[3],count=0;
        for(int i=0;i<3;++i)if(scene.active[i]&&i!=selected){char snippet[41];pico_formula_label(snippet,sizeof snippet,scene.label[i]);snprintf(labels[count],80,"Y%d: %s",i+1,snippet);names[count]=labels[count];indices[count++]=i;}
        if(!count){notice("Two curves needed","Enable a second curve first");return status::no_result;}
        names[count]="Back";int choice=screen_menu("desktop.analysis_number.choose2",action==7?"Area with":"Intersect with",names,count+1);if(choice<0||choice==count)return status::no_result;other=indices[choice];
    }
    double a=scene.window[0],b=scene.window[1],at=centre;
    if(action==4){if(!analysis_number("Slope at X =",at))return status::no_result;}
    else
    {
        bool visual=false;
        if(bounds.ready())
        {
            static const char *const choices[]={"Use marked bounds","Type bounds","Cancel"};
            int choice=screen_menu("desktop.analysis_number.choose3","Analysis bounds",choices,3);
            if(choice<0||choice==2)return status::no_result;
            visual=choice==0;
        }
        if(visual){a=bounds.first;b=bounds.last;}
        else if(!analysis_number("Interval: first X",a)||!analysis_number("Interval: last X",b))return status::no_result;
    }
    screen_page("desktop.analysis_number.page1","Analysing curve",pico_ui::Actions{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});flush_display();key_pop_all();
    result calculated=run(operations[action],source.sample,source.context,selected,other,a,b,at,sampling_poll);
    if(calculated.state==status::cancelled||calculated.state==status::callback_error)return calculated.state;
    if(calculated.state!=status::ok){notice("Analysis",message(calculated.state));return calculated.state;}
    char output[2048];size_t used=snprintf(output,sizeof output,"%s\nCurve Y%d",items[action],selected+1);
    if(action==1||action==7)used+=snprintf(output+used,sizeof output-used," and Y%d",other+1);
    used+=snprintf(output+used,sizeof output-used,"\n");
    if(action==4)used+=snprintf(output+used,sizeof output-used,"At X=%.12g\n",at);
    else used+=snprintf(output+used,sizeof output-used,"Interval %.12g .. %.12g\n",a,b);
    if(calculated.count)
        for(int i=0;i<calculated.count&&used<sizeof output-100;++i)used+=snprintf(output+used,sizeof output-used,"\nX=%.12g\nY=%.12g\n",calculated.points[i].x,calculated.points[i].y);
    else used+=snprintf(output+used,sizeof output-used,"\nValue ~= %.12g\nEstimated error %.3g\n",calculated.value,calculated.error);
    used+=snprintf(output+used,sizeof output-used,"\n%d evaluations\nApproximate, bounded numerical analysis.\n%s",calculated.evaluations,calculated.truncated?"First 16 points shown.\n":"");
    if(action==2||action==3)used+=snprintf(output+used,sizeof output-used,"Sampled/refined interval extremum; endpoints included.\n");
    if(action<=1)used+=snprintf(output+used,sizeof output-used,"Narrow/close/tangent roots can be missed.\n");
    int use=screen_text("desktop.analysis_number.view_text1","Analysis result",output,std::min(used,sizeof output-1),pico_ui::TextRole::prose,pico_ui::Actions{{calculated.count?"Copy X":"Copy",calculated.count?"Copy Y":nullptr,nullptr,nullptr,"Back"}});
    if(use>0)
    {
        double ordinal=1;bool copy=true;
        if(calculated.count>1)
        {
            copy=analysis_number("Point number (from result list)",ordinal);
            if(copy&&(ordinal<1||ordinal>calculated.count||ordinal!=std::floor(ordinal)))
            {notice("Analysis","Choose an integer point from the list");copy=false;}
        }
        if(copy)
        {
            double number=calculated.count?(use==1?calculated.points[int(ordinal)-1].x:calculated.points[int(ordinal)-1].y):calculated.value;
            object_g value=hwdouble::make(number);if(!value||!rt.push(value))engine_error();
        }
    }
    overlay=calculated;shade=action==5||action==6||action==7;shade_other=action==7?other:-1;lower=std::fmin(a,b);upper=std::fmax(a,b);
    if(action<=1)snprintf(heading,capacity,"Plot 2D | %d %s",calculated.count,action?"intersections":"roots");
    else if(action==2||action==3)snprintf(heading,capacity,"%s Y=%.8g",action==2?"Min":"Max",calculated.value);
    else snprintf(heading,capacity,"%s ~= %.8g",action==4?"Slope":action==5?"Integral":"Area",calculated.value);
    return status::ok;
}
void plot_label(char *,size_t,const char *);
uint32_t export_rgb(pattern p)
{
#ifdef CONFIG_COLOR
    color c(uint32_t(p.bits));return uint32_t(c.red())<<16|uint32_t(c.green())<<8|c.blue();
#else
    return p.bits?0xffffff:0;
#endif
}
bool export_open(pico_graph_export &file,const char *extension)
{
    if(!sys_disk_ok()){notice("Graph export","SD card unavailable");return false;}
    pico_f_mkdir("exports");char name[49]="GRAPH";
    if(!prompt("Export name (letters/digits/_/-)",name,sizeof name))return false;
    for(const char *p=name;*p;++p)if(!isalnum(static_cast<unsigned char>(*p))&&*p!='_'&&*p!='-'){notice("Export name","Use letters, digits, underscore or dash");return false;}
    if(!*name)return false;char path[96];snprintf(path,sizeof path,"exports/%s.%s",name,extension);
    FILINFO info;if(pico_f_stat(path,&info)==FR_OK&&!confirm("Replace existing export?"))return false;
    if(!file.open(path,[](){return sampling_poll(nullptr,0);})){notice("Export failed","Cannot open SD temporary file");return false;}return true;
}
void export_done(pico_graph_export &file,bool valid=true)
{
    if(!valid)file.failed=true;bool ok=file.commit();notice(ok?"Graph exported":"Export failed",ok?"Saved under /db48x/exports":file.cancelled?"Cancelled; previous file retained":"Previous file retained");key_pop_all();
}
void export_csv(pico_plot_request &scene,const plot_source &source,const double *samples)
{
    pico_graph_export file;if(!export_open(file,"csv"))return;
    file.put(source.path?"t,x,y\r\n":source.ode?"x,y_solution\r\n":"x,y1,y2,y3\r\n");char line[180];
    for(int i=0;i<300&&!file.failed;++i)
    {
        double at=pico_plot_engine::coordinate(source.path?source.path->first:scene.window[0],source.path?source.path->last:scene.window[1],i,299);
        snprintf(line,sizeof line,"%.17g",at);file.put(line);
        int count=source.path?2:source.ode?1:3;
        for(int c=0;c<count;++c){double value=samples[c*300+i];if((source.path||scene.active[c])&&std::isfinite(value))snprintf(line,sizeof line,",%.17g",value);else strcpy(line,",");file.put(line);}file.put("\r\n");
    }
    if(source.scatter){file.put("\r\nscatter_x,scatter_y\r\n");for(unsigned i=0;i<source.scatter->rows;++i){snprintf(line,sizeof line,"%.17g,%.17g\r\n",source.scatter->points[i][0],source.scatter->points[i][1]);file.put(line);}}
    export_done(file);
}
// The table and graph share the same bounded sample array and selected index.
// No evaluations, extra framebuffer or copy of the sample cache is required.
bool linked_plot_table(const pico_plot_request &scene,const plot_source &source,const double *samples,int &index,int &curve,int &scatter)
{
 int count=source.scatter?int(source.scatter->rows):300;
 if(count<=0){notice("Table unavailable","No data points");return false;}
 int row=source.scatter?std::max(0,scatter):index;row=std::max(0,std::min(count-1,row));
 for(;;){screen_page("math.graph.table",source.scatter?"Linked data points":"Linked graph values",{{"Trace",(!source.path&&!source.scatter)?"Curve":nullptr,"Copy XY","Details","Back"}});unsigned first=unsigned(row)/8*8;
 char heading[100];snprintf(heading,sizeof heading,"%s | Y%d | %d of %d",source.path?"T, X, Y":"X, Y",curve+1,row+1,count);ui_line(8,37,304,20,heading,pico_ui::TextRole::status,pico_ui::text_colour());
 auto values=[&](int i,double &x,double &y){x=source.scatter?source.scatter->points[i][0]:source.path?samples[i]:pico_plot_engine::coordinate(scene.window[0],scene.window[1],i,299);y=source.scatter?source.scatter->points[i][1]:samples[(source.path?1:curve)*300+i];};
 for(unsigned i=first;i<unsigned(count)&&i<first+8;++i){double x,y;values(i,x,y);char text[100];if(source.path&&std::isfinite(x)&&std::isfinite(y))snprintf(text,sizeof text,"%sT %.5g X %.6g Y %.6g",i==unsigned(row)?"> ":"",pico_plot_engine::coordinate(source.path->first,source.path->last,i,299),x,y);else if(std::isfinite(x)&&std::isfinite(y))snprintf(text,sizeof text,"%s%u  X %.8g   Y %.8g",i==unsigned(row)?"> ":"",i+1,x,y);else snprintf(text,sizeof text,"%s%u  undefined",i==unsigned(row)?"> ":"",i+1);ui_line(8,65+(i-first)*25,304,23,text,pico_ui::TextRole::status,pico_ui::text_colour());}
 ui_line(8,269,304,16,"Arrows select/page | Enter traces selected row",pico_ui::TextRole::status,pico_ui::text_colour());flush_display();int key=read_key_raw();
 if(key==27||key==0xb1||key==0x85)return false;
 if(key==0xb5)row=std::max(0,row-1);if(key==0xb6)row=std::min(count-1,row+1);if(key==0xb4)row=std::max(0,row-8);if(key==0xb7)row=std::min(count-1,row+8);
 if(key==0x82&&!source.path&&!source.scatter){do{curve=(curve+1)%3;}while(!scene.active[curve]);}
 if(key==0x83){double x,y;values(row,x,y);if(!std::isfinite(x)||!std::isfinite(y)){notice("Point undefined","Choose a finite row");continue;}object_g a=source.geometry?pico_quantity_number(x,source.geometry->units):hwdouble::make(x),b=source.geometry?pico_quantity_number(y,source.geometry->units):hwdouble::make(y);size_t depth=rt.depth();if(!a||!b||!rt.push(a)||!rt.push(b)){if(rt.depth()>depth)rt.drop(rt.depth()-depth);engine_error();}}
 if(key==0x84){double x,y;values(row,x,y);char details[220];if(source.path)snprintf(details,sizeof details,"T: %.17g\nX: %.17g\nY: %.17g",pico_plot_engine::coordinate(source.path->first,source.path->last,row,299),x,y);else snprintf(details,sizeof details,"Row: %d\nX: %.17g\nY: %.17g\n%s",row+1,x,y,source.scatter?"Data point":scene.label[curve]);screen_text("math.graph.point","Point details",details,strlen(details));}
 if(key==13||key==10||key==0x81){if(source.scatter)scatter=row;else index=row;return true;}
 }
}
int plot_curves(pico_plot_request &scene,const plot_source &source)
{
    bool any=false;for(int i=0;i<3;++i)if(scene.active[i])any=true;
    if(!any){notice("Curves","Enter at least one formula");return false;}
    pico_workspace_lease sample_workspace(pico_workspace_owner::plot,3*300*sizeof(double));
    double *samples=static_cast<double *>(sample_workspace.data);
    if(!samples){notice("Curves","Not enough sample memory");return false;}
    pico_sample_cache cache;
    int scatter_index=source.scatter_selection?int(*source.scatter_selection):-1;
    int contour_y=100;int cursor=150,selected=0;bool geometry_move=false;bool rebuild=true,pan=false;int status=1;pico_graph_export *exporting=nullptr;
    pico_plot_analysis::result overlay;bool shade=false;int analysed_curve=0,shade_other=-1;pico_plot_explore::selection bounds;
    bool tangent_on=false,tangent_dirty=true,tangent_valid=false,adjusting=false,vertical=false;int parameter=0;double slope=0;
    double lower=0,upper=0;char heading[80]="Plot 2D",shown_heading[80]="";unsigned plot_frame=~0u;bool shown_pan=false,shown_adjusting=false,shown_geometry_move=false;
    if(source.contour)snprintf(heading,sizeof heading,"Implicit / contours");
    if(source.geometry)snprintf(heading,sizeof heading,"Interactive geometry");
    if(source.fill)snprintf(heading,sizeof heading,"%s",source.fill->density?"Probability":"Inequality intersection");
    if(source.ode)snprintf(heading,sizeof heading,"Differential equation");
    if(source.path)snprintf(heading,sizeof heading,"%s plot",source.path->polar?"Polar":"Parametric");
    while(!scene.active[selected])selected=(selected+1)%3;
    for(;;)
    {
        if(!sample_workspace.data)
        {
            if(!sample_workspace.acquire(pico_workspace_owner::plot,3*300*sizeof(double))){notice("Curves","Working buffer busy");status=-1;break;}
            samples=static_cast<double *>(sample_workspace.data);
            if(!rebuild&&!cache.restore(samples)){notice("Curves","PSRAM cache failed; resampling");rebuild=true;}
        }
        if(rebuild)
        {
            screen_page("desktop.plot_curves.page1","Sampling curves",pico_ui::Actions{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});flush_display();
            int result=source.contour?source.contour->rebuild(scene.window,samples,[](void *){return sampling_poll(nullptr,0);}):source.ode?source.ode->rebuild(scene.window,samples):source.path?pico_plot_engine::sample_path(source.path->first,source.path->last,source.path_sample,source.context,samples,sampling_poll):pico_plot_engine::sample_curves(scene,source.sample,source.context,samples,sampling_poll);
            if(result<0){status=result;break;}rebuild=false;
        }
        if(source.geometry){auto &doc=*source.geometry;auto *points=const_cast<pico_data_table *>(source.scatter);for(unsigned i=0;i<6;++i)memcpy(points->points[i],doc.points[i],16);if(scatter_index<0)scatter_index=doc.selected;doc.selected=uint8_t(scatter_index);}
        double at=source.path?pico_plot_engine::coordinate(source.path->first,source.path->last,cursor,299):pico_plot_engine::coordinate(scene.window[0],scene.window[1],cursor,299);
        double x=source.path?samples[cursor]:at,y=source.path?samples[300+cursor]:samples[selected*300+cursor];
        if(source.contour)y=pico_plot_engine::coordinate(scene.window[2],scene.window[3],contour_y,200);
        if(source.scatter&&scatter_index>=0&&unsigned(scatter_index)<source.scatter->rows){x=source.scatter->points[scatter_index][0];y=source.scatter->points[scatter_index][1];}
        if(tangent_on&&tangent_dirty)
        {
            auto derivative=pico_plot_analysis::run(pico_plot_analysis::operation::derivative,source.sample,source.context,selected,selected,0,0,at,sampling_poll);
            if(derivative.state==pico_plot_analysis::status::callback_error){status=-1;break;}
            if(derivative.state==pico_plot_analysis::status::cancelled){status=-2;break;}
            tangent_valid=derivative.state==pico_plot_analysis::status::ok;slope=derivative.value;vertical=false;
            if(source.path&&tangent_valid)
            {
                auto dy=pico_plot_analysis::run(pico_plot_analysis::operation::derivative,source.sample,source.context,1,1,0,0,at,sampling_poll);
                if(dy.state==pico_plot_analysis::status::callback_error){status=-1;break;}
                if(dy.state==pico_plot_analysis::status::cancelled){status=-2;break;}
                tangent_valid=dy.state==pico_plot_analysis::status::ok&&std::isfinite(x)&&std::isfinite(y);
                if(tangent_valid)
                {
                    auto direction=pico_path_scene::direction(slope,dy.value,slope);
                    vertical=direction==pico_path_scene::tangent::vertical;tangent_valid=direction!=pico_path_scene::tangent::unavailable;
                }
            }
            if(source.ode){int code=source.ode->slope(x,y,slope);if(code<0){status=code;break;}tangent_valid=code==1&&std::isfinite(y);}
            tangent_dirty=false;
        }
        bool chrome=plot_frame!=pico_ui::page_generation||strcmp(shown_heading,heading)||shown_pan!=pan||shown_adjusting!=adjusting||shown_geometry_move!=geometry_move;
        if(chrome)screen_page("desktop.plot_curves.page2",heading,pico_ui::Actions{{nullptr,nullptr,nullptr,nullptr,"Back"}});else Screen.fill(0,28,319,288,pattern(Settings.Background()));
        if(exporting)exporting->svg.begin(heading,export_rgb(pattern(Settings.Background())),export_rgb(pico_ui::text_colour()));
        auto plot_line=[&](int x1,int y1,int x2,int y2,int width,pattern colour){Screen.line(x1,y1,x2,y2,width,colour);if(exporting)exporting->svg.line(x1,y1,x2,y2,width,export_rgb(colour));};
        rect previous=Screen.clip();Screen.clip(rect(10,35,309,236));
        if(source.fill)for(int i=0;i<300;i+=2){double values[3]={samples[i],samples[300+i],samples[600+i]};double x=pico_plot_engine::coordinate(scene.window[0],scene.window[1],i,299);
            for(int j=0;j<202;j+=2){double y=pico_plot_engine::coordinate(scene.window[2],scene.window[3],j,201);if(source.fill->contains(x,y,values))plot_line(10+i,236-j,11+i,236-j,1,pico_ui::plot_ink(pattern(75,170,145),pattern(Settings.Background())));}}
        if(shade)
        {
            double zero=pico_wireframe::normalize(0,scene.window[2],scene.window[3]);
            int base=int(135.5-std::fmax(-1.0,std::fmin(1.0,double(zero)))*100.5);
            for(int i=0;i<300;i+=2)
            {
                double x=pico_plot_engine::coordinate(scene.window[0],scene.window[1],i,299);
                double y=pico_wireframe::normalize(samples[analysed_curve*300+i],scene.window[2],scene.window[3]);
                double second=shade_other<0?zero:pico_wireframe::normalize(samples[shade_other*300+i],scene.window[2],scene.window[3]);
                if(x>=lower&&x<=upper&&std::isfinite(y)&&std::isfinite(second))
                {base=int(135.5-std::fmax(-1.0,std::fmin(1.0,second))*100.5);int py=int(135.5-std::fmax(-4.0,std::fmin(4.0,y))*100.5);plot_line(10+i,base,10+i,py,1,pattern(65,125,145));}
            }
        }
        for(int i=0;i<=10;++i)plot_line(10+i*30,35,10+i*30,236,1,pattern(110,125,140));
        for(int i=0;i<=8;++i)plot_line(10,35+i*26,309,35+i*26,1,pattern(110,125,140));
        double nx=pico_wireframe::normalize(0,scene.window[0],scene.window[1]);
        double ny=pico_wireframe::normalize(0,scene.window[2],scene.window[3]);
        if(std::isfinite(nx)&&std::fabs(nx)<=1)plot_line(int(159.5+nx*149.5),35,int(159.5+nx*149.5),236,2,pico_ui::text_colour());
        if(std::isfinite(ny)&&std::fabs(ny)<=1)plot_line(10,int(135.5-ny*100.5),309,int(135.5-ny*100.5),2,pico_ui::text_colour());
        if(source.ode&&(source.ode->scene.flags&1))for(int j=0;j<15;++j)for(int i=0;i<21;++i)
        {
            double slope=samples[300+j*21+i];if(!std::isfinite(slope))continue;
            double angle=pico_ode::field_angle(slope,scene.window[1]-scene.window[0],scene.window[3]-scene.window[2]);
            int px=10+int((i+.5)*300/21),py=236-int((j+.5)*202/15),dx=int(5*cos(angle)),dy=int(5*sin(angle));
            plot_line(px-dx,py+dy,px+dx,py-dy,1,pattern(100,170,140));
        }
        if(source.geometry)
        {
            auto &doc=*source.geometry;
            auto segment=[&](double x1,double y1,double x2,double y2,pattern ink){double a=pico_wireframe::normalize(x1,scene.window[0],scene.window[1]),b=pico_wireframe::normalize(y1,scene.window[2],scene.window[3]),c=pico_wireframe::normalize(x2,scene.window[0],scene.window[1]),d=pico_wireframe::normalize(y2,scene.window[2],scene.window[3]);if(!std::isfinite(a)||!std::isfinite(b)||!std::isfinite(c)||!std::isfinite(d))return;plot_line(int(159.5+149.5*fmax(-4.,fmin(4.,a))),int(135.5-100.5*fmax(-4.,fmin(4.,b))),int(159.5+149.5*fmax(-4.,fmin(4.,c))),int(135.5-100.5*fmax(-4.,fmin(4.,d))),2,ink);};
            for(unsigned line=0;line<2;++line)if(doc.flags&(1u<<line)){double ends[2][2];if(pico_geometry::clip_line(doc.points[line*2],doc.points[line*2+1],scene.window,ends))segment(ends[0][0],ends[0][1],ends[1][0],ends[1][1],pico_ui::plot_ink(line?pattern(225,100,170):pattern(40,155,225),pattern(Settings.Background())));}
            if(doc.flags&4){double radius=pico_geometry::distance(doc.points[4],doc.points[5]);for(unsigned i=0;i<96;++i){double a=i*6.283185307179586/96,b=(i+1)*6.283185307179586/96;segment(doc.points[4][0]+radius*cos(a),doc.points[4][1]+radius*sin(a),doc.points[4][0]+radius*cos(b),doc.points[4][1]+radius*sin(b),pico_ui::plot_ink(pattern(70,185,105),pattern(Settings.Background())));}}
            for(unsigned i=0;i<6;++i){double a=pico_wireframe::normalize(doc.points[i][0],scene.window[0],scene.window[1]),b=pico_wireframe::normalize(doc.points[i][1],scene.window[2],scene.window[3]);if(fabs(a)>1||fabs(b)>1)continue;int px=int(159.5+149.5*a),py=int(135.5-100.5*b);plot_line(px-3,py,px+3,py,2,pico_ui::text_colour());plot_line(px,py-3,px,py+3,2,pico_ui::text_colour());char label[]={char('A'+i),0};ui_line(std::max(10,std::min(291,px+5)),std::max(35,std::min(219,py-15)),18,16,label,pico_ui::TextRole::plot_label,pico_ui::text_colour());if(exporting)exporting->svg.text(px+5,py-4,label,export_rgb(pico_ui::text_colour()));}
        }
        if(source.contour)for(unsigned i=0;i<source.contour->used;++i){pico_contour_segment v;if(!source.contour->segments.read(i,v)){status=-1;break;}unsigned c=v.level%3;pattern ink=pico_ui::plot_ink(pattern((scene.colour[c]>>16)&255,(scene.colour[c]>>8)&255,scene.colour[c]&255),pattern(Settings.Background()));plot_line(int(159.5+v.x1*149.5),int(135.5-v.y1*100.5),int(159.5+v.x2*149.5),int(135.5-v.y2*100.5),c==unsigned(selected)?2:1,ink);}
        if(source.contour&&!source.contour->segments.good()){status=-1;break;}
        for(int c=0;c<3;++c)if(!source.contour&&scene.active[c]&&(!source.ode||(source.ode->scene.flags&2)))
        {
            pattern curve_ink=pico_ui::plot_ink(pattern((scene.colour[c]>>16)&255,(scene.colour[c]>>8)&255,scene.colour[c]&255),pattern(Settings.Background()));
            bool have=false;int last_y=0,last_x=0;
            for(int i=0;i<300;++i)
            {
                double value=source.path?samples[300+i]:samples[c*300+i],n=pico_wireframe::normalize(value,scene.window[2],scene.window[3]);
                double nx=source.path?pico_wireframe::normalize(samples[i],scene.window[0],scene.window[1]):2.0*i/299-1;
                if(!std::isfinite(n)||!std::isfinite(nx)||std::fabs(n)>4||std::fabs(nx)>4){have=false;continue;}
                int py=int(135.5-n*100.5),px=source.path?int(159.5+nx*149.5):10+i;
                if(have&&(!source.fill||abs(source.fill->relation[c])!=2||(i/4)%2==0)&&std::abs(py-last_y)<202&&std::abs(px-last_x)<300)plot_line(last_x,last_y,px,py,1,curve_ink);
                last_x=px;last_y=py;have=true;
            }
        }
        if(source.scatter)for(unsigned i=0;i<source.scatter->rows;++i)
        {
            double px=pico_wireframe::normalize(source.scatter->points[i][0],scene.window[0],scene.window[1]);
            double py=pico_wireframe::normalize(source.scatter->points[i][1],scene.window[2],scene.window[3]);
            if(std::isfinite(px)&&std::isfinite(py)&&std::fabs(px)<=1&&std::fabs(py)<=1){int x=int(159.5+px*149.5),y=int(135.5-py*100.5);plot_line(x-3,y,x+3,y,2,pattern(240,120,30));plot_line(x,y-3,x,y+3,2,pattern(240,120,30));}
        }
        for(int i=0;i<overlay.count;++i)
        {
            double x=pico_wireframe::normalize(overlay.points[i].x,scene.window[0],scene.window[1]);
            double y=pico_wireframe::normalize(overlay.points[i].y,scene.window[2],scene.window[3]);
            if(std::isfinite(x)&&std::isfinite(y)&&std::fabs(x)<=1&&std::fabs(y)<=1)
            {int px=int(159.5+x*149.5),py=int(135.5-y*100.5);plot_line(px-4,py,px+4,py,2,pattern(240,170,35));plot_line(px,py-4,px,py+4,2,pattern(240,170,35));}
        }
        if(tangent_on&&tangent_valid&&vertical)
        {
            double n=pico_wireframe::normalize(x,scene.window[0],scene.window[1]);
            if(std::isfinite(n)&&std::fabs(n)<=1){int px=int(159.5+n*149.5);plot_line(px,35,px,236,3,pattern(235,110,180));}
        }
        if(tangent_on&&tangent_valid&&!vertical)
        {
            bool have=false;int previous_y=0;
            for(int i=0;i<300;++i)
            {
                double value,n;
                if(!pico_plot_explore::tangent(pico_plot_engine::coordinate(scene.window[0],scene.window[1],i,299),x,y,slope,value)||!std::isfinite(n=pico_wireframe::normalize(value,scene.window[2],scene.window[3]))||std::fabs(n)>4){have=false;continue;}
                int py=int(135.5-n*100.5);if(have)plot_line(9+i,previous_y,10+i,py,3,pattern(235,110,180));previous_y=py;have=true;
            }
        }
        for(int i=0;i<bounds.count;++i)
        {
            double n=pico_wireframe::normalize(i?bounds.last:bounds.first,scene.window[0],scene.window[1]);
            if(std::isfinite(n)&&std::fabs(n)<=1){int px=int(159.5+n*149.5);plot_line(px,35,px,236,2,pattern(80,200,160));}
        }
        double cursor_x=pico_wireframe::normalize(x,scene.window[0],scene.window[1]);
        int trace_x=(source.path||scatter_index>=0)&&std::isfinite(cursor_x)&&std::fabs(cursor_x)<=1?int(159.5+cursor_x*149.5):(source.path||scatter_index>=0)?-1:10+cursor;
        if(trace_x>=10)plot_line(trace_x,35,trace_x,236,1,pattern(230,160,30));
        double cursor_y=pico_wireframe::normalize(y,scene.window[2],scene.window[3]);
        if(std::isfinite(cursor_y)&&std::fabs(cursor_y)<=1){int py=int(135.5-cursor_y*100.5);plot_line(10,py,309,py,1,pattern(230,160,30));if(trace_x>=10)plot_line(trace_x-4,py,trace_x+4,py,3,pattern(240,170,35));}
        Screen.clip(previous);if(exporting)exporting->svg.end_plot();
        
        char info[160],formula_label[43];plot_label(formula_label,source.path?27:sizeof formula_label,scene.label[selected]);if(source.path)snprintf(info,sizeof info,"T=%.8g | %s",at,formula_label);else if(source.contour)snprintf(info,sizeof info,"F(X,Y): %s",formula_label);else snprintf(info,sizeof info,"Y%d: %s",selected+1,formula_label);ui_line(5,pico_native_layout::graph_formula,310,16,info,pico_ui::TextRole::status,pattern((scene.colour[selected]>>16)&255,(scene.colour[selected]>>8)&255,scene.colour[selected]&255));
        if(std::isfinite(x)&&std::isfinite(y))snprintf(info,sizeof info,"X=%.8g Y=%.8g",x,y);else snprintf(info,sizeof info,"X=%.8g Y=undefined",x);
        ui_line(5,pico_native_layout::graph_values,310,16,info,pico_ui::TextRole::status,pico_ui::text_colour());
        if(source.contour)snprintf(info,sizeof info,"Arrows probe XY | F2 level | +/- zoom");
        else if(source.geometry)snprintf(info,sizeof info,"Point %c | %s",'A'+scatter_index,geometry_move?"Arrows move | Enter edit | Esc trace":"Arrows select | F3 geometry | F4 pan");
        else if(adjusting){const auto &p=source.work->parameters[parameter];snprintf(info,sizeof info,"%c=%.7g step %.5g Enter edit Esc trace",'A'+parameter,p.value,p.step);}
        else if(tangent_on){if(tangent_valid&&vertical)snprintf(info,sizeof info,"Vertical tangent | F4 Pan +/- Zoom");else if(tangent_valid)snprintf(info,sizeof info,"dY/dX=%.7g | F4 Pan +/- Zoom",slope);else snprintf(info,sizeof info,"%s",source.path?"Tangent unavailable | F3 Explore":"Tangent unavailable Enter: mark bound");}
        else if(source.ode)snprintf(info,sizeof info,"F2 solution info | F3 layers / export");
        else if(source.path)snprintf(info,sizeof info,"Left/Right trace T | F4 pan +/- zoom");
        else if(bounds.count==1)snprintf(info,sizeof info,"A=%.7g  Enter: mark B",bounds.first);
        else if(bounds.count==2)snprintf(info,sizeof info,"A=%.7g B=%.7g F2: analyse",bounds.first,bounds.last);
        else snprintf(info,sizeof info,"Enter: mark A  F4: pan  +/-: zoom");
        Screen.fill(0,pico_native_layout::graph_footer_top,319,288,pattern(Settings.HeaderBackground()));ui_line(5,pico_native_layout::graph_status,310,13,info,pico_ui::TextRole::status,pattern(255,255,255));if(chrome){if(source.contour)action_bar({{"Copy XY","Level","Export",pan?"Probe":"Pan","Back"}});else if(adjusting||geometry_move)action_bar({{nullptr,nullptr,nullptr,nullptr,"Trace"}});else action_bar({{"Table",source.geometry?"Measure":source.ode?"Info":source.path?"T range":"Analyse","Explore",pan?"Trace":"Pan","Back"}});}
        if(exporting){char caption[180];snprintf(caption,sizeof caption,"X=%.8g Y=%.8g",x,y);exporting->svg.text(5,265,scene.label[selected],export_rgb(pico_ui::readable(pico_themes::colour(scene.colour[selected]),pattern(Settings.Background()))));exporting->svg.text(5,285,caption,export_rgb(pico_ui::text_colour()));bool ok=exporting->svg.finish();export_done(*exporting,ok);delete exporting;exporting=nullptr;continue;}
        bool parked=!source.contour&&cache.store(samples);if(parked)sample_workspace.release();
        if(chrome)flush_display();else{mark_dirty(0,28,319,288);lcd_refresh();}plot_frame=pico_ui::page_generation;shown_pan=pan;shown_adjusting=adjusting;shown_geometry_move=geometry_move;snprintf(shown_heading,sizeof shown_heading,"%s",heading);int key=read_key_raw();
        if(parked)
        {
            if(!sample_workspace.acquire(pico_workspace_owner::plot,3*300*sizeof(double))){status=-1;break;}
            samples=static_cast<double *>(sample_workspace.data);
            if(!cache.restore(samples)){notice("Curves","PSRAM cache failed; resampling");rebuild=true;continue;}
        }
        if(source.geometry&&geometry_move)
        {
            auto &doc=*source.geometry;unsigned point=doc.selected;
            if(key==27||key==0xb1||key==0x85){geometry_move=false;continue;}
            if(key==13||key==10){geometry_coordinates(doc,point);continue;}
            double next[2]={doc.points[point][0],doc.points[point][1]};
            if(key==0xb4)next[0]-=(scene.window[1]-scene.window[0])/100;if(key==0xb7)next[0]+=(scene.window[1]-scene.window[0])/100;if(key==0xb5)next[1]+=(scene.window[3]-scene.window[2])/100;if(key==0xb6)next[1]-=(scene.window[3]-scene.window[2])/100;
            if(std::isfinite(next[0])&&std::isfinite(next[1])&&fabs(next[0])<=1e50&&fabs(next[1])<=1e50&&memcmp(next,doc.points[point],16)){memcpy(doc.points[point],next,16);doc.dirty=true;}
            continue;
        }
        if(adjusting)
        {
            if(key==27||key==0xb1||key==0x85){adjusting=false;continue;}
            if(key==0xb5||key==0xb6){parameter=(parameter+(key==0xb5?2:1))%3;continue;}
            if(key==0xb4||key==0xb7){source.work->adjust(parameter,key==0xb4?-1:1);rebuild=tangent_dirty=true;overlay.count=0;shade=false;snprintf(heading,sizeof heading,"Plot 2D");continue;}
            if(key==10||key==13)
            {
                double value=source.work->parameters[parameter].value;
                if(analysis_number("Parameter value",value))
                {
                    auto &p=source.work->parameters[parameter];
                    if(value<p.minimum||value>p.maximum)notice("Parameter","Value must lie within configured bounds");
                    else{p.value=value;rebuild=tangent_dirty=true;overlay.count=0;shade=false;snprintf(heading,sizeof heading,"Plot 2D");}
                }
            }
            continue;
        }
        if(key==27||key==0xb1||key==0x85)break;
        if(source.contour){
            if(key==0x81){size_t depth=rt.depth();object_g ax=hwdouble::make(x),ay=hwdouble::make(y);if(!ax||!ay||!rt.push(ax)||!rt.push(ay)){if(rt.depth()>depth)rt.drop(rt.depth()-depth);engine_error();}continue;}
            if(key==0x82){char labels[3][40];const char *items[3];for(unsigned i=0;i<source.contour->count;++i){snprintf(labels[i],40,"Level %.10g",source.contour->levels[i]);items[i]=labels[i];}int n=screen_menu("research.contour_levels","Active contour level",items,source.contour->count);if(n>=0)selected=n;continue;}
            if(key==0x83){auto *file=new(std::nothrow) pico_graph_export;if(!file){notice("Export","Not enough memory");continue;}if(export_open(*file,"svg"))exporting=file;else delete file;continue;}
            if(key==13||key==10){double f;char text[180];if(source.contour->field(source.contour->context,x,y,f))snprintf(text,sizeof text,"X: %.12g\nY: %.12g\nField: %.12g\nSelected level: %.12g\nResidual: %.8g\n\nThe crosshair probes the field; it does not snap to a contour.",x,y,f,source.contour->levels[selected],f-source.contour->levels[selected]);else snprintf(text,sizeof text,"The field is undefined at this probe point.");screen_text("research.contour_probe","Field at cursor",text,strlen(text));continue;}
            if(!pan&&(key==0xb4||key==0xb7||key==0xb5||key==0xb6)){if(key==0xb4)cursor=std::max(0,cursor-3);if(key==0xb7)cursor=std::min(299,cursor+3);if(key==0xb5)contour_y=std::min(200,contour_y+2);if(key==0xb6)contour_y=std::max(0,contour_y-2);continue;}
        }
        if(key==0x81){int next=cursor,curve=selected,point=scatter_index;if(linked_plot_table(scene,source,samples,next,curve,point)){cursor=next;selected=curve;scatter_index=point;if(source.scatter_selection&&point>=0)*source.scatter_selection=unsigned(point);if(source.scatter&&point>=0){double position=(source.scatter->points[point][0]-scene.window[0])/(scene.window[1]-scene.window[0])*299;cursor=int(std::fmax(0,std::fmin(299,std::round(position))));tangent_on=false;}tangent_dirty=true;pan=false;}continue;}
        if(key==0x82&&source.geometry){geometry_measurements(*source.geometry);continue;}
        if(key==0x82&&source.ode){char message[200];auto &r=source.ode->info;snprintf(message,sizeof message,"%s\n%d evaluations, %d accepted steps\nLocal tolerance 1e-7; approximate solution.\nDomain gaps or budget limits may stop either branch.",r.partial?"Partial numerical solution":"Numerical solution",r.evaluations,r.accepted);screen_text("desktop.plot_curves.view_text1","ODE solution",message,strlen(message));continue;}
        if(key==0x82&&source.path)
        {
            double first=source.path->first,last=source.path->last;
            if(analysis_number("First T",first)&&analysis_number("Last T",last))
            {
                if(first>=last)notice("Parameter range","First T must be less than last T");
                else{source.path->first=first;source.path->last=last;rebuild=tangent_dirty=true;}
            }
            continue;
        }
        if(key==0x82)
        {
            auto result=analyse_curve(scene,source,selected,x,overlay,shade,lower,upper,heading,sizeof heading,bounds,shade_other);if(result==pico_plot_analysis::status::ok)analysed_curve=selected;
            if(result==pico_plot_analysis::status::cancelled){status=-2;break;}
            if(result==pico_plot_analysis::status::callback_error){status=-1;break;}
            continue;
        }
        if(key==0x83)
        {
            static const char *const choices[]={"Explore / controls","Export graph SVG","Export samples CSV","Back"};int choice=screen_menu("desktop.plot_curves.choose1","Graph actions",choices,4);
            if(choice==1){auto *file=new(std::nothrow) pico_graph_export;if(!file){notice("Export","Not enough memory");continue;}if(export_open(*file,"svg"))exporting=file;else delete file;continue;}
            if(choice==2){export_csv(scene,source,samples);continue;}if(choice!=0)continue;
        }
        if(key==0x83&&source.geometry)
        {
            const char *items[]={"Move selected point","Edit selected coordinates","Distances / intersections"};int n=screen_menu("math.geometry.explore","Geometry controls",items,3);
            if(n==0){geometry_move=true;pan=false;}if(n==1)geometry_coordinates(*source.geometry,source.geometry->selected);if(n==2)geometry_measurements(*source.geometry);continue;
        }
        if(key==0x83&&source.ode)
        {
            auto &model=source.ode->scene;const char *options[]={tangent_on?"Hide tangent":"Show tangent","Initial condition",(model.flags&1)?"Hide direction field":"Show direction field",(model.flags&2)?"Hide solution":"Show solution","Back"};int action=screen_menu("desktop.plot_curves.choose2","ODE controls",options,5);
            if(action==0){tangent_on=!tangent_on;tangent_dirty=true;}
            if(action==1){double x0=model.x0,y0=model.y0;if(analysis_number("Initial X",x0)&&analysis_number("Initial Y",y0)){model.x0=x0;model.y0=y0;rebuild=tangent_dirty=true;}}
            if(action==2||action==3){unsigned mask=action==2?1:2;if((model.flags^mask)==0)notice("ODE layers","Keep a field or solution visible");else{model.flags^=mask;rebuild=tangent_dirty=true;}}
            continue;
        }
        if(key==0x83&&source.path)
        {
            const char *options[]={tangent_on?"Hide tangent":"Show tangent","Go to T","Equal axis units","Back"};int action=screen_menu("desktop.plot_curves.choose3","Explore path",options,4);
            if(action==0){tangent_on=!tangent_on;tangent_dirty=true;}
            if(action==1)
            {
                double value=at;
                if(analysis_number("Trace T (within range)",value))
                {
                    if(value<source.path->first||value>source.path->last)notice("Trace","T must lie in the parameter interval");
                    else{cursor=int(std::round((value-source.path->first)/(source.path->last-source.path->first)*299));tangent_dirty=true;pan=false;}
                }
            }
            if(action==2)
            {
                double centre=(scene.window[2]+scene.window[3])/2,half=(scene.window[1]-scene.window[0])/2*201/299;
                double next[]={scene.window[0],scene.window[1],centre-half,centre+half};if(pico_graph_scene::bounds(next))memcpy(scene.window,next,sizeof next);
            }
            continue;
        }
        if(key==0x83)
        {
            const char *options[]={tangent_on?"Hide tangent":"Show live tangent","Mark bound at cursor","Clear marked bounds","Go to X",source.work?"Parameters":"Back","Back"};
            int action=screen_menu("desktop.plot_curves.choose4","Explore graph",options,source.work?6:5);
            if(action==4&&source.work){adjusting=true;pan=false;}
            if(action==0){tangent_on=!tangent_on;tangent_dirty=true;}
            if(action==1){bounds.mark(x);pan=false;}
            if(action==2)bounds.clear();
            if(action==3)
            {
                double at=x;
                if(analysis_number("Cursor X (visible window)",at))
                {
                    if(at<scene.window[0]||at>scene.window[1])notice("Cursor","X must lie in the visible window");
                    else{cursor=int(std::round((at-scene.window[0])/(scene.window[1]-scene.window[0])*299));tangent_dirty=true;pan=false;}
                }
            }
            continue;
        }
        if(source.geometry&&!pan&&(key==0xb4||key==0xb7||key==0xb5||key==0xb6)){scatter_index=(scatter_index+((key==0xb4||key==0xb5)?5:1))%6;if(source.scatter_selection)*source.scatter_selection=unsigned(scatter_index);continue;}
        if(source.geometry&&(key==13||key==10)){geometry_move=true;pan=false;continue;}
        if(key==10||key==13){if(source.ode){notice("ODE controls","F3 Explore / controls edits the initial condition");continue;}if(source.path)notice("Path bounds","F2 edits the T interval; function analysis is Cartesian");else{bounds.mark(x);pan=false;}continue;}
        if(key==0x84){pan=!pan;continue;}
        if(!pan&&(key==0xb4||key==0xb7)){scatter_index=-1;cursor=std::max(0,std::min(299,cursor+(key==0xb4?-1:1)));tangent_dirty=true;continue;}
        if(!pan&&!source.path&&(key==0xb5||key==0xb6)){do{selected=(selected+(key==0xb5?2:1))%3;}while(!scene.active[selected]);tangent_dirty=true;overlay.count=0;shade=false;snprintf(heading,sizeof heading,"Plot 2D");continue;}
        double *w=scene.window,cx=(w[0]+w[1])/2,cy=(w[2]+w[3])/2,dx=(w[1]-w[0])/2,dy=(w[3]-w[2])/2;
        if(key=='+'||key=='='){dx/=2;dy/=2;}else if(key=='-'){dx*=2;dy*=2;}
        else if(pan&&key==0xb4)cx-=dx/4;else if(pan&&key==0xb7)cx+=dx/4;else if(pan&&key==0xb5)cy+=dy/4;else if(pan&&key==0xb6)cy-=dy/4;else continue;
        double proposed[]={cx-dx,cx+dx,cy-dy,cy+dy};if(pico_graph_scene::bounds(proposed)){memcpy(w,proposed,sizeof proposed);rebuild=!source.path;tangent_dirty=true;overlay.count=0;shade=false;snprintf(heading,sizeof heading,"%s",source.contour?"Implicit / contours":source.geometry?"Interactive geometry":source.path?(source.path->polar?"Polar plot":"Parametric plot"):"Plot 2D");}
    }
    delete exporting;ui.show_graphics(false);key_pop_all();return status;
}
void plot_label(char *destination,size_t capacity,const char *source)
{
    size_t n=std::min(strlen(source),capacity-1);
    while(n&&!pico_text_editor::valid_utf8(source,n))--n;
    memcpy(destination,source,n);destination[n]=0;
}
pico_plot_request plot_request()
{
    pico_plot_request request{};
    memcpy(request.window,graph_scene.window,sizeof request.window);
    memcpy(request.bounds3d,graph_scene.bounds3d,sizeof request.bounds3d);
    memcpy(request.colour,graph_scene.colour,sizeof request.colour);
    for(int i=0;i<3;++i){request.active[i]=*graph_scene.curve[i]!=0;plot_label(request.label[i],80,graph_scene.curve[i]);}
    request.yaw=graph_scene.camera.yaw;request.pitch=graph_scene.camera.pitch;request.zoom=graph_scene.camera.zoom;
    return request;
}
int db48x_plot_sample(void *opaque,int curve,double x,double y,double *z)
{
    auto **evaluators=static_cast<pico_surface_evaluator **>(opaque);
    auto result=evaluators[curve]->sample(x,y,*z);
    if(result==pico_surface_evaluator::failed){engine_error();return -1;}
    if(result==pico_surface_evaluator::cancelled){rt.clear_error();return -2;}
    return result==pico_surface_evaluator::finite?1:0;
}
void multi_plot()
{
    pico_surface_evaluator a(*graph_scene.curve[0]?graph_scene.curve[0]:"0",false),
        b(*graph_scene.curve[1]?graph_scene.curve[1]:"0",false),c(*graph_scene.curve[2]?graph_scene.curve[2]:"0",false);
    if(!a.valid()||!b.valid()||!c.valid()){engine_error();return;}
    pico_surface_evaluator *evaluators[]={&a,&b,&c};
    auto request=plot_request();plot_curves(request,{db48x_plot_sample,evaluators});
    memcpy(graph_scene.window,request.window,sizeof request.window);
}
bool native_window()
{
    char values[4][40];static const char *const titles[]={"X minimum","X maximum","Y minimum","Y maximum"};double next[4];
    for(int n=0;n<4;++n){snprintf(values[n],sizeof values[n],"%.17g",graph_scene.window[n]);if(!prompt(titles[n],values[n],sizeof values[n]))return false;char *end;next[n]=strtod(values[n],&end);if(*end||!std::isfinite(next[n])){notice("Window","Use finite decimal values");return false;}}
    if(!pico_graph_scene::bounds(next)){notice("Window","Bounds must be ordered and <=1e100");return false;}memcpy(graph_scene.window,next,sizeof next);return true;
}
void multi_menu()
{
    graph_scene.multiple=1;
    for(;;)
    {
        char labels[3][80];for(int c=0;c<3;++c){char snippet[36];pico_formula_label(snippet,sizeof snippet,*graph_scene.curve[c]?graph_scene.curve[c]:"(off)");snprintf(labels[c],sizeof labels[c],"Y%d: %s",c+1,snippet);}
        const char *items[]={"Plot / trace curves",labels[0],labels[1],labels[2],"Window bounds","Cycle curve colours","Back"};int choice=screen_menu("desktop.multi_menu.choose1","2D functions",items,7);
        if(choice<0||choice==6)break;
        if(choice==0)multi_plot();
        else if(choice>=1&&choice<=3)
        {
            char formula[160];snprintf(formula,sizeof formula,"%s",graph_scene.curve[choice-1]);
            if(formula_entry("Edit curve formula",formula,true))
            {
                if(!pico_graph_scene::formula(formula)){notice("Formula","Enter without quotes or Y=");continue;}
                if(*formula){pico_surface_evaluator test(formula,false);if(!test.valid()){engine_error();continue;}}
                snprintf(graph_scene.curve[choice-1],160,"%s",formula);
            }
        }
        else if(choice==4)native_window();
        else if(choice==5){uint32_t c=graph_scene.colour[0];graph_scene.colour[0]=graph_scene.colour[1];graph_scene.colour[1]=graph_scene.colour[2];graph_scene.colour[2]=c;}
    }
}
bool to_scene_double(algebraic_g value,double &number)
{
    settings::SaveHardwareFloatingPoint hardware(true);settings::SavePrecision precision(16);
    if(!value||!algebraic::to_hwdouble(value))return false;
    number=value->type()==object::ID_hwdouble?hwdouble_p(+value)->value():value->type()==object::ID_hwfloat?hwfloat_p(+value)->value():NAN;
    return std::isfinite(number);
}
bool capture_legacy_scene()
{
    error_save errors;PlotParametersAccess p;double bounds[4];pico_graph_scene candidate=graph_scene;
    if(to_scene_double(p.xmin,bounds[0])&&to_scene_double(p.xmax,bounds[1])&&to_scene_double(p.ymin,bounds[2])&&to_scene_double(p.ymax,bounds[3])&&pico_graph_scene::bounds(bounds))memcpy(candidate.window,bounds,sizeof bounds);else return false;
    object_g eq=directory::recall_all(object::static_object(object::ID_Equation),false);
    if(eq)
    {
        if(eq->type()!=object::ID_expression||p.type!=object::ID_Function)return false;
        text_g editable=eq->as_text(true);if(!editable)return false;size_t n;utf8 bytes=editable->value(&n);
        if(n<2||n>161||bytes[0]!='\''||bytes[n-1]!='\'')return false;
        char source[160]={};memcpy(source,bytes+1,n-2);if(!pico_graph_scene::formula(source))return false;
        snprintf(candidate.curve[0],160,"%s",source);
    }
#ifdef CONFIG_COLOR
    candidate.colour[0]=pico_rgb565_to_rgb(uint16_t(Settings.Foreground()));
#else
    candidate.colour[0]=Settings.Foreground()==pattern::black.bits?0:0xffffff;
#endif
    graph_scene=candidate;return true;
}

void scene_menu()
{
    if(!sys_disk_ok()){notice("Scenes","SD card unavailable");return;}pico_f_mkdir("graphs");
    static const char *const items[]={"Save scene","Load scene","Back"};int action=screen_menu("desktop.scene_menu.choose1","Graph scenes",items,3);if(action<0||action==2)return;
    if(action==0)
    {
        char name[40]="LAST";if(!prompt("Scene name",name,sizeof name,true))return;
        if(!graph_scene.multiple&&!capture_legacy_scene()){notice("Scene save","Use a function EQ or Multi-curve tools");return;}graph_scene.angle=angle_number();
        char path[100];snprintf(path,sizeof path,"graphs/%s.scene",name);if(file_exists(path)&&!confirm("Replace saved scene?"))return;
        pico_graph_scene saved=graph_scene;
        if(!saved.multiple){saved.curve[1][0]=saved.curve[2][0]=0;saved.multiple=1;}
        uint8_t data[1024];size_t size=saved.encode(data,sizeof data);
        if(!size){notice("Scene","Invalid scene values");return;}
        bool ok=save_bytes(path,data,size);if(ok)graph_revision.saved();notice(ok?"Scene saved":"Scene save failed",ok?"Stored under /db48x/graphs":"Previous scene retained");return;
    }
    int &selected=pico_ui::focus("files.graphs").selected;
    for(;;)
    {
        char names[8][256]={};int offset=selected/8*8,total=0;DIR d;FILINFO info;
        if(pico_f_opendir(&d,"graphs")!=FR_OK){notice("Scenes","Cannot read scene directory");return;}
        FRESULT result;while((result=f_readdir(&d,&info))==FR_OK&&info.fname[0])
        {
            if(!file_visible(info))continue;char display[256];snprintf(display,sizeof display,"%s",info.fname);const char *ext=strrchr(display,'.');if(!ext)continue;
            if((!strcasecmp(ext,".bak")||!strcasecmp(ext,".rollback")))
            {
                bool old_backup=!strcasecmp(ext,".bak");display[ext-display]=0;ext=strrchr(display,'.');if(!ext||strcasecmp(ext,".scene"))continue;
                char primary[320];snprintf(primary,sizeof primary,"graphs/%s",display);if(file_exists(primary))continue;if(old_backup){char pending[340];snprintf(pending,sizeof pending,"%s.rollback",primary);if(file_exists(pending))continue;}
            }
            else if(strcasecmp(ext,".scene"))continue;
            if(total>=offset&&total<offset+8)snprintf(names[total-offset],256,"%s",display);++total;
        }
        f_closedir(&d);if(result!=FR_OK){notice("Scenes","Directory read failed");return;}
        screen_page("desktop.scene_menu.page1","Load graph scene",pico_ui::Actions{{total?"Open":nullptr,nullptr,nullptr,total?"Details":nullptr,"Back"}});for(int n=0;n<8&&n+offset<total;++n)file_row_label(n,names[n],n+offset==selected);
        list_page_hint(selected,total);if(!total)info_label(8,70,"No scenes yet. Back returns to Save scene.");flush_display();int key=read_key_raw();file_row_key(key,selected,total);
        if(key==27||key==0xb1||key==0x85)return;
        if(key==0xb5&&total)selected=(selected+total-1)%total;else if(key==0xb6&&total)selected=(selected+1)%total;
        else if(key==0xb4&&total)selected=std::max(0,selected-8);else if(key==0xb7&&total)selected=std::min(total-1,selected+8);
        else if(key==0x84&&total)screen_text("files.details","File details",names[selected-offset],strlen(names[selected-offset]));
        else if((key==10||key==13||key==0x81)&&total)
        {
            char path[320];snprintf(path,sizeof path,"graphs/%s",names[selected-offset]);uint8_t data[1024];pico_graph_scene candidate;
            bool recovered=false;bool ok=pico_resource_load(path,data,sizeof data,recovered,[&](size_t size){return candidate.decode(data,size);});
            if(!ok){notice("Scene invalid","Primary and backup are unavailable");return;}
            for(auto &formula:candidate.curve)if(*formula){pico_surface_evaluator parser(formula,false);if(!parser.valid()){engine_error();return;}}
            {pico_surface_evaluator parser(candidate.surface);if(!parser.valid()){engine_error();return;}}
            if(!confirm("Load scene and its angle mode?"))return;
            // Load native scene state only; unrelated calculator variables stay intact.
            if(graph_revision.dirty&&!confirm("Replace unsaved graph changes?"))return;graph_scene=candidate;graph_scene.multiple=1;graph_revision.saved();set_scene_angle(candidate.angle);notice(recovered?"Scene backup loaded":"Scene loaded","Use Plot 2D or 3D surfaces");return;
        }
    }
}
}

namespace {
struct surface_3d
{
    double xmin=-3,xmax=3,ymin=-3,ymax=3,zmin=0,zmax=0;
    double height[pico_wireframe::side*pico_wireframe::side];
    int valid=0;
};
int sample_surface(surface_3d &mesh,const plot_source &source)
{
    pico_plot_request request{};request.bounds3d[0]=mesh.xmin;request.bounds3d[1]=mesh.xmax;request.bounds3d[2]=mesh.ymin;request.bounds3d[3]=mesh.ymax;
    int row=-1;key_pop_all();
    int result=pico_plot_engine::sample_surface(request,source.sample,source.context,mesh.height,mesh.zmin,mesh.zmax,mesh.valid,sampling_poll,&row);
    if(result==0)notice("3D surface","No finite real samples in this window");
    return result;
}

void draw_surface(const surface_3d &mesh,const pico_wireframe::camera &camera,pico_graph_export *exporting=nullptr,bool chrome=true)
{
    if(chrome)screen_page("desktop.draw_surface.page1","3D wireframe: Z=f(X,Y)",pico_ui::Actions{{nullptr,nullptr,nullptr,nullptr,"Back"}});else Screen.fill(0,28,319,250,pattern(Settings.Background()));
    if(exporting)exporting->svg.begin("3D wireframe",export_rgb(pattern(Settings.Background())),export_rgb(pico_ui::text_colour()),0,28,320,223);
    auto previous_clip=Screen.clip();Screen.clip(rect(0,28,319,250));
    auto line=[&](pico_wireframe::point a,pico_wireframe::point b,pattern colour)
    { Screen.line(int(a.x),int(a.y),int(b.x),int(b.y),1,colour);if(exporting)exporting->svg.line(int(a.x),int(a.y),int(b.x),int(b.y),1,export_rgb(colour)); };
    pattern mesh_x=pico_ui::plot_ink(pattern(40,170,235),pattern(Settings.Background()));
    pattern mesh_y=pico_ui::plot_ink(pattern(240,150,55),pattern(Settings.Background()));
    for(int j=0;j<pico_wireframe::side;++j)
        for(int i=0;i<pico_wireframe::side;++i)
        {
            float z=mesh.height[j*pico_wireframe::side+i];if(!std::isfinite(z)) continue;
            float x=-1+float(i)/8,y=-1+float(j)/8;
            auto a=camera.project(x,y,z);
            if(i+1<pico_wireframe::side && std::isfinite(mesh.height[j*pico_wireframe::side+i+1]))
                line(a,camera.project(x+0.125f,y,mesh.height[j*pico_wireframe::side+i+1]),mesh_x);
            if(j+1<pico_wireframe::side && std::isfinite(mesh.height[(j+1)*pico_wireframe::side+i]))
                line(a,camera.project(x,y+0.125f,mesh.height[(j+1)*pico_wireframe::side+i]),mesh_y);
        }
    auto origin=camera.project(-1,-1,-1);
    static const char *const names[]={"X","Y","Z"};
    const pico_wireframe::point ends[]={camera.project(1,-1,-1),camera.project(-1,1,-1),camera.project(-1,-1,1)};
    for(int n=0;n<3;++n)line(origin,ends[n],pico_ui::text_colour());
    int label_x[3],label_y[3];static constexpr int offsets[]={0,-16,16,-32,32};
    for(int n=0;n<3;++n)
    {
        label_x[n]=std::max(4,std::min(300,int(ends[n].x)));
        int proposed=std::max(32,std::min(232,int(ends[n].y)));label_y[n]=proposed;
        for(int offset:offsets)
        {
            int y=std::max(32,std::min(232,proposed+offset));bool overlap=false;
            for(int j=0;j<n;++j)if(std::abs(label_x[n]-label_x[j])<20&&std::abs(y-label_y[j])<16)overlap=true;
            if(!overlap){label_y[n]=y;break;}
        }
        // Readability must use the label's background, not an axis/mesh pixel.
        Screen.fill(label_x[n],label_y[n],label_x[n]+19,label_y[n]+15,pattern(Settings.Background()));
        ui_line(label_x[n],label_y[n],20,16,names[n],pico_ui::TextRole::plot_label,pico_ui::text_colour());
    }
    Screen.clip(previous_clip);
    if(exporting)exporting->svg.end_plot();
    if(exporting){exporting->svg.text(5,280,"3D surface: sampled mesh, auto Z scale",export_rgb(pico_ui::text_colour()));}
    // Chrome and bounds are independent of camera rotation.
    if(chrome)
    {
    Screen.fill(0,0,319,27,pattern(Settings.HeaderBackground()));
    ui_line(8,3,304,23,"3D surface",pico_ui::TextRole::title,pattern(255,255,255));
    Screen.fill(0,251,319,319,pattern(Settings.HeaderBackground()));
    char bounds[96];snprintf(bounds,sizeof bounds,"X %.3g..%.3g  Y %.3g..%.3g",mesh.xmin,mesh.xmax,mesh.ymin,mesh.ymax);
    ui_line(4,257,312,16,bounds,pico_ui::TextRole::status,pattern(255,255,255));
    snprintf(bounds,sizeof bounds,"Z %.3g..%.3g auto | Arrows rotate +/- zoom",mesh.zmin,mesh.zmax);
    ui_line(4,273,312,13,bounds,pico_ui::TextRole::status,pattern(255,255,255));
    action_bar({{"Reset","Export",nullptr,nullptr,"Back"}});
    flush_display();
    }else{mark_dirty(0,28,319,250);lcd_refresh();}
}
int plot_surface(pico_plot_request &request,const plot_source &source)
{
    surface_3d mesh;mesh.xmin=request.bounds3d[0];mesh.xmax=request.bounds3d[1];mesh.ymin=request.bounds3d[2];mesh.ymax=request.bounds3d[3];
    pico_wireframe::camera camera;camera.yaw=request.yaw;camera.pitch=request.pitch;camera.zoom=request.zoom;
    int sampled=sample_surface(mesh,source);if(sampled!=1)return sampled;unsigned surface_frame=~0u;
    for(;;)
    {
        draw_surface(mesh,camera,nullptr,surface_frame!=pico_ui::page_generation);surface_frame=pico_ui::page_generation;int key=read_key_raw();
        if(key==27||key==0xb1||key==0x85)break;
        if(key==0xb4)camera.rotate(-0.15f,0);else if(key==0xb7)camera.rotate(0.15f,0);
        else if(key==0xb5)camera.rotate(0,0.15f);else if(key==0xb6)camera.rotate(0,-0.15f);
        else if(key=='+'||key=='=')camera.magnify(1.15f);else if(key=='-')camera.magnify(1/1.15f);
        else if(key==0x81)camera=pico_wireframe::camera{};
        else if(key==0x82){pico_graph_export file;if(export_open(file,"svg")){draw_surface(mesh,camera,&file);export_done(file,file.svg.finish());}}
    }
    request.yaw=camera.yaw;request.pitch=camera.pitch;request.zoom=camera.zoom;return true;
}
void surface_menu()
{
    char *formula=graph_scene.surface;
    pico_wireframe::camera &camera=graph_scene.camera;
    char plot_label[80];
    const char *items[]={plot_label,"Enter formula","Saddle example","Ripple example","X/Y bounds","Back"};
    for (;;)
    {
        char snippet[41];pico_formula_label(snippet,sizeof snippet,formula);snprintf(plot_label,sizeof plot_label,"Plot: %s",snippet);
        int action=screen_menu("desktop.surface_menu.choose1","3D surfaces",items,6);
        if(action<0 || action==5) break;
        if(action==1||action==7)
        {
            if(!formula_entry("Enter 3D formula",formula,false,true)) continue;
            if(strchr(formula,'\'')) { notice("3D formula","Enter without quotes or Z=");continue; }
        }
        else if(action==2) strcpy(formula,"X^2-Y^2");
        else if(action==3) strcpy(formula,"SIN(X^2+Y^2)");
        else if(action==4)
        {
            double values[4];char text[4][32];
            const double *old=graph_scene.bounds3d;
            static const char *const titles[]={"3D X minimum","3D X maximum","3D Y minimum","3D Y maximum"};
            bool ok=true;
            for(int n=0;n<4;++n)
            {
                snprintf(text[n],sizeof text[n],"%.9g",old[n]);
                if(!prompt(titles[n],text[n],sizeof text[n])) { ok=false;break; }
                char *end;values[n]=strtod(text[n],&end);
                if(*end || !std::isfinite(values[n])) { ok=false;break; }
            }
            if(ok && pico_graph_scene::bounds(values))
            { memcpy(graph_scene.bounds3d,values,sizeof values); }
            else notice("3D bounds","Use finite, ordered bounds");
            continue;
        }
        pico_surface_evaluator evaluator(formula);
        if(!evaluator.valid()){engine_error();continue;}
        pico_surface_evaluator *evaluators[]={&evaluator};auto request=plot_request();request.surface=1;
        plot_surface(request,{db48x_plot_sample,evaluators});
        camera.yaw=request.yaw;camera.pitch=request.pitch;camera.zoom=request.zoom;
    }
    key_pop_all();
}
}

namespace {
void use_calculus_result(object_g value,const char *title)
{
    char source[160];bool plottable=pico_formula_source(value,source,sizeof source);
    if(!plottable&&rt.error()){engine_error();return;}
    if(screen_result("desktop.use_calculus_result.value_result1",value,title,"",true))
    {
        if(!plottable){notice("Calculus","Result exceeds formula limit");return;}
        snprintf(graph_scene.curve[0],160,"%s",source);graph_scene.curve[1][0]=graph_scene.curve[2][0]=0;graph_scene.multiple=1;multi_plot();
    }
}
void calculus_menu()
{
    for(;;)
    {
        static const char *const items[]={"Derivative d/dX (symbolic)","Antiderivative (+C implicit)","Slope at X (numerical)","Definite integral (numerical)","Area under curve (absolute)","Definite integral (symbolic)","Back"};
        int action=screen_menu("desktop.calculus_menu.choose1","Calculus in X",items,7);if(action<0||action==6)break;
        char formula[160];snprintf(formula,sizeof formula,"%s",*graph_scene.curve[0]?graph_scene.curve[0]:"X^2");
        if(!formula_entry("Calculus formula",formula))continue;
        if(action<=1)
        {
            cleaner temporary;expression_g answer=pico_formula_calculus(formula,action==1);
            if(!answer){engine_error();continue;}
            object_g value=object_p(+answer);
            use_calculus_result(value,action?"Antiderivative (+ C)":"Derivative");
        }
        else if(action==5)
        {
            char lower[160]="0",upper[160]="1";
            if(!prompt("Symbolic integral: lower bound",lower,sizeof lower)||!prompt("Symbolic integral: upper bound",upper,sizeof upper))continue;
            object_g value=pico_formula_definite(formula,lower,upper);
            if(!value){engine_error();continue;}
            use_calculus_result(value,"DB48X definite integral");
        }
        else
        {
            double a=graph_scene.window[0],b=graph_scene.window[1],at=0;
            if(action==2){if(!analysis_number("Slope at X =",at))continue;}
            else if(!analysis_number("Integral: first X",a)||!analysis_number("Integral: last X",b))continue;
            pico_surface_evaluator evaluator(formula,false);if(!evaluator.valid()){engine_error();continue;}
            pico_surface_evaluator *evaluators[]={&evaluator};
            auto operation=action==2?pico_plot_analysis::operation::derivative:action==3?pico_plot_analysis::operation::integral:pico_plot_analysis::operation::area;
            screen_page("desktop.calculus_menu.page1","Calculating",pico_ui::Actions{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});flush_display();
            auto result=pico_plot_analysis::run(operation,db48x_plot_sample,evaluators,0,0,a,b,at,sampling_poll);
            if(result.state!=pico_plot_analysis::status::ok){notice("Calculus",pico_plot_analysis::message(result.state));continue;}
            char report[320];snprintf(report,sizeof report,"Estimated error: %.3g\nEvaluations: %d\n%s",result.error,result.evaluations,action==4?"Area is nonnegative.":action==3?"Integral is signed.":"Slope uses a centred finite difference.");
            object_g number=hwdouble::make(result.value);if(!number){engine_error();continue;}
            screen_result("desktop.calculus_menu.value_result1",number,items[action],report);
        }
    }
}
}

extern "C" int pico_native_plot(pico_plot_request *request,pico_plot_sample sample,void *context)
{
    if(!request||!sample||!pico_graph_scene::bounds(request->surface?request->bounds3d:request->window))return 0;
    save<bool> direct_screen(user_display_enable,false);rect clip=Screen.clip();Screen.clip(Screen.area());
    int status=request->surface?plot_surface(*request,{sample,context}):plot_curves(*request,{sample,context});
    Screen.clip(clip);ui.show_graphics(false);key_pop_all();return status;
}

void pico_graph_menu()
{
    if(rt.editing()){notice("Graphs","Finish or cancel the calculator entry");return;}
    save<bool> direct_screen(user_display_enable,false);
    if(!graph_scene.multiple&&capture_legacy_scene()){graph_scene.curve[1][0]=graph_scene.curve[2][0]=0;graph_scene.multiple=1;}
    for(;;)
    {
        pico_ui::Mutation<pico_graph_scene> mutation(graph_scene,graph_revision);
        static const char *const items[]={"2D functions","Parametric / polar","3D surfaces","Differential equations","Saved graphs","Interactive geometry","Inequalities","Implicit / contour plots"};
        int action=screen_menu("desktop.pico_graph_menu.choose1","Graphs",items,8);if(action<0)break;
        if(action==0)multi_menu();else if(action==1)pico_path_menu();else if(action==2)surface_menu();else if(action==3)pico_ode_menu();else if(action==4)scene_menu();else if(action==5)geometry_menu();else if(action==6)lab_launch(2);else research_launch(1);
    }
    key_pop_all();
}


namespace {
void battery_status()
{
    for(;;)
    {
        pico_battery_state status;pico_battery_snapshot(&status);
        screen_page("desktop.battery_status.page1","Battery status",pico_ui::Actions{{"refresh",nullptr,nullptr,nullptr,"Back"}});char line[128];
        if(status.valid)snprintf(line,sizeof line,"Controller level: %d%%",status.percent);
        else snprintf(line,sizeof line,"Controller level: unavailable");
        info_label(8,40,line);
        info_label(8,67,status.valid?(status.charging?"Charging: yes":"Charging: no"):"Charging: unknown");
        bool unsupported=pico_battery_bios_unsupported(status.bios_version);
        if(unsupported)snprintf(line,sizeof line,"Battery query skipped");
        else if(status.last_reply<0)snprintf(line,sizeof line,"Last I2C transaction failed");
        else snprintf(line,sizeof line,"Last reply: 0x%04X",unsigned(status.last_reply));
        info_label(8,94,line);
        if(status.valid)snprintf(line,sizeof line,"Valid sample age: %lu s",(unsigned long)((uint32_t(sys_current_ms())-status.valid_at)/1000));
        else snprintf(line,sizeof line,"No current valid sample");
        info_label(8,121,line);
        if(status.bios_version>=0)snprintf(line,sizeof line,"Keyboard BIOS: %X.%X (0x%02X)",status.bios_version>>4,status.bios_version&15,status.bios_version);
        else snprintf(line,sizeof line,"Keyboard BIOS: unknown");info_label(8,148,line);
        snprintf(line,sizeof line,"Write %d/1 | Read %d/2",status.write_result,status.read_result);info_label(8,175,line);
        info_label(8,202,unsupported?"Battery needs keyboard BIOS 1.2+":status.write_result!=1?"Failed: register write":status.read_result!=2?"Failed: response read":pico_battery_reply_valid(status.last_reply)?"Valid battery response":"Invalid battery response");
        info_label(8,229,"500 ms timeout | 16 ms pause");
        info_label(8,256,"-1: bus error  -2: timeout");flush_display();
        int key=read_key_raw();if(key==27||key==0xb1||key==0x85)break;
    }
}
}



#include "workbench_ui.inc"
#include "document_browser.inc"
#include "data_ui.inc"

#include "form_ui.inc"

#include "notebook_ui.inc"

#include "path_ui.inc"

#include "ode_ui.inc"

#include "conversion_ui.inc"
#include "math_tools_ui.inc"
#include "exploration_ui.inc"
#include "math_lab_ui.inc"
#include "advanced_math_ui.inc"
#include "science_ui.inc"
#include "research_ui.inc"

namespace {

void memory_status()
{
            pico_ui_performance timing;pico_ui_performance_snapshot(&timing);
            pico_memory_observation memory=pico_memory_observe();
            char status[1800];snprintf(status,sizeof status,
                "Heap allocated now: %lu bytes\nHeap committed peak: %lu bytes\nStack watermark: %lu / 32768 bytes\n%s\n\nPSRAM: %s\nCapacity: %lu KiB\nBacking slots reserved: %u KiB\nParkable document payload: %lu bytes\nEditor undo working structure: %lu bytes\n\nInternal SRAM budgets:\nCalculator arena: 208 KiB\nHeap: 96 KiB (Lua limit: 48 KiB)\nStack reservation: 32 KiB\n256-colour framebuffer: 100 KiB\nShared editor / plot buffer: 16 KiB\nLargest workspace request: %u bytes\n\nInactive math labs, matrices, finance, forms, notebooks, data and path/ODE documents use checked PSRAM backing. Editor undo and paused Lua-editor text, plus active graph samples, use PSRAM. Without PSRAM, documents and undo stay in the bounded heap. PSRAM is volatile; save to SD to survive power-off.\n\nLast display: %lu dirty rows, %lu us transfer\nDecoded key to display: %lu us\nLast cursor: %lu rows, %lu us\nMaximum cursor: %lu us\nDevice timings include SPI transfer; host captures are model results.",
                (unsigned long)memory.heap_allocated,(unsigned long)memory.heap_committed_peak,(unsigned long)memory.stack_high_water,memory.device?"Device measurements since boot":"Host/model: device measurements unavailable",pico_psram_available()?"self-test passed":"unavailable; SRAM fallback",
                (unsigned long)(pico_psram_capacity()/1024),unsigned(pico_bulk_used()/1024),(unsigned long)(pico_memory_profile[0]+pico_memory_profile[1]+pico_memory_profile[2]+pico_memory_profile[3]+pico_memory_profile[4]+pico_memory_profile[7]+pico_memory_profile[8]+pico_memory_profile[9]+pico_memory_profile[10]+pico_memory_profile[11]+pico_memory_profile[12]+pico_memory_profile[13]+pico_memory_profile[14]+pico_memory_profile[15]),(unsigned long)pico_memory_profile[5],unsigned(pico_workspace_peak()),(unsigned long)timing.rows,(unsigned long)timing.refresh_us,(unsigned long)timing.input_us,(unsigned long)timing.cursor_rows,(unsigned long)timing.cursor_us,(unsigned long)timing.max_cursor_us);
            screen_text("desktop.memory_status.view_text1","Memory / PSRAM",status,strlen(status));
        
}
void about_menu()
{
            static constexpr char about[]=PICO_PROGRAM_NAME "\nVersion " PICO_PROGRAM_VERSION_STRING "\nFor PicoCalc with Pico 2 W\n\nDesign direction and device testing\nIain Bennett\n\nScientific calculation, RPL and Lua programming, native graphs and saved workbenches.\n\nPowered by DB48X and the open-source components acknowledged in Credits / open source.";
            static const char *const entries[]={"About calculator","Credits / open source","Back"};
            for(;;)
            {
                int selected=screen_menu("desktop.about_menu.choose1","About / credits",entries,3);
                if(selected<0||selected==2)break;
                if(selected==0)screen_text("desktop.about_menu.view_text1","About calculator",about,strlen(about));
                else screen_text("desktop.about_menu.view_text2","Credits",pico_credits::text,strlen(pico_credits::text));
            }
        
}
void native_settings()
{
    for(;;)
    {
        static const char *const items[]={"Display / theme","Calculation","Clock / timezone","Network","Device status"};int a=screen_menu("desktop.native_settings.choose1","Settings",items,5);if(a<0)return;
        if(a==0){const char *names[pico_themes::count];for(unsigned i=0;i<pico_themes::count;++i)names[i]=pico_themes::themes[i].name;int n=screen_menu("desktop.native_settings.choose2","Display / theme",names,pico_themes::count);if(n>=0)apply_theme(n);}
        if(a==1)
        {
            static const char *const names[]={"Angle mode","Precision","Advanced RPL settings"};int n=screen_menu("desktop.native_settings.choose3","Calculation",names,3);
            if(n==0){static const char *const angles[]={"Degrees","Radians","Grads","Pi radians"};int k=screen_menu("desktop.native_settings.choose4","Angle mode",angles,4);if(k>=0)set_scene_angle(k);}
            if(n==1){char text[16];snprintf(text,sizeof text,"%u",unsigned(Settings.Precision()));if(prompt("Significant digits",text,sizeof text)){char *end;unsigned long v=strtoul(text,&end,10);if(*end||v<1||v>DB48X_MAXDIGITS)notice("Precision","Value exceeds calculator precision range");else Settings.Precision(v);}}
            if(n==2){pico_open_calculator_menu(true);return;}
        }
        if(a==2)timezone_menu();if(a==3)pico_wifi_menu();
        if(a==4){static const char *const names[]={"Battery","Memory / PSRAM","Radio / connection"};int n=screen_menu("desktop.native_settings.choose5","Device status",names,3);if(n==0)battery_status();if(n==1)memory_status();if(n==2)wifi_status();}
    }
}
#include "reference_ui.inc"
void editor_language_reference(source_language language)
{native_command_reference(nullptr,false,language==source_language::rpl?rpl_book:language==source_language::lua?lua_book:chip8_book);key_pop_all();}
void reference_find(){native_command_reference(nullptr,true);}
void calculator_reference()
{
    constexpr unsigned count=sizeof pico_ui_help::topics/sizeof *pico_ui_help::topics;
    const char *names[count+1];for(unsigned i=0;i<count;++i)names[i]=pico_ui_help::topics[i].title;
    names[count]="Command reference";
    for(;;)
    {
        int selected=screen_menu("reference.topics","PicoCalc calculator reference",names,count+1);
        if(selected<0)return;
        if(unsigned(selected)==count){native_command_reference();continue;}
        const auto &topic=pico_ui_help::topics[selected];
        screen_text("reference.topic","PicoCalc reference",topic.text,strlen(topic.text));
    }
}
void native_help()
{
    for(;;)
    {
        static const char *const items[]={"Calculator reference","Command catalog","Keyboard shortcuts","Lua reference","CHIP-8 reference","About LumaRPL","Credits / licenses"};int a=screen_menu("desktop.native_help.choose1","Help & about",items,7);if(a<0)return;
        if(a==0){calculator_reference();if(return_to_calculator)return;}
        if(a==1){pico_open_calculator_menu(false);return;}
        if(a==2){const char *text=pico_ui_help::tool_text("desktop.native_help.view_text1");screen_text("desktop.native_help.view_text1","Keyboard shortcuts",text,strlen(text));}
        if(a==3)native_command_reference(nullptr,false,lua_book);if(a==4)native_command_reference(nullptr,false,chip8_book);
        if(a==5)about_menu();if(a==6)screen_text("desktop.native_help.view_text2","Credits / licenses",pico_credits::text,strlen(pico_credits::text));
    }
}
void native_files()
{
    for(;;)
    {
        static const char *const items[]={"Save session","Load session","Workbenches","Graphs / paths / equations","Forms","Notebooks","Data / CSV","Save & return to loader"};int a=screen_menu("desktop.native_files.choose1","Files & session",items,8);if(a<0)return;
        if(a==0)pico_handle_shortcut(PICO_SAVE);if(a==1){pico_handle_shortcut(PICO_LOAD);return_to_calculator=true;return;}
        if(a==2)workbench_saved();
        if(a==3){static const char *const names[]={"Function scenes","Parametric / polar","Differential equations"};int n=screen_menu("desktop.native_files.choose2","Graph documents",names,3);if(n==0)scene_menu();if(n==1){pico_document_access<pico_path_scene> access(path_scene_cache,path_scene);if(path_scene)path_files();}if(n==2){pico_document_access<pico_ode_scene> access(ode_scene_cache,ode_scene);if(ode_scene)ode_files();}}
        if(a==4){pico_document_access<pico_form_document> access(active_form_cache,active_form);if(active_form)form_files();}
        if(a==5){pico_document_access<pico_notebook> access(notebook_cache,notebook);if(notebook)notebook_files();}
        if(a==6){pico_document_access<pico_data_table> access(data_cache,data_table);if(data_table)data_files();}
        if(a==7)pico_handle_shortcut(PICO_EXIT);
    }
}
}
bool pico_ui_calculator_redraw(bool consume)
{static unsigned rendered=~0u;unsigned generation=pico_ui::page_generation;bool changed=rendered!=generation;if(consume)rendered=generation;return changed;}
void pico_ui_draw_message(const char *title,const char *const *lines,unsigned count,bool error){legacy_message(title,lines,count,error);}
void pico_about_menu(){about_menu();}
void pico_settings_menu(){native_settings();}
void pico_reference_find(){reference_find();}

void pico_show_command_reference(const char *topic,size_t length)
{
    char name[80];if(!length)length=topic?strlen(topic):0;length=std::min(length,sizeof name-1);if(topic)memcpy(name,topic,length);name[length]=0;
    native_command_reference(name);key_pop_all();
}

void pico_open_calculator_menu(int menu)
{
    if(rt.error())return;
    Stack.interactive=false;
    ui.clear_help();
    ui.menu_root();
    object::static_object(menu==3?object::ID_MathMenu:menu==2?object::ID_BasesMenu:menu==1?object::ID_ModesMenu:object::ID_Catalog)->evaluate();
    return_to_calculator=true;
}
void pico_setup_menu()
{
    return_to_calculator=false;
    for(;;)
    {
        static const char *const items[]={"Calculator","Math & graphs","Data & notebooks","Programming","Conversions","Files & session","Settings","Help & about"};int a=screen_menu("desktop.pico_setup_menu.choose1","LumaRPL Home",items,8);if(a<=0)return;
        if(rt.editing()&&a>=1&&a<=5){notice("Calculator entry","Finish with Enter or cancel with Esc before opening this tool");continue;}
        if(a==1){for(;;){static const char *const names[]={"Graphs","Equation solving","Calculus & algebra","Formula forms","Matrices & vectors","Finance / engineering","Probability","More: complex / applied math"};int n=screen_menu("desktop.pico_setup_menu.choose2","Math & graphs",names,8);if(n<0)break;if(n==0)pico_graph_menu();if(n==1)pico_workbench_menu();if(n==2){for(;;){const char *tools[]={"Calculus in X","Multivariable calculus","Symbolic algebra","Value / approximation","Guided polynomials"};int t=screen_menu("math.lab.calculus_route","Calculus & algebra",tools,5);if(t<0)break;if(t==0)calculus_menu();if(t==1)lab_launch(3);if(t==2)lab_launch(4);if(t==4)advanced_launch(0);if(t==3){if(!rt.depth())notice("Stack empty","Enter a calculator value first");else screen_result("math.result.stack",rt.top(),"Value / approximation","");}}}if(n==3)pico_forms_menu();if(n==4){for(;;){const char *tools[]={"Matrix workbench","Vector workbench"};int t=screen_menu("math.lab.matrix_route","Matrices & vectors",tools,2);if(t<0)break;if(t==0)matrix_menu();if(t==1)lab_launch(0);}}if(n==5){for(;;){const char *tools[]={"Financial calculator","Engineering: RLC / RC filters"};int t=screen_menu("advanced.finance_route","Finance / engineering",tools,2);if(t<0)break;if(t==0)finance_menu();if(t==1)advanced_launch(2);}}if(n==7){for(;;){const char *tools[]={"Complex workbench","Units and dimensions","Measurement uncertainty","Interpolation / curve fitting","Discrete mathematics","Signal analysis","Advanced studies"};int t=screen_menu("science.route","More mathematics",tools,7);if(t<0)break;if(t==0)complex_menu();else if(t==6)research_launch();else science_launch(t-1);}}if(n==6)lab_launch(1);}}
        if(a==2){for(;;){static const char *const names[]={"Data / statistics","Calculation notebooks","Probability","Inference / distributions","Interpolation / curve fitting","Signal analysis"};int n=screen_menu("desktop.pico_setup_menu.choose3","Data & notebooks",names,6);if(n<0)break;if(n==0)pico_data_menu();if(n==1)pico_notebooks_menu();if(n==2)lab_launch(1);if(n==3)advanced_launch(3);if(n==4)science_launch(2);if(n==5)science_launch(4);}}
        if(a==3)pico_program_browser();if(a==4)pico_conversions_menu();if(a==5)native_files();if(a==6)native_settings();if(a==7)native_help();
        if(return_to_calculator)return;
    }
}
namespace {
int pending_document(int which)
{
    if(which==0)return workbench_revision.dirty;
    if(which==1)return graph_revision.dirty;
    if(which==2){if(!active_form_cache.resident&&!active_form_cache.handle)return 0;pico_document_access<pico_form_document> access(active_form_cache,active_form);return active_form?int(active_form->dirty):-1;}
    if(which==3){if(!notebook_cache.resident&&!notebook_cache.handle)return 0;pico_document_access<pico_notebook> access(notebook_cache,notebook);return notebook?int(notebook->dirty):-1;}
    if(which==4){if(!data_cache.resident&&!data_cache.handle)return 0;pico_document_access<pico_data_table> access(data_cache,data_table);return data_table?int(data_table->dirty):-1;}
    if(which==7){if(!matrix_cache.resident&&!matrix_cache.handle)return 0;pico_document_access<pico_matrix_document> access(matrix_cache,matrix_doc);return matrix_doc?int(matrix_doc->dirty):-1;}
    if(which==8){if(!finance_cache.resident&&!finance_cache.handle)return 0;pico_document_access<pico_finance_document> access(finance_cache,finance_doc);return finance_doc?int(finance_doc->dirty):-1;}
    if(which==9){if(!matrix_b_cache.resident&&!matrix_b_cache.handle)return 0;pico_document_access<pico_matrix_document> access(matrix_b_cache,matrix_b_doc);return matrix_b_doc?int(matrix_b_doc->dirty):-1;}
    if(which==10){if(!geometry_cache.resident&&!geometry_cache.handle)return 0;pico_document_access<pico_geometry_document> access(geometry_cache,geometry_doc);return geometry_doc?int(geometry_doc->dirty):-1;}
    if(which==15){if(!research_cache.resident&&!research_cache.handle)return 0;pico_document_access<pico_research_document> access(research_cache,research);return research?int(research->dirty):-1;}
    if(which==14){if(!science_cache.resident&&!science_cache.handle)return 0;pico_document_access<pico_science_document> access(science_cache,science);return science?int(science->dirty):-1;}
    if(which==13){if(!advanced_cache.resident&&!advanced_cache.handle)return 0;pico_document_access<pico_advanced_math> access(advanced_cache,advanced);return advanced?int(advanced->dirty):-1;}
    if(which==12){if(!lab_cache.resident&&!lab_cache.handle)return 0;pico_document_access<pico_math_lab> access(lab_cache,lab);return lab?int(lab->dirty):-1;}
    if(which==11){if(!complex_cache.resident&&!complex_cache.handle)return 0;pico_document_access<pico_complex_document> access(complex_cache,complex_doc);return complex_doc?int(complex_doc->dirty):-1;}
    if(which==5)return path_revision.dirty;return ode_revision.dirty;
}
bool save_pending(int which)
{
    if(!sys_disk_ok()){notice("Save unavailable","Insert the SD card; working documents remain in memory");return false;}
    static const char *const folders[]={"workbenches","graphs","forms","notebooks","datasets","paths","odes","matrices","finance","matrices","geometry","complex","mathlabs","advanced","science","studies"};
    static const char *const extensions[]={".workbench",".scene",".form",".notebook",".csv",".path",".ode",".matrix",".finance",".matrix",".geometry",".complex",".mathlab",".advanced",".science",".study"};
    char path[320];if(!document_save_name(folders[which],extensions[which],path,sizeof path))return false;
    if(which==15){pico_document_access<pico_research_document> access(research_cache,research);return research&&research_save(path);}
    if(which==14){pico_document_access<pico_science_document> access(science_cache,science);return science&&pico_math_save(path,*science);}
    if(which==13){pico_document_access<pico_advanced_math> access(advanced_cache,advanced);return advanced&&pico_math_save(path,*advanced);}
    if(which==12){pico_document_access<pico_math_lab> access(lab_cache,lab);return lab&&pico_math_save(path,*lab);}
    if(which==10){pico_document_access<pico_geometry_document> access(geometry_cache,geometry_doc);return geometry_doc&&pico_math_save(path,*geometry_doc);}
    if(which==11){pico_document_access<pico_complex_document> access(complex_cache,complex_doc);return complex_doc&&pico_math_save(path,*complex_doc);}
    if(which==9){pico_document_access<pico_matrix_document> access(matrix_b_cache,matrix_b_doc);return matrix_b_doc&&pico_math_save(path,*matrix_b_doc);}
    if(which==7){pico_document_access<pico_matrix_document> access(matrix_cache,matrix_doc);return matrix_doc&&pico_math_save(path,*matrix_doc);}
    if(which==8){pico_document_access<pico_finance_document> access(finance_cache,finance_doc);return finance_doc&&pico_math_save(path,*finance_doc);}
    if(which==0){workbench.scene.angle=angle_number();bool ok=pico_workbench_save(path,workbench);if(ok)workbench_revision.saved();return ok;}
    if(which==1){graph_scene.angle=angle_number();uint8_t data[1024];size_t n=graph_scene.encode(data,sizeof data);bool ok=n&&save_bytes(path,data,n);if(ok)graph_revision.saved();return ok;}
    if(which==2){pico_document_access<pico_form_document> access(active_form_cache,active_form);bool ok=active_form&&pico_form_save(path,*active_form);if(ok)active_form->dirty=false;return ok;}
    if(which==3){pico_document_access<pico_notebook> access(notebook_cache,notebook);bool ok=notebook&&pico_notebook_save(path,*notebook);if(ok)notebook->dirty=false;return ok;}
    if(which==4){pico_document_access<pico_data_table> access(data_cache,data_table);bool ok=data_table&&pico_data_save(path,*data_table);if(ok)data_table->dirty=false;return ok;}
    if(which==5){pico_document_access<pico_path_scene> access(path_scene_cache,path_scene);bool ok=path_scene&&pico_path_save(path,*path_scene);if(ok)path_revision.saved();return ok;}
    pico_document_access<pico_ode_scene> access(ode_scene_cache,ode_scene);bool ok=ode_scene&&pico_ode_save(path,*ode_scene);if(ok)ode_revision.saved();return ok;
}
}
bool pico_prepare_session_action()
{
    if(!rt.editing())return true;
    notice("Calculator entry","Finish with Enter or cancel with Esc before saving/loading a session or returning to the loader.");return false;
}
bool pico_prepare_exit()
{
    static const char *const names[]={"Workbench","Graphs","Form","Notebook","Data table","Parametric / polar","Differential equations","Sprite","Matrix A","Finance","Matrix B","Geometry","Complex","Math lab","Advanced math","Applied math","Connected study"};
    bool pending[17];char text[384]="Unsaved documents:\n";size_t used=strlen(text);bool any=false;
    for(int i=0;i<17;++i){int state=i==7?int(sprite_dirty):pending_document(i>7?i-1:i);if(state<0){notice("Document unavailable","Cannot check unsaved work; stay in calculator and recover the document");return false;}pending[i]=state;if(state){any=true;used+=snprintf(text+used,sizeof text-used,"\n%s",names[i]);}}
    if(!any)return true;
    int action=screen_text("desktop.pico_prepare_exit.view_text1","Return to loader",text,strlen(text),pico_ui::TextRole::prose,pico_ui::Actions{{"Save all",nullptr,"Leave",nullptr,"Cancel"}});
    if(!action)return false;
    if(action==3)return confirm("Leave without saving these documents? Unsaved changes will be lost.");
    for(int i=0;i<17;++i)if(pending[i]&&!(i==7?save_sprite_document():save_pending(i>7?i-1:i))){notice("Exit cancelled","Saving did not complete; working documents remain in memory");return false;}
    return true;
}

#undef choose
