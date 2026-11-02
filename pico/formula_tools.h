#pragma once
#include "text_editor.h"
// Bounded insertion of a template, leaving the cursor at the first argument.
inline bool pico_formula_insert(pico_text_editor &editor,const char *before,const char *after,bool replace)
{
    size_t prefix=strlen(before),suffix=strlen(after),old=replace?0:editor.length;
    if(prefix+suffix>editor.capacity-1-old)return false;
    if(replace){editor.length=editor.cursor=0;editor.text[0]=0;}
    size_t cursor=editor.cursor;editor.insert(before,prefix);editor.insert(after,suffix);editor.cursor=cursor+prefix;return true;
}

inline void pico_formula_next_slot(pico_text_editor &editor)
{
    for(size_t n=editor.cursor;n+1<editor.length;++n)if(editor.text[n]=='('&&editor.text[n+1]==')'){editor.cursor=n+1;return;}
    for(size_t n=0;n<editor.cursor&&n+1<editor.length;++n)if(editor.text[n]=='('&&editor.text[n+1]==')'){editor.cursor=n+1;return;}
    editor.cursor=editor.length;
}
inline void pico_formula_label(char *destination,size_t capacity,const char *source)
{
    if(!capacity)return;size_t n=strlen(source);if(n>=capacity)n=capacity-1;
    while(n&&!pico_text_editor::valid_utf8(source,n))--n;
    memcpy(destination,source,n);destination[n]=0;
}
