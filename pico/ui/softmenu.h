// Compact calculator soft-key labels; no framebuffer or persistent cache.
#pragma once
#include "typography.h"
#include "reference_font.h"
#include "utf8.h"

namespace pico_ui {
inline bool menu_glyphs(font_p face, utf8 text, size_t length)
{
    for (size_t at=0;at<length;at=utf8_next(text,at,length))
    {
        font::glyph_info glyph;
        if (!face->glyph(utf8_codepoint(text,at,length),glyph))return false;
    }
    return true;
}
inline font_p softmenu_font(utf8 text,size_t length,font_p original)
{
    if(menu_glyphs(compact(),text,length))return compact();
    if(menu_glyphs(reference_font(),text,length))return reference_font();
    return original;
}
struct MenuInk { int left=0,right=0,advance=0;int width()const{return right-left;} };
inline void menu_ink(MenuInk &ink,font_p face,utf8 text,size_t length)
{
    for(size_t at=0;at<length;at=utf8_next(text,at,length))
    {
        font::glyph_info glyph;
        if(face->glyph(utf8_codepoint(text,at,length),glyph))
        {
            ink.left=std::min(ink.left,ink.advance+glyph.x);
            ink.right=std::max(ink.right,ink.advance+glyph.x+int(glyph.w));
            ink.advance+=glyph.advance;
            ink.right=std::max(ink.right,ink.advance);
        }
    }
}
#ifdef PICOCALC_UI_HOST
inline void (*softmenu_observer)(utf8,size_t,font_p,int,int,int,int,bool)=nullptr;
#endif
}
