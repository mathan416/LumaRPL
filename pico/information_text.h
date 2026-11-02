// Allocation-free text layout shared by native information and result pages.
#pragma once
#include "native_layout.h"
#include <algorithm>
namespace pico_information_text {
inline size_t glyph(const char *body,size_t length,size_t at,char (&text)[5])
{
    unsigned char c=body[at];size_t n=c>=0xc2&&c<=0xdf?2:c>=0xe0&&c<=0xef?3:c>=0xf0&&c<=0xf4?4:1;
    if(at+n>length||!pico_text_editor::valid_utf8(body+at,n)){text[0]='?';return 1;}
    memcpy(text,body+at,n);if(c=='\t')text[0]=' ';return n;
}
// Return last logical row for scrolling. Fixed cells preserve source/table spacing;
// proportional prose wraps whole words when they fit, and splits long tokens.
template<class Surface>
size_t draw(Surface &screen,font_p font,const char *body,size_t length,size_t scroll,
            int x,int y,int width,int step,size_t visible,int cell,typename Surface::pattern foreground)
{
    size_t row=0;int column=0;bool word_start=true,soft_break=false;
    for(size_t p=0;p<length;)
    {
        if(body[p]=='\r'){++p;continue;}
        if(body[p]=='\n'){++row;column=0;word_start=true;soft_break=false;++p;continue;}
        char text[5]={};size_t bytes=glyph(body,length,p,text);
        bool space=body[p]==' '||body[p]=='\t';
        if(!cell&&word_start&&!space&&column)
        {
            int word=0;
            for(size_t q=p;q<length&&body[q]!=' '&&body[q]!='\t'&&body[q]!='\n'&&body[q]!='\r'&&word<=width;)
            {char next[5]={};q+=glyph(body,length,q,next);word+=font->width(utf8(next));}
            if(word<=width&&column+word>width){++row;column=0;soft_break=true;}
        }
        int advance=std::max(cell,int(font->width(utf8(text))));
        if(column+advance>width){++row;column=0;soft_break=true;}
        if(!cell&&space&&soft_break&&!column){p+=bytes;word_start=true;continue;}
        if(advance&&row>=scroll&&row<scroll+visible)
            pico_native_layout::draw(screen,x+column,y+int(row-scroll)*step,std::min(advance,width-column),step,text,font,font,foreground);
        column+=advance;p+=bytes;word_start=space;soft_break=false;
    }
    return row;
}
}
