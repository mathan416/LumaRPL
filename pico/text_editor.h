#pragma once
#include <stddef.h>
#include <string.h>

// UTF-8 codepoint cursor, bounded insertion, and visual-row navigation.
struct pico_text_editor
{
    char *text;
    size_t capacity,length,cursor;
    bool dirty=false;
    static bool continuation(unsigned char c) { return (c&0xc0)==0x80; }
    static bool valid_utf8(const char *text,size_t length)
    {
        for(size_t p=0;p<length;)
        {
            unsigned char c=text[p++];
            if(c<128) continue;
            unsigned count=c>=0xc2 && c<=0xdf?1:c>=0xe0 && c<=0xef?2:c>=0xf0 && c<=0xf4?3:0;
            if(!count || count>length-p) return false;
            unsigned value=c&((1u<<(6-count))-1);
            for(unsigned n=0;n<count;++n)
            { unsigned char b=text[p++];if(!continuation(b)) return false;value=(value<<6)|(b&63); }
            if((count==1 && value<128)||(count==2 && value<2048)||(count==3 && value<65536)||
               value>0x10ffff || (value>=0xd800 && value<=0xdfff)) return false;
        }
        return true;
    }
    size_t next(size_t p) const
    { size_t limit=p+4;if(p<length) ++p;while(p<length && p<limit && continuation(text[p])) ++p;return p; }
    size_t previous(size_t p) const
    { if(p) --p;while(p && continuation(text[p])) --p;return p; }
    bool insert(const char *bytes,size_t count)
    {
        if(count>capacity-1-length) return false;
        memmove(text+cursor+count,text+cursor,length-cursor+1);
        memcpy(text+cursor,bytes,count);cursor+=count;length+=count;dirty=true;return true;
    }
    void backspace()
    {
        if(!cursor) return;
        size_t p=previous(cursor);
        memmove(text+p,text+cursor,length-cursor+1);
        length-=cursor-p;cursor=p;dirty=true;
    }
    void position(size_t p,size_t &row,size_t &column,size_t width=30) const
    {
        row=column=0;
        for(size_t i=0;i<p;i=next(i))
        {
            if(text[i]=='\n') { ++row;column=0; }
            else if(++column==width) { ++row;column=0; }
        }
    }
    void vertical(int direction,size_t width=30)
    {
        size_t row,col;position(cursor,row,col,width);
        if(direction<0 && row==0) return;
        size_t target=direction<0?row-1:row+1,best=cursor;
        bool found=false;size_t r=0,c=0;
        for(size_t p=0;;p=next(p))
        {
            if(r==target && c<=col) { best=p;found=true; }
            if(r>target || p==length) break;
            if(text[p]=='\n') { ++r;c=0; }
            else if(++c==width) { ++r;c=0; }
        }
        if(found) cursor=best;
    }
};
