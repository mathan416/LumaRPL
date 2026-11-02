#pragma once
#include <stddef.h>
#include <stdint.h>
#include <cstring>
#include <cstdlib>
// Bounded streaming HTTP/1 response framing; no allocation and no compression.
struct pico_http_response
{
    char header[1536]{};char *body;size_t capacity,headers=0,used=0,total=0,remaining=0;
    bool parsed=false,chunked=false,sized=false,complete=false,failed=false,trailers=false;
    int state=0;char line[128]{};size_t line_used=0;
    pico_http_response(char *out,size_t size):body(out),capacity(size){if(size)*body=0;else failed=true;}
    static bool same(const char *a,const char *b)
    {while(*a&&*b){char x=*a++,y=*b++;if(x>='A'&&x<='Z')x+=32;if(y>='A'&&y<='Z')y+=32;if(x!=y)return false;}return !*a&&!*b;}
    bool parse_headers()
    {
        if(strncmp(header,"HTTP/1.1 200 ",13)&&strncmp(header,"HTTP/1.0 200 ",13))return false;
        char *at=strstr(header,"\r\n");if(!at)return false;at+=2;bool transfer=false;
        while(*at&&strncmp(at,"\r\n",2))
        {
            char *end=strstr(at,"\r\n"),*colon=strchr(at,':');if(!end||!colon||colon>=end)return false;*end=0;*colon=0;char *value=colon+1;while(*value==' '||*value=='\t')++value;
            char *last=end;while(last>value&&(last[-1]==' '||last[-1]=='\t'))*--last=0;
            if(same(at,"Content-Length"))
            {
                if(sized||!*value)return false;size_t n=0;for(char *p=value;*p;++p){if(*p<'0'||*p>'9'||n>capacity)return false;n=n*10+(*p-'0');}if(n>=capacity)return false;sized=true;remaining=n;
            }
            if(same(at,"Transfer-Encoding")){if(transfer||!same(value,"chunked"))return false;transfer=chunked=true;}
            if(same(at,"Content-Encoding")&&!same(value,"identity"))return false;
            at=end+2;
        }
        if(chunked&&sized)return false;parsed=true;if(sized&&!remaining)complete=true;return true;
    }
    bool append(char c){if(!c||used+1>=capacity)return false;body[used++]=c;body[used]=0;return true;}
    bool byte(char c)
    {
        if(failed||complete||++total>12288)return false;
        if(!parsed)
        {
            if(headers+1>=sizeof header||(!c)||((unsigned char)c<32&&c!='\r'&&c!='\n'&&c!='\t'))return false;header[headers++]=c;header[headers]=0;
            return headers<4||memcmp(header+headers-4,"\r\n\r\n",4)||parse_headers();
        }
        if(!chunked){if(!append(c))return false;if(sized&&!--remaining)complete=true;return true;}
        if(state==1){if(!append(c))return false;if(!--remaining)state=2;return true;}
        if(state==2){if(c!='\r')return false;state=3;return true;}if(state==3){if(c!='\n')return false;state=0;return true;}
        if(line_used+1>=sizeof line)return false;line[line_used++]=c;line[line_used]=0;
        if(c!='\n')return true;if(line_used<2||line[line_used-2]!='\r')return false;line[line_used-2]=0;
        if(trailers){if(line_used==2)complete=true;else if(!strchr(line,':'))return false;}
        else
        {
            size_t n=0;bool digit=false;for(char *p=line;*p&&*p!=';';++p){unsigned v=*p>='0'&&*p<='9'?*p-'0':*p>='a'&&*p<='f'?*p-'a'+10:*p>='A'&&*p<='F'?*p-'A'+10:16;if(v==16||n>capacity/16)return false;n=n*16+v;digit=true;}
            if(!digit||n>=capacity-used)return false;remaining=n;if(!n)trailers=true;else state=1;
        }
        line_used=0;return true;
    }
    bool feed(const unsigned char *data,size_t size)
    {for(size_t i=0;i<size;++i)if(!byte(char(data[i]))){failed=true;return false;}return true;}
    bool finish(){if(!parsed||failed||(!complete&&(sized||chunked)))return false;complete=true;return true;}
};
