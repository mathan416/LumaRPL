// Shared firmware/host UI implementation. GPL-3.0.
#include "help_data.h"
// Keep the bounded reader buffer out of every nested menu stack frame.
__attribute__((noinline)) void menu_details(const char *id,unsigned row,const char *name)
{
    pico_ui::ScreenScope scope("dialog.details");
    char description[1024];
    if(pico_ui_help::describe(id,row,name,description,sizeof description))
        view_text("Details",description,strlen(description));
}
void notice(const char *title,const char *message)
{
    pico_ui::ScreenScope scope("dialog.notice");
    if(strchr(message,'\n')||native_compact_font()->width(utf8(message))>304){view_text(title,message,strlen(message));return;}
    page(title,pico_ui::Actions{{nullptr,nullptr,nullptr,nullptr,"Back"}});info_label(8,48,message);flush_display();key_pop_all();for(;;){int key=read_key_raw();if(key==27||key==0xb1||key==0x85||key==10||key==13)break;}key_pop_all();
}
void engine_error()
{
    pico_ui::ScreenScope scope("legacy.error");
    char report[384];snprintf(report,sizeof report,"%s",rt.error()?cstring(rt.error()):"Operation failed");
    object_g command=rt.command();
    if(command)
    {
        char text[160]={};command->render(text,sizeof text);text[sizeof text-1]=0;
        size_t n=strlen(report);snprintf(report+n,sizeof report-n,"\nCommand: %s",text);
    }
    view_text("Calculator error",report,strlen(report));rt.clear_error();key_pop_all();
}
int choose_impl(const char *title,const char *const *items,int count,pico_ui::MenuState &state,const char *status=nullptr)
{
    auto back=[](const char *s){return !strcmp(s,"Back")||!strcmp(s,"Cancel")||!strcmp(s,"Back to plot");};
    int visible=0,back_index=-1;for(int i=0;i<count;++i)if(back(items[i]))back_index=i;else ++visible;
    if(!visible)return back_index;state.selected=std::max(0,std::min(state.selected,visible-1));key_pop_all();
    auto original=[&](int row){for(int i=0;i<count;++i)if(!back(items[i])&&row--==0)return i;return -1;};
    unsigned frame=~0u;
    for(;;)
    {
        const bool details=pico_ui_help::context(pico_ui::active_screen);
        bool chrome=content_page(title,pico_ui::Actions{{"Open",visible>8&&state.selected>=8?"Previous":nullptr,visible>8&&pico_native_menu::offset(state.selected)+8<visible?"Next":nullptr,details?"Details":nullptr,"Back"}},frame);
        int offset=pico_native_menu::offset(state.selected);
        for(int n=offset;n<visible&&n<offset+8;++n)
        {
            int y=37+(n-offset)*27;label(28,y,items[original(n)],n==state.selected);
            char number[]={char('1'+n-offset),0};ui_line(7,y+3,16,16,number,pico_ui::TextRole::status,n==state.selected?pattern(255,255,255):pico_ui::text_colour());
        }
        list_page_hint(state.selected,visible,true);
        if(visible<=8&&status&&*status)info_label(8,270,status);
        if(pico_ui::menu_observer)pico_ui::menu_observer(pico_ui::active_screen,items,count,state.selected);
        content_refresh(chrome,frame);
        for(;;)
        {
        int key=read_key_raw(),previous=state.selected;
        if(key==27||key==0xb1)return -1;if(key==0x85)return back_index;
        if(key==0xb5)state.selected=(state.selected+visible-1)%visible;
        else if(key==0xb6)state.selected=(state.selected+1)%visible;
        else if((key==0xb4||(key==0x82&&state.selected>=8))&&visible>8)state.selected=std::max(0,(state.selected/8-1)*8);
        else if((key==0xb7||(key==0x83&&pico_native_menu::offset(state.selected)+8<visible))&&visible>8)state.selected=std::min(visible-1,(state.selected/8+1)*8);
        else if(key==10||key==13||key==0x81)return original(state.selected);
        else if(key==0x84&&details){menu_details(pico_ui::active_screen,original(state.selected),items[original(state.selected)]);break;}
        else if(key>='1'&&key<='8'){int row=offset+key-'1';if(row<visible){state.selected=row;return original(row);}}
        if(state.selected!=previous){if(pico_native_menu::offset(state.selected)!=pico_native_menu::offset(previous))frame=~0u;break;}
        }
    }
}
#define PICO_UI_STRING_INNER(x) #x
#define PICO_UI_STRING(x) PICO_UI_STRING_INNER(x)
#define choose(title,items,count) choose_impl(title,items,count,pico_ui::focus(__FILE__ ":" PICO_UI_STRING(__LINE__)))
bool confirm(const char *question)
{
    pico_ui::ScreenScope screen_scope("dialog.confirm");
    key_pop_all();bool accept=false;unsigned frame=~0u;
    for(;;)
    {
        bool chrome=content_page("Confirm action",pico_ui::Actions{{"Confirm",nullptr,nullptr,nullptr,"Cancel"}},frame);
        pico_information_text::draw(Screen,native_compact_font(),question,strlen(question),0,8,45,304,16,10,0,pico_ui::text_colour());
        label(12,232,"F1 Confirm",accept);label(12,259,"F5 Cancel",!accept);content_refresh(chrome,frame);
        int key=read_key_raw();
        if(key==27||key==0xb1||key==0x85)return false;
        if(key==0x81)return true;
        if(key==0xb4||key==0xb7||key==0xb5||key==0xb6)accept=!accept;
        if(key==10||key==13)return accept;
    }
}

