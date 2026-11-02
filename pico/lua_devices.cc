#include "lua_device.h"
#include "lua_canvas.h"
#include "sprite_file.h"
#include "lua_runner.h"
#include "lua_input.h"
#include "platform.h"
#include "target.h"
#include "user_interface.h"
#include "settings.h"
#include "dmcp.h"
#include "text_editor.h"
#include "ui/canvas_text.h"
#include "ui/screen.h"
#include "pico/stdlib.h"
#include <cstring>
namespace {
pico_lua_input input;
rect previous_clip;
void pump(){
#ifndef PICOCALC_UI_HOST
    pico_poll_background();
#endif
    int state=0,code=pico_keyboard_event(&state);if(code>=0)input.feed(code,state);}

}
extern "C" {
void pico_lua_device_begin(void){previous_clip=Screen.clip();pico_lua_device_reset();}
void pico_lua_device_reset(void){input=pico_lua_input{};pico_keyboard_reset();key_pop_all();}
void pico_lua_device_end(void){stop_buzzer();Screen.clip(previous_clip);ui.show_graphics(false);pico_keyboard_reset();key_pop_all();}
void pico_lua_draw(int op,const int *a,uint32_t rgb,const char *text,size_t length)
{
    pattern c((rgb>>16)&255,(rgb>>8)&255,rgb&255);Screen.clip(Screen.area());ui.show_graphics(true);
    if(pico_lua_canvas(Screen,op,a,c))return;
    switch(op)
    {
    case PICO_LUA_TEXT:pico_ui::canvas_text(Screen,true,a[5],a[0],a[1],text,length,c,Settings.header_font());break;
    case PICO_LUA_SHOW:{pico_ui::ScreenScope screen_scope("lua.canvas");pico_ui::seen("Lua canvas");for(unsigned y=0;y<320;++y)pico_mark_dirty(y);lcd_refresh();pump();break;}
    }
}
int pico_lua_sprite(const char *name,int x,int y)
{
    char path[100];if(!pico_sprite::path(name,path,sizeof path))return 0;
    pico_sprite sprite;bool backup=false;if(!pico_sprite_load(path,sprite,backup))return 0;
    Screen.clip(Screen.area());ui.show_graphics(true);
    for(unsigned i=0;i<256;++i)if(sprite.opaque(i)){int px=x+int(i%16),py=y+int(i/16);if(px>=0&&px<320&&py>=0&&py<320){uint32_t c=pico_sprite::rgb(sprite.pixels[i]);Screen.fill(px,py,px,py,pattern(c>>16,(c>>8)&255,c&255));}}
    return 1;
}
int pico_lua_cancel(void){pump();return input.cancelled;}
void pico_lua_text_measure(int profile,const char *text,size_t length,int *width,int *height)
{auto size=pico_ui::canvas_text(Screen,false,profile,0,0,text,length,pattern::black,Settings.header_font());*width=size.width;*height=size.height;}
int pico_lua_key_poll(void){pump();return input.poll();}
int pico_lua_key_down(int code){pump();return input.held(code);}
uint32_t pico_lua_millis(void){return to_ms_since_boot(get_absolute_time());}
int pico_lua_sleep(unsigned ms)
{
    uint32_t start=pico_lua_millis();
    do{pump();if(input.cancelled)return 1;if(!ms)break;sleep_ms(1);}while(uint32_t(pico_lua_millis()-start)<ms);
    return input.cancelled;
}
int pico_lua_beep(unsigned frequency,unsigned ms)
{if(frequency)start_buzzer_freq(frequency*1000);else stop_buzzer();int cancelled=pico_lua_sleep(ms);stop_buzzer();return cancelled;}
}
