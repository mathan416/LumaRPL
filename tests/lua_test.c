#include "lua_runner.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
struct reader_state {const char *source;size_t position;int fail;};
static const char *stream_read(void *opaque,size_t *size)
{struct reader_state *r=opaque;if(r->fail){*size=SIZE_MAX;return NULL;}size_t left=strlen(r->source)-r->position;*size=left>7?7:left;const char *part=r->source+r->position;r->position+=*size;return *size?part:NULL;}
static int checks,stop_after;
extern int test_draws,test_last_op,test_key,test_held,test_beeps;
extern unsigned test_colour;
extern int test_analysis_mode,test_analysis_status;extern double test_analysis_value;
extern int test_plots,test_plot_mode,test_plot_count;
extern double test_plot_min,test_plot_max;
int pico_lua_cancel(void){return stop_after && ++checks>=stop_after;}
int pico_lua_eval(const char *s,char *out,size_t n){snprintf(out,n,"%s",s);return 1;}
int main(void)
{
    {pico_lua_result sprite_result;const char *s="assert(screen.sprite('DEMO.sprite',-2,310));local ok,err=screen.sprite('../bad.sprite',0,0);assert(ok==nil and type(err)=='string')";assert(pico_lua_run(s,strlen(s),&sprite_result));assert(sprite_result.graphics);}
    {pico_lua_result font_result;assert(pico_lua_run("assert(screen.font()=='compact'); assert(screen.font('mono')=='compact'); assert(screen.font()=='mono'); local w,h=screen.measure('ABC'); assert(w==30 and h==16); assert(screen.font('legacy')=='mono'); assert(screen.font('compact')=='legacy')",sizeof("assert(screen.font()=='compact'); assert(screen.font('mono')=='compact'); assert(screen.font()=='mono'); local w,h=screen.measure('ABC'); assert(w==30 and h==16); assert(screen.font('legacy')=='mono'); assert(screen.font('compact')=='legacy')")-1,&font_result));}

    {pico_lua_result r;pico_lua_session *session=pico_lua_open(&r);assert(session);
     const char *set="screen.font('mono')";assert(pico_lua_submit(session,set,strlen(set),0,&r)==1);
     const char *check="assert(screen.font()=='mono');screen.font('legacy');screen.text(1,2,'old')";assert(pico_lua_submit(session,check,strlen(check),0,&r)==1);
     const char *bad="screen.font('invalid')";assert(pico_lua_submit(session,bad,strlen(bad),0,&r)==0);
     check="assert(screen.font()=='legacy')";assert(pico_lua_submit(session,check,strlen(check),0,&r)==1);pico_lua_close(session);puts("PASS: Lua font persists across console commands and errors");}
    for(int demo=0;demo<3;++demo)
    {
        const char *name=demo==2?"resources/lua/ANALYSE.lua":demo?"resources/lua/PLOT3D.lua":"resources/lua/PLOT2D.lua";
        FILE *f=fopen(name,"rb");assert(f);char source[4096];size_t n=fread(source,1,sizeof source-1,f);assert(!ferror(f));fclose(f);
        pico_lua_result result;assert(pico_lua_run(source,n,&result));assert(strstr(result.output,demo==2?"Analysis demo finished":"Returned from native"));
        assert(test_plot_mode==(demo==1)&&test_plot_count==(demo==1?289:600));
    }
    pico_lua_result r;
    const char *script="local s=0; for i=1,100 do s=s+i end; print(s); print(math.sqrt(81)); print(calc.eval('bridge')); print(type(io),type(os),type(package)); print(string.format(\"%a\",0.5))";
    assert(pico_lua_run(script,strlen(script),&r));assert(strstr(r.output,"5050\n") && strstr(r.output,"bridge") && strstr(r.output,"nil\tnil\tnil") && strstr(r.output,"0x1p-1"));
    printf("Lua normal script peak: %zu bytes (budget 49152)\n",r.peak);assert(r.peak<=49152);
    const char *expanded="local t={} for i=1,800 do t[i]=i end print(#t)";
    assert(pico_lua_run(expanded,strlen(expanded),&r));assert(!strcmp(r.output,"800\n"));assert(r.peak>28672&&r.peak<=49152);
    printf("PASS: Lua 800-element table uses %zu bytes, above the previous 28672-byte limit\n",r.peak);
    assert(!pico_lua_run("local =",7,&r));assert(strstr(r.output,"script:1:"));
    script="local t={};for i=1,100000 do t[i]=string.rep('X',100) end";
    assert(!pico_lua_run(script,strlen(script),&r));assert(strstr(r.output,"memory"));assert(r.peak<=49152);
    char deep[1024]="return ";size_t used=strlen(deep);
    for(int i=0;i<200;++i)deep[used++]='(';deep[used++]='1';for(int i=0;i<200;++i)deep[used++]=')';deep[used]=0;
    assert(!pico_lua_run(deep,used,&r));
    checks=0;stop_after=5;script="while true do end";assert(!pico_lua_run(script,strlen(script),&r)&&r.cancelled);stop_after=0;
    for(int i=0;i<20;++i)
    {
        struct reader_state reader={"plot.function2d(function(x) return x*x end);plot.show()",0,0};assert(pico_lua_run_stream(stream_read,&reader,&r));
        reader=(struct reader_state){"this is not valid Lua !",0,0};assert(!pico_lua_run_stream(stream_read,&reader,&r));
        reader=(struct reader_state){"print('never execute')",0,1};assert(!pico_lua_run_stream(stream_read,&reader,&r)&&strstr(r.output,"read failed"));
    }
    puts("PASS: streamed Lua compilation, read/syntax errors and repeated cleanup");
    puts("PASS: bounded Lua, libraries, print/bridge, line errors, memory exhaustion, interruption and repeated cleanup");
}
