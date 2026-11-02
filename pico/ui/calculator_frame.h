// Calculator composition shared by the Pico loop and transition regression tests.
#pragma once
#include "platform.h"
#include "target.h"
#include "user_interface.h"
inline void pico_draw_calculator_frame(bool force)
{
    bool replaced=pico_ui_calculator_redraw();
    Screen.clip(Screen.area());
    if(replaced)pico_keyboard_transition();
    if(force||replaced){Screen.fill(pattern(Settings.Background()));for(unsigned y=0;y<LCD_H;++y)pico_mark_dirty(y);}
    ui.draw_start(force||replaced);
    ui.draw_header();ui.draw_battery();ui.draw_annunciators();ui.draw_menus();
    if(!ui.draw_help())
    {
        ui.draw_editor();ui.draw_cursor(true,ui.cursor_position());ui.draw_stack();
        if(!ui.draw_stepping_object())ui.draw_command();
    }
    ui.draw_error();
}
