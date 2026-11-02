#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
namespace pico_chip8 {
// Two-pass, line-oriented CHIP-8 assembler. No dynamic allocation.
struct assembler {
    struct label { char name[32];unsigned address; } labels[96];
    unsigned count=0,line=0;const char *error=nullptr;
    int reg(const char *s) const
    { if((s[0]=='V'||s[0]=='v')&&s[1]&&!s[2]){int n=std::tolower((unsigned char)s[1]);return n>='0'&&n<='9'?n-'0':n>='a'&&n<='f'?n-'a'+10:-1;}return -1; }
    int number(const char *s,int pass)
    {
        for(unsigned i=0;i<count;++i)if(!strcmp(s,labels[i].name))return labels[i].address;
        char *end;long value=strtol(s,&end,0);
        if(*s && !*end && value>=0 && value<=65535)return int(value);
        if(pass==0 && (std::isalpha((unsigned char)*s)||*s=='_'))return 0;
        error="Unknown label or invalid number";return -1;
    }
    bool assemble(const char *source,uint8_t *output,size_t capacity,size_t &size,unsigned target=~0u,unsigned *source_line=nullptr)
    {
        count=0;error=nullptr;size=0;if(source_line)*source_line=0;
        for(int pass=0;pass<2;++pass)
        {
            const char *p=source;unsigned pc=0x200;line=0;
            while(*p)
            {
                ++line;char text[256];size_t n=0;
                while(*p && *p!='\n'){if(n+1>=sizeof text){error="Source line is too long";return false;}text[n++]=*p++;}
                if(*p)++p;text[n]=0;
                char *comment=strpbrk(text,";#");if(comment)*comment=0;
                char *tokens[20];int nt=0;char *save=nullptr;
                for(char *t=strtok_r(text," \t\r,",&save);t;t=strtok_r(nullptr," \t\r,",&save))
                {if(nt==20){error="Too many operands";return false;}tokens[nt++]=t;}
                int first=0;if(!nt)continue;
                size_t len=strlen(tokens[0]);
                if(len && tokens[0][len-1]==':')
                {
                    tokens[0][len-1]=0;
                    if(pass==0)
                    {
                        if(count==96||len>32||len<2){error="Invalid label or label table full";return false;}
                        for(unsigned j=0;j<count;++j)if(!strcmp(labels[j].name,tokens[0])){error="Duplicate label";return false;}
                        snprintf(labels[count].name,32,"%s",tokens[0]);labels[count++].address=pc;
                    }
                    first=1;if(first==nt)continue;
                }
                char *op=tokens[first++];for(char *q=op;*q;++q)*q=std::toupper((unsigned char)*q);
                int argc=nt-first;char *a=argc>0?tokens[first]:const_cast<char *>("");char *b=argc>1?tokens[first+1]:const_cast<char *>("");
                int x=reg(a),y=reg(b),code=-1;
                auto value=[&](const char *t,int limit){int v=number(t,pass);if(v>limit){error="Operand out of range";return -1;}return v;};
                if(!strcmp(op,"DB")||!strcmp(op,"DW"))
                {
                    if(!argc){error="Data directive needs bytes";return false;}
                    unsigned width=!strcmp(op,"DB")?1:2;
                    for(int j=0;j<argc;++j)
                    {
                        int v=value(tokens[first+j],width==1?255:65535);if(v<0)return false;
                        if(pc-0x200+width>capacity || pc+width>4096){error="Program exceeds 3584 bytes";return false;}
                        if(pass==1&&source_line&&target>=pc&&target<pc+width)*source_line=line;
                        if(pass==1){if(width==2)output[pc++-0x200]=v>>8;output[pc++-0x200]=v;}
                        else pc+=width;
                    }
                    continue;
                }
                // Operand keywords are case-insensitive; label spelling is retained.
                auto eq=[](const char *l,const char *r){while(*l&&*r){if(std::toupper((unsigned char)*l++)!=*r++)return false;}return !*l&&!*r;};
                if(argc==0){if(eq(op,"CLS"))code=0xe0;else if(eq(op,"RET"))code=0xee;else if(eq(op,"EXIT"))code=0xfd;else if(eq(op,"LOW"))code=0xfe;else if(eq(op,"HIGH"))code=0xff;else if(eq(op,"SCR"))code=0xfb;else if(eq(op,"SCL"))code=0xfc;}
                if(argc==1)
                {
                    int v;
                    if(eq(op,"JP")||eq(op,"CALL")||eq(op,"JPOFF")){v=value(a,4095);if(v<0)return false;code=(eq(op,"JP")?0x1000:eq(op,"CALL")?0x2000:0xb000)|v;}
                    else if(eq(op,"SCD")){v=value(a,15);if(v<0)return false;code=0xc0|v;}
                    else if(x>=0){if(eq(op,"SKP"))code=0xe09e|(x<<8);else if(eq(op,"SKNP"))code=0xe0a1|(x<<8);else if(eq(op,"SHR"))code=0x8006|(x<<8)|(x<<4);else if(eq(op,"SHL"))code=0x800e|(x<<8)|(x<<4);else if(eq(op,"SAVE"))code=0xf055|(x<<8);else if(eq(op,"LOAD"))code=0xf065|(x<<8);else if(eq(op,"FLAGSAVE"))code=0xf075|(x<<8);else if(eq(op,"FLAGLOAD"))code=0xf085|(x<<8);}
                }
                if(argc==2)
                {
                    if(eq(op,"LD"))
                    {
                        if(x>=0){if(y>=0)code=0x8000|(x<<8)|(y<<4);else if(eq(b,"DT"))code=0xf007|(x<<8);else if(eq(b,"K"))code=0xf00a|(x<<8);else if(eq(b,"[I]"))code=0xf065|(x<<8);else {int v=value(b,255);if(v<0)return false;code=0x6000|(x<<8)|v;}}
                        else if(eq(a,"I")){int v=value(b,4095);if(v<0)return false;code=0xa000|v;}
                        else if(y>=0){int suffix=eq(a,"DT")?0x15:eq(a,"ST")?0x18:eq(a,"F")?0x29:eq(a,"HF")?0x30:eq(a,"B")?0x33:eq(a,"[I]")?0x55:-1;if(suffix>=0)code=0xf000|(y<<8)|suffix;}
                    }
                    else if(eq(op,"ADD") && eq(a,"I") && y>=0)code=0xf01e|(y<<8);
                    else if(x>=0)
                    {
                        if(eq(op,"RND")){int v=value(b,255);if(v<0)return false;code=0xc000|(x<<8)|v;}
                        else if(eq(op,"SE")||eq(op,"SNE")){if(y>=0)code=(eq(op,"SE")?0x5000:0x9000)|(x<<8)|(y<<4);else{int v=value(b,255);if(v<0)return false;code=(eq(op,"SE")?0x3000:0x4000)|(x<<8)|v;}}
                        else if(eq(op,"ADD") && y<0){int v=value(b,255);if(v<0)return false;code=0x7000|(x<<8)|v;}
                        else if(y>=0){int suffix=eq(op,"OR")?1:eq(op,"AND")?2:eq(op,"XOR")?3:eq(op,"ADD")?4:eq(op,"SUB")?5:eq(op,"SHR")?6:eq(op,"SUBN")?7:eq(op,"SHL")?14:-1;if(suffix>=0)code=0x8000|(x<<8)|(y<<4)|suffix;}
                    }
                }
                if(argc==3 && (eq(op,"DRW")||eq(op,"DRAW")) && x>=0 && y>=0)
                {int v=value(tokens[first+2],15);if(v<0)return false;code=0xd000|(x<<8)|(y<<4)|v;}
                if(error)return false;
                if(code<0){error="Unknown instruction or wrong operands";return false;}
                if(pc&1){error="Instruction must be word-aligned";return false;}
                if(pc-0x200+2>capacity || pc+2>4096){error="Program exceeds 3584 bytes";return false;}
                if(pass==1&&source_line&&target>=pc&&target<pc+2)*source_line=line;
                if(pass==1){output[pc-0x200]=code>>8;output[pc+1-0x200]=code;}
                pc+=2;
            }
            size=pc-0x200;
        }
        if(!size){error="Empty program";return false;}return true;
    }
};
}
