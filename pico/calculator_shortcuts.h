#pragma once
#include "input.h"
// Native editors consume raw keys in their own loops. Only the calculator's
// event dispatcher uses these translated Ctrl shortcuts.
template<class Interface,class Open>
bool pico_calculator_menu_shortcut(Interface &ui,int key,Open open)
{
    if(key!=PICO_CATALOG&&key!=PICO_RPL_SETTINGS&&key!=PICO_BASES)return false;
    if(!ui.showing_help()&&!ui.showing_graphics())open(key==PICO_BASES?2:key==PICO_RPL_SETTINGS?1:0);
    return true;
}
