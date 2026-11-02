#pragma once
#include "graph_scene.h"
#include <cstdio>
#include <cstring>
// Saved notebook contents are independent of the volatile calculation previews.
struct pico_notebook
{
    static constexpr unsigned capacity=12;
    static constexpr size_t bytes=8+48+4+capacity*(160+96)+4;
    enum class state : unsigned char { not_run, complete, error, cancelled };
    struct cell
    {
        char expression[160]="0";
        char note[96]="";
        char preview[128]="";
        state status=state::not_run;
    };
    char title[48]="Calculation notebook";
    unsigned count=3;
    cell cells[capacity];
    bool dirty=false;
    pico_notebook()
    {
        strcpy(cells[0].expression,"2+3");strcpy(cells[0].note,"First result is R1");
        strcpy(cells[1].expression,"R1^2");strcpy(cells[1].note,"Square the first result");
        strcpy(cells[2].expression,"R2/3");strcpy(cells[2].note,"Exact fraction when exact arithmetic is enabled");
    }
    static bool text(const char *source,size_t capacity,bool multiline=false)
    {
        const char *end=static_cast<const char *>(memchr(source,0,capacity));if(!end)return false;
        for(const char *p=source;p<end;++p)if((static_cast<unsigned char>(*p)<32&&!(multiline&&(*p=='\n'||*p=='\t')))||*p==127)return false;
        return pico_text_editor::valid_utf8(source,size_t(end-source));
    }
    bool valid()const
    {
        if(!text(title,sizeof title)||!*title||count<1||count>capacity)return false;
        for(unsigned i=0;i<capacity;++i)
            if(!pico_graph_scene::formula(cells[i].expression,i>=count)||!text(cells[i].note,sizeof cells[i].note,true))return false;
        return true;
    }
    void clear_results(unsigned first=0)
    {for(unsigned i=first;i<capacity;++i){cells[i].preview[0]=0;cells[i].status=state::not_run;}}
    void invalidate(unsigned first=0){clear_results(first);dirty=true;}
    bool set_expression(unsigned index,const char *source)
    {
        if(index>=count||strnlen(source,160)>=160||!pico_graph_scene::formula(source,false))return false;
        snprintf(cells[index].expression,160,"%s",source);invalidate(index);return true;
    }
    bool set_note(unsigned index,const char *source)
    {
        if(index>=count||strnlen(source,96)>=96||!text(source,strlen(source)+1,true))return false;
        snprintf(cells[index].note,96,"%s",source);dirty=true;return true;
    }
    bool append()
    {if(count==capacity)return false;cells[count]=cell{};++count;invalidate(count-1);return true;}
    bool erase(unsigned index)
    {
        if(index>=count||count==1)return false;
        for(unsigned i=index;i+1<count;++i)cells[i]=cells[i+1];cells[--count]=cell{};invalidate(index);return true;
    }
    static bool identifier_start(unsigned char c){return c>='A'&&c<='Z'||c>='a'&&c<='z'||c=='_'||c>=128;}
    static bool identifier_part(unsigned char c){return identifier_start(c)||c>='0'&&c<='9';}
    // Result symbols are reserved: a future Rn must never read an unrelated
    // calculator variable with the same name. Quoted text is not a reference.
    static bool prior_results_only(const char *source,unsigned index,bool ignore_case=true)
    {
        for(size_t p=0;source[p];)
        {
            if(source[p]=='"'){++p;while(source[p]&&source[p]!='"'){if(source[p]=='\\'&&source[p+1])++p;++p;}if(source[p])++p;continue;}
            if(!identifier_start(static_cast<unsigned char>(source[p]))){++p;continue;}
            size_t start=p++;while(identifier_part(static_cast<unsigned char>(source[p])))++p;
            if(source[start]!='R'&&!(ignore_case&&source[start]=='r'))continue;
            unsigned number=0;bool digits=p>start+1;
            for(size_t n=start+1;n<p;++n){if(source[n]<'0'||source[n]>'9'){digits=false;break;}if(number<=capacity)number=number*10+unsigned(source[n]-'0');}
            if(digits&&number>=1&&number<=capacity&&number>index)return false;
        }
        return true;
    }
    size_t encode(uint8_t *out,size_t capacity)const
    {
        if(capacity<bytes||!valid())return 0;size_t at=0;
        auto word=[&](uint32_t n){for(int i=0;i<4;++i)out[at++]=uint8_t(n>>(8*i));};
        word(0x3133424e);word(1);memcpy(out+at,title,48);at+=48;word(count);
        for(const auto &c:cells){memcpy(out+at,c.expression,160);at+=160;memcpy(out+at,c.note,96);at+=96;}
        word(pico_graph_scene::checksum(out,at));return at;
    }
    bool decode(const uint8_t *data,size_t length)
    {
        if(length!=bytes)return false;size_t at=0;
        auto word=[&](){uint32_t n=0;for(int i=0;i<4;++i)n|=uint32_t(data[at++])<<(i*8);return n;};
        if(word()!=0x3133424e||word()!=1)return false;
        const char *new_title=reinterpret_cast<const char *>(data+at);at+=48;unsigned new_count=word();
        if(!text(new_title,48)||!*new_title||new_count<1||new_count>capacity)return false;
        for(unsigned i=0;i<capacity;++i)
        {
            const char *expression=reinterpret_cast<const char *>(data+at);at+=160;
            const char *note=reinterpret_cast<const char *>(data+at);at+=96;
            if(!pico_graph_scene::formula(expression,i>=new_count)||!text(note,96,true))return false;
        }
        uint32_t checksum=pico_graph_scene::checksum(data,at);if(word()!=checksum)return false;
        memcpy(title,new_title,48);count=new_count;at=60;
        for(auto &c:cells){memcpy(c.expression,data+at,160);at+=160;memcpy(c.note,data+at,96);at+=96;}
        clear_results();dirty=false;return true;
    }
};
