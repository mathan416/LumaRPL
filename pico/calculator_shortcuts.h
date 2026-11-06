#pragma once
#include "input.h"
// Native editors consume raw keys in their own loops. Only the calculator's
// event dispatcher uses these translated Ctrl shortcuts.
template<class Interface,class Open>
bool pico_calculator_menu_shortcut(Interface &ui,int key,Open open)
{
    if(key!=PICO_CATALOG&&key!=PICO_RPL_SETTINGS&&key!=PICO_BASES&&key!=PICO_MATH_MENU)return false;
    if(!ui.showing_help()&&!ui.showing_graphics())open(key==PICO_MATH_MENU?3:key==PICO_BASES?2:key==PICO_RPL_SETTINGS?1:0);
    return true;
}

inline bool pico_calculator_shortcut_full_redraw(int key)
{
    return key!=PICO_LEFT && key!=PICO_RIGHT;
}

template<class Interface>
bool pico_calculator_cursor_shortcut(Interface &ui,int key)
{
    if(pico_calculator_shortcut_full_redraw(key))return false;
    if(rt.editing())
    {
        unsigned pos=ui.cursor_position();
        pos=key==PICO_LEFT?utf8_previous(rt.editor(),pos)
            :utf8_next(rt.editor(),pos,rt.editing());
        ui.cursor_position(pos);
        ui.update_mode();
    }
    return true;
}
