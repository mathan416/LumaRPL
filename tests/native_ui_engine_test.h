// Execute the shared production widgets against the real engine and fonts.
#include "information_text.h"
#include "native_menu.h"
#include "editor_tools.h"
#include "source_style.h"
#include "ui/typography.h"
#include "ui/screen.h"
#include "ui/canvas_text.h"
#include <cctype>
#include <cmath>
namespace native_ui_test {
static blitter::pixword pixels[320*320*surface::BPP/32];
static surface Screen(pixels,320,320);
static const int *input;static size_t remaining;static unsigned errors;
static const char *capture=nullptr;
enum class source_language {rpl,assembly,lua};
static bool dirty_rows[320];static unsigned partial_refreshes,full_refreshes;
void mark_dirty(int,int y1,int,int y2){for(int y=std::max(0,y1);y<=y2&&y<320;++y)dirty_rows[y]=true;}
void lcd_refresh(){++partial_refreshes;}
void pico_ui_cursor_complete(){}
void key_pop_all(){}
int read_key_raw(){surface_check(remaining,"UI waits only for expected input");rt.gc();--remaining;return *input++;}
void flush_display()
{
    ++full_refreshes;if(!capture)return;
    FILE *f=fopen(capture,"wb");surface_check(f,"native UI preview file");
    fprintf(f,"P6\n320 320\n255\n");
    for(int y=0;y<320;++y)for(int x=0;x<320;++x)
    {auto c=Screen.pixel_color(x,y);unsigned char rgb[]={c.red(),c.green(),c.blue()};fwrite(rgb,1,3,f);}
    fclose(f);capture=nullptr;
}
int view_text(const char *,const char *,size_t,pico_ui::TextRole=pico_ui::TextRole::prose,pico_ui::Actions={{nullptr,nullptr,nullptr,nullptr,"Back"}});
#include "../pico/ui/shell.inc"
#include "../pico/ui/dialogs.inc"
#include "../pico/ui/editor.inc"
#include "../pico/ui/results.inc"
#include "../pico/ui/lua_output.inc"
inline void run()
{
    // Check contrast after the renderer's real RGB332 expansion, without rounding.
    auto luminance=[](pattern p){blitter::pattern<blitter::mode::RGB_8BPP> packed(p);blitter::color<blitter::RGB_8BPP> q(pixword(packed.bits&255));auto linear=[](double c){c/=255;return c<=0.04045?c/12.92:std::pow((c+0.055)/1.055,2.4);};return .2126*linear(q.red())+.7152*linear(q.green())+.0722*linear(q.blue());};
    double worst=100;
    for(unsigned c=0;c<256;++c)for(unsigned f=0;f<256;++f){blitter::color<blitter::RGB_8BPP> q{pixword(c)},ink{pixword(f)};pattern bg{blitter::color<blitter::RGB_16BPP>(q)};pattern fg=pico_ui::readable(pattern(blitter::color<blitter::RGB_16BPP>(ink)),bg);double a=luminance(fg),b=luminance(bg);if(a<b)std::swap(a,b);double ratio=(a+.05)/(b+.05);worst=std::min(worst,ratio);surface_check(ratio>=4.5,"quantized text contrast >=4.5:1");}
    printf("PASS: all 65536 RGB332 foreground/background pairs: minimum corrected text contrast %.6f:1\n",worst);
    for(unsigned c=0;c<256;++c)for(unsigned f=0;f<256;++f)
    {
        blitter::color<blitter::RGB_8BPP> q{pixword(c)},ink{pixword(f)};
        pattern bg{blitter::color<blitter::RGB_16BPP>(q)},requested{blitter::color<blitter::RGB_16BPP>(ink)};
        pattern drawn=pico_ui::plot_ink(requested,bg);double a=luminance(drawn),b=luminance(bg);if(a<b)std::swap(a,b);
        surface_check((a+.05)/(b+.05)>=3.0,"native curve remains visible after RGB332 conversion");
        if(pico_ui::plot_contrast(requested,bg))surface_check(drawn.bits==requested.bits,"readable curve colours retained exactly");
    }
    puts("PASS: all 65536 RGB332 native-curve/background pairs >=3:1; readable requested colours unchanged");
    Screen.fill(pattern::white);auto compact=pico_ui::canvas_text(Screen,false,0,0,0,"WiWi\n42",7,pattern::black,Settings.header_font());
    surface_check(compact.width==std::max(int(native_compact_font()->width(utf8("WiWi"))),int(native_compact_font()->width(utf8("42"))))&&compact.height==32,"Lua measure uses actual proportional advances and 16px lines");
    auto mono=pico_ui::canvas_text(Screen,false,1,0,0,"WiWi\n42",7,pattern::black,Settings.header_font());surface_check(mono.width==4*pico_native_layout::compact_cell_width(native_compact_font())&&mono.height==32,"Lua mono measure uses fixed cells");
    auto legacy=pico_ui::canvas_text(Screen,false,2,0,0,"ABC",3,pattern::black,Settings.header_font());surface_check(legacy.width==int(Settings.header_font()->width(utf8("ABC")))&&legacy.height==int(Settings.header_font()->height()+1),"Lua legacy reproduces header metrics");
    auto bounds=Screen.clip();Screen.clip(blitter::rect(4,4,35,35));pico_ui::canvas_text(Screen,true,0,-10,0,"Unicode αβ\nclipped output",strlen("Unicode αβ\nclipped output"),pattern::black,Settings.header_font());Screen.clip(bounds);
    for(int y=0;y<320;++y)for(int x=0;x<320;++x)if(x<4||x>35||y<4||y>35)surface_check(Screen.pixel_color(x,y).value==255,"Lua respects clipping at negative coordinates");
    puts("PASS: production Lua text metrics, proportional/mono/legacy, Unicode, multiline and clipped drawing");
    Screen.clip(blitter::rect(10,35,309,236));page("Clip recovery");surface_check(Screen.clip().x1==0&&Screen.clip().y2==319,"shared page restores full screen before chrome");
    const size_t depth=rt.depth();object_g value=integer::make(42);surface_check(value,"UI rooted value");
    const int cancel[]={27};input=cancel;remaining=1;capture="../reports/port184-layout-currency.ppm";
    surface_check(!value_result(value,"CAD to USD","100 CAD to USD\nRate date: 2026-10-30\nSource: Example rates (layout test)\nCached reference rates; fees excluded."),"result cancel returns");
    surface_check(rt.depth()==depth&&!remaining,"view/cancel preserves stack");
    const int copy[]={0x81,27};input=copy;remaining=2;
    value_result(value,"Result");surface_check(rt.depth()==depth+1&&rt.stack(0)==value,"explicit copy survives repeated GC");rt.drop();
    const int details[]={0x82,27,27};input=details;remaining=3;value_result(value,"Result");
    surface_check(rt.depth()==depth&&!remaining,"details Esc returns to result, then parent");
    const int plot[]={0x83};input=plot;remaining=1;
    surface_check(value_result(value,"Derivative","",true)&&rt.depth()==depth,"plot is explicit and preserves stack");
    char long_source[2500];memset(long_source,'A',sizeof long_source);text_g large=text::make(utf8(long_source),sizeof long_source);
    surface_check(large,"large result fixture");const int large_keys[]={0x81,0x82,27,27};input=large_keys;remaining=4;
    value_result(large,"Large result");surface_check(rt.depth()==depth+1&&rt.stack(0)==large&&!remaining,"large value details/copy retains the complete object");rt.drop();
    const int action_key[]={0x82};input=action_key;remaining=1;
    surface_check(view_text("Solver result","X = 42",6,pico_ui::TextRole::prose,pico_ui::Actions{{"Copy","Store",nullptr,nullptr,"Back"}})==2,"result action is available without leaving the page");
    const int enter[]={13};input=enter;remaining=1;capture="../reports/port184-layout-confirm.ppm";
    surface_check(!confirm("Discard unsaved notebook changes?"),"confirmation defaults to cancel");
    const int yes[]={0x81};input=yes;remaining=1;surface_check(confirm("Replace saved file?"),"F1 explicitly confirms");
    input=cancel;remaining=1;surface_check(!confirm("Replace saved file?"),"Esc cancels");
    const char *items[]={"Convert","Amount: 100","From: CAD","To: USD","Swap currencies","Rate details","Refresh rates","Wi-Fi settings","Update options","Back"};
    const int page[]={0xb7,0x81};input=page;remaining=2;capture="../reports/port184-layout-menu.ppm";
    surface_check(choose("Currency",items,10)==8&&!remaining,"menu page F1 opens highlighted row");
    pico_ui::MenuState retained;retained.selected=7;const int select_second_page[]={0xb7,0x81};input=select_second_page;remaining=2;
    surface_check(choose_impl("Pagination",items,10,retained)==8&&retained.selected==8,"page/selection retained after action");
    const int ignored_action[]={0x81,27};input=ignored_action;remaining=2;surface_check(!view_text("Read only","Text",4),"unavailable F1 does not activate an action");
    const int cancel_dirty[]={13};input=cancel_dirty;remaining=1;surface_check(unsaved_source()==0,"dirty source defaults to Cancel");
    const int discard_dirty[]={0x83};input=discard_dirty;remaining=1;surface_check(unsaved_source()==2,"dirty source explicit Discard");
    const int save_dirty[]={0x81};input=save_dirty;remaining=1;surface_check(unsaved_source()==1,"dirty source explicit Save");
    {unsigned first_full=full_refreshes,first_partial=partial_refreshes;const int moves[]={0xb6,0xb5,0x85};input=moves;remaining=3;pico_ui::MenuState state;choose_impl("Menu refresh",items,10,state);surface_check(full_refreshes==first_full+1&&partial_refreshes==first_partial+2,"menu arrows retain title and action bar; one body refresh per move");}
    {unsigned first_full=full_refreshes,first_partial=partial_refreshes;const int noop[]={0x81,0xb5,0x85};input=noop;remaining=3;view_text("No-op","Short text",10);surface_check(full_refreshes==first_full+1&&partial_refreshes==first_partial,"unavailable action and no-op reader scroll do not refresh");}
    {char entry[32]="";unsigned first_full=full_refreshes,first_partial=partial_refreshes;const int typing[]={'a','b',13};input=typing;remaining=3;surface_check(prompt("Input",entry,sizeof entry)&&!strcmp(entry,"ab"),"prompt applies typed text");surface_check(full_refreshes==first_full+1&&partial_refreshes==first_partial+2,"prompt typing retains static title and controls");}
    puts("PASS: native menu/reader preserve static controls on partial refresh and send no pixels for no-op keys");
    const char *prose="Long information paragraphs wrap at word boundaries, without stretching the spaces between letters.\n\nCached rates remain usable offline.\nInvalid UTF-8: \xff.\nLongTokenWithoutSpacesABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
    input=cancel;remaining=1;capture="../reports/port184-layout-prose.ppm";view_text("Information",prose,strlen(prose));
    const int output_keys[]={0x83,0x83,27};input=output_keys;remaining=3;capture="../reports/port185-layout-lua-output.ppm";
    lua_output_view("Lua finished","RECOVERY TEST\n42\n",17);surface_check(!remaining,"Lua normal/columns toggle and Esc");
    char source[512]="print(6 * 7)\nprint(42)\n",clipboard[257]={};pico_text_editor editor{source,sizeof source,strlen(source),0,false};pico_editor_tools tools(editor,clipboard);size_t scroll=0;
    render_editor(editor,"RECOVERY.lua",scroll,source_language::lua,tools);
    static blitter::pixword original_pixels[sizeof pixels/sizeof *pixels];memcpy(original_pixels,pixels,sizeof pixels);
    memset(dirty_rows,0,sizeof dirty_rows);unsigned full=full_refreshes,partial=partial_refreshes;
    surface_check(move_editor_cursor(editor,scroll,tools,0xb7)&&editor.cursor==1,"fast cursor advances one glyph");
    unsigned rows=0;for(int y=0;y<320;++y){rows+=dirty_rows[y];surface_check(!dirty_rows[y]||(y>=38&&y<=57)||(y>=266&&y<=285),"only caret/Ln/Col rows sent to LCD");}
    surface_check(rows==40&&full_refreshes==full&&partial_refreshes==partial+1,"one partial refresh, no full refresh");
    surface_check(move_editor_cursor(editor,scroll,tools,0xb4)&&!memcmp(original_pixels,pixels,sizeof pixels),"moving back restores all pixels exactly, including footer/hints");
    tools.anchor=0;surface_check(!move_editor_cursor(editor,scroll,tools,0xb7)&&editor.cursor==0,"selection uses full redraw path");tools.anchor=pico_editor_tools::none;
    partial=partial_refreshes;surface_check(move_editor_cursor(editor,scroll,tools,0xb4)&&partial_refreshes==partial,"boundary key sends no pixels");
    char multiline[64];for(int i=0;i<12;++i){multiline[2*i]='x';multiline[2*i+1]='\n';}multiline[24]=0;
    pico_text_editor tall{multiline,sizeof multiline,24,18,false};pico_editor_tools tall_tools(tall,clipboard);
    surface_check(!move_editor_cursor(tall,0,tall_tools,0xb6)&&tall.cursor==18,"scrolling defers to full redraw without moving twice");
    char unicode[16]="αβ";pico_text_editor utf8_editor{unicode,sizeof unicode,strlen(unicode),0,false};pico_editor_tools unicode_tools(utf8_editor,clipboard);
    render_editor(utf8_editor,"UTF8.lua",scroll,source_language::lua,unicode_tools);surface_check(move_editor_cursor(utf8_editor,scroll,unicode_tools,0xb7)&&utf8_editor.cursor==2,"fast cursor respects UTF-8 glyph boundary");
    puts("PASS: actual editor cursor fast path, exact pixel restoration, 40-row single refresh, static footer/tools hints, no-op boundaries, selection/scroll fallback and UTF-8; Lua proportional/columns toggle");
    surface_check(!errors&&rt.depth()==depth,"native widgets preserve stack and engine error state");
    puts("PASS: actual native result/reader/menu/confirmation widgets, explicit copy/plot, cancel/details navigation, repeated GC, safe confirmation and real 320x320 previews");
}
}

#undef choose
