// Coordinated RGB332 display palettes. GPL-3.0.
#pragma once
#include "blitter.h"
namespace pico_themes {
struct Theme {
    const char *name,*description;
    unsigned background,foreground,header,selection,accent,result;
};
inline constexpr Theme themes[]={
    {"Classic light","White paper, dark ink and blue controls.",0xffffff,0x101820,0x183050,0x204c90,0x005090,0x004000},
    {"Midnight blue","Deep blue-black with cool text and mint results.",0x0f1623,0xe0e8f5,0x182c50,0x2a466e,0x60c0ff,0x64ebaf},
    {"Amber terminal","Black with warm amber text and bronze controls.",0x181208,0xffd064,0x483000,0x664400,0xffd064,0xffd064},
    {"Forest","Deep green with pale mint text and green controls.",0x082c10,0xd8ffe0,0x103c20,0x205830,0x80ffac,0xa0ffc0},
    {"Paper","Warm ivory with dark brown ink and burgundy controls.",0xffedaa,0x281808,0x503020,0x683020,0x683020,0x204020},
    {"Ocean","Deep teal with pale cyan text and turquoise controls.",0x002c48,0xd0ffff,0x005c48,0x007c68,0x60e8ff,0x80ffd0},
    {"Graphite","Neutral black and grey with bright white text.",0x000000,0xf0f0f0,0x484855,0x6c6c55,0xd8d8d8,0xffffff},
    {"Plum","Deep purple with soft lilac text and violet controls.",0x240048,0xffe0ff,0x48006c,0x6c246c,0xffa0ff,0xe0c0ff}
};
inline constexpr unsigned count=sizeof themes/sizeof *themes;
inline const Theme &get(int index){return themes[index>=0&&unsigned(index)<count?index:0];}
inline pattern colour(unsigned rgb){return pattern((rgb>>16)&255,(rgb>>8)&255,rgb&255);}
}
