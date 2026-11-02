#pragma once
#include <cstdio>
#include <cstring>
#include <stdint.h>
namespace pico_svg {
typedef bool (*sink)(void *,const char *,size_t);
struct writer
{
    sink output;void *context;bool ok=true,clipped=false;
    bool put(const char *s){if(ok)ok=output(context,s,strlen(s));return ok;}
    void xml(const char *s)
    {
        for(;ok&&*s;++s){const char *escape=*s=='&'?"&amp;":*s=='<'?"&lt;":*s=='>'?"&gt;":*s=='\"'?"&quot;":*s=='\''?"&apos;":nullptr;if(escape)put(escape);else if(ok)ok=output(context,s,1);}
    }
    void text(int x,int y,const char *s,uint32_t colour)
    {
        char line[160];snprintf(line,sizeof line,"<text x=\"%d\" y=\"%d\" fill=\"#%06lx\" font-family=\"monospace\" font-size=\"12\">",x,y,(unsigned long)(colour&0xffffff));put(line);xml(s);put("</text>\n");
    }
    void begin(const char *title,uint32_t background,uint32_t foreground,int x=10,int y=35,int width=300,int height=210)
    {
        put("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"320\" height=\"320\" viewBox=\"0 0 320 320\"><title>");xml(title);put("</title>\n");
        char line[240];snprintf(line,sizeof line,"<rect width=\"320\" height=\"320\" fill=\"#%06lx\"/><defs><clipPath id=\"plot\"><rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\"/></clipPath></defs>\n",(unsigned long)(background&0xffffff),x,y,width,height);put(line);text(5,20,title,foreground);put("<g clip-path=\"url(#plot)\">\n");clipped=true;
    }
    void line(int x1,int y1,int x2,int y2,int width,uint32_t colour)
    {
        char line[200];snprintf(line,sizeof line,"<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"#%06lx\" stroke-width=\"%d\"/>\n",x1,y1,x2,y2,(unsigned long)(colour&0xffffff),width);put(line);
    }
    void end_plot(){if(clipped){put("</g>\n");clipped=false;}}
    bool finish(){end_plot();put("</svg>\n");return ok;}
};
}