int unsaved_source(const char *title="Unsaved source")
{
    pico_ui::ScreenScope screen_scope("editor.unsaved");
    key_pop_all();
    page(title,pico_ui::Actions{{"Save",nullptr,"Discard",nullptr,"Cancel"}});
    info_label(8,55,"Save changes before leaving the editor?");info_label(8,87,"Enter or Esc keeps the editor open.");flush_display();
    for(;;)
    {
        int key=read_key_raw();if(key==0x81)return 1;if(key==0x83)return 2;if(key==0x85||key==27||key==0xb1||key==10||key==13)return 0;
    }
}
bool prompt(const char *title,char *out,size_t capacity,bool filename=false,bool allow_empty=false,const char *history=nullptr,bool secret=false)
{
    pico_ui::ScreenScope screen_scope("dialog.prompt");
    key_pop_all();
    pico_text_editor editor{out,capacity,strlen(out),strlen(out),false};
    bool untouched=true;unsigned frame=~0u;
    for (;;)
    {
        bool chrome=content_page(title,pico_ui::Actions{{"Apply",nullptr,nullptr,nullptr,"Cancel"}},frame);
        ui_line(8,31,304,16,"Type here:",pico_ui::TextRole::status,pico_ui::text_colour());
        pattern border(50,140,230);
        Screen.fill(3,50,316,50,border);Screen.fill(3,194,316,194,border);
        Screen.fill(3,50,3,194,border);Screen.fill(316,50,316,194,border);
        size_t row,column;editor.position(editor.cursor,row,column);
        size_t scroll=row>4?row-4:0;
        size_t r=0,draw_column=0;
        for(size_t n=0;n<editor.length;n=editor.next(n))
        {
            if(r>=scroll&&r<scroll+5)
            {char glyph[5]={};size_t count=editor.next(n)-n;if(secret)strcpy(glyph,"*");else memcpy(glyph,out+n,count);Screen.text(8+draw_column*10,55+(r-scroll)*24,utf8(glyph),LibMonoFont10x17,pico_ui::text_colour());}
            if(++draw_column==30){++r;draw_column=0;}
        }
        Screen.fill(8+column*10,55+(row-scroll)*24,9+column*10,74+(row-scroll)*24,pattern(Settings.CursorBackground()));
        info_label(8,210,untouched?"Start typing to replace the example":"Arrows move the cursor");
        content_refresh(chrome,frame);int c=read_key_raw();
        if(c==27 || c==0xb1 || c==0x85) return false;
        if(c==10 || c==13 || c==0x81) return allow_empty||editor.length>0;
        if(c==0xb4) { editor.cursor=editor.previous(editor.cursor);untouched=false; }
        else if(c==0xb7) { editor.cursor=editor.next(editor.cursor);untouched=false; }
        else if(history && c==0xb5){snprintf(out,capacity,"%s",history);editor.length=editor.cursor=strlen(out);untouched=false;}
        else if(history && c==0xb6){out[0]=0;editor.length=editor.cursor=0;untouched=false;}
        else if(c==8 || c==127)
        {
            if(untouched) { editor.length=editor.cursor=0;out[0]=0;untouched=false; }
            else editor.backspace();
        }
        else if(c>=32 && c<127)
        {
            if(filename && !(isalnum(c)||c=='_'||c=='-')) continue;
            if(untouched) { editor.length=editor.cursor=0;out[0]=0;untouched=false; }
            char value=char(c);editor.insert(&value,1);
        }
    }
}
