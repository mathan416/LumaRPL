#pragma once
#include "font.h"
#include "blitter.h"
#include "text_editor.h"
#include <cstring>
namespace pico_native_layout {
constexpr int width=320,height=320,footer_top=289,footer_text=297;
constexpr int graph_formula=239,graph_values=255,graph_status=273,graph_controls=303,graph_footer_top=273;
constexpr int compact_height=13;
constexpr int information_top=37,information_row_step=16,information_rows=15;
// Keep space-padded numerical tables, matrices and Lua output aligned even
// though the compact face itself has proportional glyph advances.
inline int compact_cell_width(font_p font)
{
    int width=1;
    for(char c=32;c<127;++c)
    {
        char text[]={c,0};int advance=font->width(utf8(text));
        if(advance>width)width=advance;
    }
    return width;
}
// Keep a complete UTF-8 prefix and mark elision explicitly.
inline font_p fit(char *out,size_t capacity,const char *text,int pixels,int rows,font_p preferred,font_p compact)
{
    if(!capacity)return compact;size_t n=strlen(text);if(n>=capacity)n=capacity-1;
    while(n&&!pico_text_editor::valid_utf8(text,n))--n;memcpy(out,text,n);out[n]=0;
    font_p chosen=preferred;
    if(int(chosen->height())>rows||int(chosen->width(utf8(out)))>pixels)chosen=compact;
    bool shortened=n<strlen(text);
    while(n&&int(chosen->width(utf8(out)))+(shortened?int(chosen->width(utf8("..."))):0)>pixels)
    {
        do{--n;}while(n&&(static_cast<unsigned char>(out[n])&0xc0)==0x80);out[n]=0;shortened=true;
    }
    if(shortened&&n+3<capacity&&int(chosen->width(utf8("...")))<=pixels)strcat(out,"...");
    return chosen;
}
template<class Surface>
inline void draw(Surface &screen,int x,int y,int width,int height,const char *text,font_p preferred,font_p compact,typename Surface::pattern foreground)
{
    char fitted[320];font_p used=fit(fitted,sizeof fitted,text,width,height,preferred,compact);
    auto previous=screen.clip();screen.clip(blitter::rect(x,y,x+width-1,y+height-1));screen.text(x,y,utf8(fitted),used,foreground);screen.clip(previous);
}

}
