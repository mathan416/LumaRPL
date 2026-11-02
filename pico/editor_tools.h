#pragma once
#include "text_editor.h"
#include <stdint.h>
#ifdef PICOCALC
#include "bulk_storage.h"
#endif
struct pico_editor_tools {
    static constexpr size_t none=~size_t(0),limit=256,depth=8;
    struct change {size_t at,before,after,old_cursor,new_cursor;char removed[limit],inserted[limit];};
    pico_text_editor &e;char *clipboard;
#ifdef PICOCALC
    pico_bulk_records<change,depth> history;
#else
    change history[depth];
#endif
    size_t count=0,applied=0,anchor=none;bool merging=false;
    pico_editor_tools(pico_text_editor &editor,char *clip):e(editor),clipboard(clip){}
    bool ready()const
    {
#ifdef PICOCALC
        return history.good();
#else
        return true;
#endif
    }
    bool selected()const{return anchor!=none&&anchor!=e.cursor;}
    size_t first()const{return selected()?(anchor<e.cursor?anchor:e.cursor):e.cursor;}
    size_t last()const{return selected()?(anchor>e.cursor?anchor:e.cursor):e.cursor;}
    void break_group(){merging=false;}
    void raw(size_t at,size_t removed,const char *bytes,size_t length)
    {
        memmove(e.text+at+length,e.text+at+removed,e.length-at-removed+1);
        if(length)memcpy(e.text+at,bytes,length);e.length=e.length-removed+length;e.cursor=at+length;e.dirty=true;anchor=none;
    }
    bool replace(size_t at,size_t end,const char *bytes,size_t length,bool group=false)
    {
        if(!ready()||at>end||end>e.length||end-at>limit||length>limit||length>e.capacity-1-(e.length-(end-at)))return false;
        bool merge=group&&merging&&applied==count&&count&&at==end&&history[count-1].before==0&&history[count-1].at+history[count-1].after==at&&history[count-1].after+length<=limit;
        if(merge)
        {
            change &c=history[count-1];if(!ready())return false;memcpy(c.inserted+c.after,bytes,length);size_t old=c.after;c.after+=length;raw(at,0,c.inserted+old,length);c.new_cursor=e.cursor;
        }
        else
        {
            count=applied;if(count==depth){
#ifdef PICOCALC
history.drop_first();
#else
memmove(history,history+1,(depth-1)*sizeof(change));
#endif
--count;}
            change &c=history[count++];if(!ready())return false;c.at=at;c.before=end-at;c.after=length;c.old_cursor=e.cursor;
            memcpy(c.removed,e.text+at,c.before);if(length)memcpy(c.inserted,bytes,length);
            raw(at,c.before,c.inserted,length);c.new_cursor=e.cursor;applied=count;
        }
        merging=group;return true;
    }
    bool insert(const char *s,size_t n,bool group=false){return replace(first(),last(),s,n,group);}
    bool backspace(){if(selected())return insert("",0);if(!e.cursor)return true;return replace(e.previous(e.cursor),e.cursor,"",0);}
    bool undo()
    {break_group();if(!ready()||!applied)return false;change &c=history[--applied];if(!ready()||c.at>e.length||c.after>e.length-c.at||c.before>limit||c.after>limit||c.before>e.capacity-1-(e.length-c.after))return false;raw(c.at,c.after,c.removed,c.before);e.cursor=c.old_cursor;return true;}
    bool redo()
    {break_group();if(!ready()||applied==count)return false;change &c=history[applied++];if(!ready()||c.at>e.length||c.before>e.length-c.at||c.before>limit||c.after>limit||c.after>e.capacity-1-(e.length-c.before))return false;raw(c.at,c.before,c.inserted,c.after);e.cursor=c.new_cursor;return true;}
    size_t line_start()const{size_t p=e.cursor;while(p&&e.text[p-1]!='\n')--p;return p;}
    bool copy(bool cut=false)
    {
        size_t a=first(),b=last();if(!selected()){a=line_start();b=a;while(b<e.length&&e.text[b]!='\n')++b;if(cut&&b<e.length)++b;}
        if(b-a>limit)return false;memcpy(clipboard,e.text+a,b-a);clipboard[b-a]=0;
        return !cut||replace(a,b,"",0);
    }
    bool newline()
    {
        char text[66]={'\n'};size_t p=line_start(),n=1;
        while(p<e.cursor && e.text[p]==' '&&n<65){text[n++]=' ';++p;}return insert(text,n);
    }
    bool unindent()
    {
        size_t a=line_start(),b=a,cursor=e.cursor;while(b<e.length&&b<a+2&&e.text[b]==' ')++b;
        if(a==b)return true;if(!replace(a,b,"",0))return false;e.cursor=cursor>=b?cursor-(b-a):a;history[applied-1].new_cursor=e.cursor;return true;
    }
    bool find(const char *needle)
    {
        if(!*needle)return false;const char *p=strstr(e.text+e.cursor,needle);if(!p)p=strstr(e.text,needle);
        if(!p)return false;anchor=p-e.text;e.cursor=anchor+strlen(needle);break_group();return true;
    }
    int replace_all(const char *needle,const char *replacement)
    {
        size_t a=strlen(needle),b=strlen(replacement);if(!a||a>limit||b>limit)return -1;
        size_t matches=0;for(const char *p=e.text;(p=strstr(p,needle));p+=a)++matches;
        int64_t total=int64_t(e.length)+int64_t(matches)*(int64_t(b)-int64_t(a));
        if(total<0||uint64_t(total)>=e.capacity)return -1;
        size_t pos=0;for(size_t n=0;n<matches;++n)
        {const char *p=strstr(e.text+pos,needle);size_t at=p-e.text;if(!replace(at,at+a,replacement,b))return -1;pos=at+b;}
        return int(matches);
    }
};
