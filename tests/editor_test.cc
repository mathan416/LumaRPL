#include "text_editor.h"
#include <cassert>
#include <cstring>
#include <cstdio>
int main()
{
    char buffer[32]="abc";pico_text_editor e{buffer,sizeof buffer,3,1};
    assert(e.insert("«",2));assert(!strcmp(buffer,"a«bc"));
    assert(e.cursor==3);e.backspace();assert(!strcmp(buffer,"abc") && e.cursor==1);
    e.cursor=e.length;assert(e.next(e.cursor)==e.length);
    assert(!e.insert("012345678901234567890123456789",30));assert(!strcmp(buffer,"abc"));
    strcpy(buffer,"ab\ncdef\ng");e.length=strlen(buffer);e.cursor=5;
    e.vertical(-1);assert(e.cursor==2);
    e.vertical(1);assert(e.cursor==5);
    e.vertical(1);assert(e.cursor==9);
    e.vertical(-1);assert(e.cursor==4); // preserve current column, clamped on short row
    strcpy(buffer,"0123456789«xyz");e.length=strlen(buffer);e.cursor=12;
    size_t row,col;e.position(e.cursor,row,col,10);assert(row==1 && col==1);
    e.vertical(-1,10);assert(e.cursor==1);
    assert(pico_text_editor::valid_utf8("« DUP * »",11));
    assert(!pico_text_editor::valid_utf8("\xc0\xaf",2));
    assert(!pico_text_editor::valid_utf8("\xed\xa0\x80",3));
    assert(!pico_text_editor::valid_utf8("\xf4\x90\x80\x80",4));
    assert(!pico_text_editor::valid_utf8("\xc2",1));
    assert(!pico_text_editor::valid_utf8("\x80",1));
    puts("PASS: UTF-8 editor insert/delete, bounds, wrapped navigation and malformed input");
}
