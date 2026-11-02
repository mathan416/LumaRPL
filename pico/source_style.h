#pragma once
#include <stddef.h>
#include <ctype.h>
#include <string.h>
struct pico_source_style {
    enum kind { plain,comment,string,number,keyword,delimiter };
    enum language { rpl,assembly,lua };
    language mode;bool in_comment=false,in_string=false,escaped=false;unsigned char quote=0;
    explicit pico_source_style(language lang=rpl):mode(lang){}
    kind next(unsigned char c,unsigned char following=0)
    {
        if(c=='\n'){in_comment=false;escaped=false;return plain;}
        if(in_comment)return comment;
        if(in_string){if(c==quote&&!escaped){in_string=false;quote=0;}bool old=escaped;escaped=c=='\\'&&!old;return string;}
        if(c=='"'||(mode==lua&&c=='\'')){in_string=true;quote=c;escaped=false;return string;}
        if((mode==rpl&&c=='@')||(mode==assembly&&(c==';'||c=='#'))||(mode==lua&&c=='-'&&following=='-')){in_comment=true;return comment;}
        if(c=='\''||c=='['||c==']'||c=='{'||c=='}'||c==0xc2)return delimiter;
        if(isdigit(c)||c=='.')return number;
        if(mode!=lua&&c>='A'&&c<='Z')return keyword;
        return plain;
    }
    static bool lua_keyword(const char *word,size_t length)
    {
        const char *words[]={"and","break","do","else","elseif","end","false","for","function","goto","if","in","local","nil","not","or","repeat","return","then","true","until","while"};
        for(auto s:words)if(strlen(s)==length&&!memcmp(s,word,length))return true;return false;
    }
};
