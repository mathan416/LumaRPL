#pragma once
#include "graph_scene.h"
#include <cstdint>
#include <cstring>

struct pico_form_document
{
    static constexpr unsigned maximum_inputs=6;
    struct field {char name[13]{};char input[64]{};char units[32]{};};
    char title[48]="Ohm's law";
    char formula[160]="VOLTAGE/RESISTANCE";
    unsigned count=2;
    field fields[maximum_inputs]={ {"VOLTAGE","12"}, {"RESISTANCE","4"} };
    char output_units[32]="";
    bool dirty=false;
    static constexpr size_t legacy_bytes=686,bytes=910;
    static bool text(const char *source,size_t capacity,bool empty=false)
    {
        const char *end=static_cast<const char *>(memchr(source,0,capacity));
        if(!end||(!empty&&end==source))return false;
        for(const char *p=source;p<end;++p)if(static_cast<unsigned char>(*p)<32||*p==127)return false;
        return pico_text_editor::valid_utf8(source,end-source);
    }
    static bool identifier(const char *name)
    {
        size_t n=strnlen(name,13);if(!n||n>12)return false;
        if(!((name[0]>='A'&&name[0]<='Z')||(name[0]>='a'&&name[0]<='z')))return false;
        for(size_t i=1;i<n;++i)if(!((name[i]>='A'&&name[i]<='Z')||(name[i]>='a'&&name[i]<='z')||(name[i]>='0'&&name[i]<='9')||name[i]=='_'))return false;
        return true;
    }
    static bool same_name(const char *a,const char *b)
    {
        for(;;++a,++b){char x=*a,y=*b;if(x>='a'&&x<='z')x-=32;if(y>='a'&&y<='z')y-=32;if(x!=y)return false;if(!x)return true;}
    }
    bool valid()const
    {
        if(count>maximum_inputs||!text(title,sizeof title)||!pico_graph_scene::formula(formula,false)||!text(output_units,sizeof output_units,true)||strchr(output_units,'\''))return false;
        for(unsigned i=0;i<count;++i)
        {
            if(!identifier(fields[i].name)||!text(fields[i].input,sizeof fields[i].input)||strchr(fields[i].input,'\'')||!text(fields[i].units,sizeof fields[i].units,true)||strchr(fields[i].units,'\''))return false;
            for(unsigned j=0;j<i;++j)if(same_name(fields[i].name,fields[j].name))return false;
        }
        return true;
    }
    void circle()
    {
        *this=pico_form_document();strcpy(title,"Circle area");strcpy(formula,"PI*RADIUS^2");count=1;memset(fields,0,sizeof fields);strcpy(fields[0].name,"RADIUS");strcpy(fields[0].input,"3");dirty=true;
    }
    size_t encode(uint8_t *out,size_t capacity)const
    {
        if(capacity<bytes||!valid())return 0;size_t at=0;
        auto word=[&](uint32_t n){for(int i=0;i<4;++i)out[at++]=uint8_t(n>>(i*8));};
        word(0x32314d46);word(2);memcpy(out+at,title,sizeof title);at+=sizeof title;memcpy(out+at,formula,sizeof formula);at+=sizeof formula;word(count);
        for(const auto &f:fields){memcpy(out+at,f.name,sizeof f.name);at+=sizeof f.name;memcpy(out+at,f.input,sizeof f.input);at+=sizeof f.input;memcpy(out+at,f.units,sizeof f.units);at+=sizeof f.units;}
        memcpy(out+at,output_units,sizeof output_units);at+=sizeof output_units;
        uint32_t hash=pico_graph_scene::checksum(out,at);word(hash);return at;
    }
    bool decode(const uint8_t *data,size_t length)
    {
        if(length!=bytes&&length!=legacy_bytes)return false;size_t at=0;
        auto word=[&](){uint32_t n=0;for(int i=0;i<4;++i)n|=uint32_t(data[at++])<<(i*8);return n;};
        if(word()!=0x32314d46)return false;uint32_t version=word();if((version!=1||length!=legacy_bytes)&&(version!=2||length!=bytes))return false;pico_form_document next;
        memcpy(next.title,data+at,sizeof title);at+=sizeof title;memcpy(next.formula,data+at,sizeof formula);at+=sizeof formula;next.count=word();
        for(auto &f:next.fields){memcpy(f.name,data+at,sizeof f.name);at+=sizeof f.name;memcpy(f.input,data+at,sizeof f.input);at+=sizeof f.input;if(version==2){memcpy(f.units,data+at,sizeof f.units);at+=sizeof f.units;}}
        if(version==2){memcpy(next.output_units,data+at,sizeof output_units);at+=sizeof output_units;}
        uint32_t hash=pico_graph_scene::checksum(data,at);if(word()!=hash||!next.valid())return false;next.dirty=false;*this=next;return true;
    }
};
