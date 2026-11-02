// Shared drawing/measurement for Lua text; GPL-3.0.
#pragma once
#include "typography.h"
#include "information_text.h"
namespace pico_ui {
struct TextSize {int width=0,height=0;};
template<class Surface>
TextSize canvas_text(Surface &screen,bool paint,int profile,int x,int y,const char *text,size_t length,typename Surface::pattern colour,font_p legacy)
{
    font_p face=profile==2?legacy:compact();int cell=profile==1?pico_native_layout::compact_cell_width(face):0;
    int step=profile==2?face->height()+1:16,left=x,advance=0,lines=length?1:0;TextSize size;
    for(size_t p=0;p<length;)
    {
        unsigned char c=text[p];char glyph[5]={};size_t n=pico_information_text::glyph(text,length,p,glyph);p+=n;
        if(c=='\r')continue;
        if(c=='\n'){size.width=std::max(size.width,advance);advance=0;x=left;y+=step;++lines;continue;}
        if(!c)strcpy(glyph,"?");int width=std::max(cell,int(face->width(utf8(glyph))));
        if(paint)screen.text(x,y,utf8(glyph),face,colour);x+=width;advance+=width;
    }
    size.width=std::max(size.width,advance);size.height=lines*step;return size;
}
}
