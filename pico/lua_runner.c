#include "lua_runner.h"
#include "lua_device.h"
#include "plot_api.h"
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifndef LUA_BUDGET
#define LUA_BUDGET (48*1024)
#endif
struct pico_lua_session { size_t used,peak;lua_State *L;pico_lua_result storage,*result;pico_plot_request plot;int plot_ref[4],plot_busy,font; };
typedef struct pico_lua_session context;
static void *allocate(void *ud,void *ptr,size_t old,size_t size)
{
    context *c=ud;if(!ptr)old=0;
    if(!size){free(ptr);c->used-=old;return NULL;}
    if(size>LUA_BUDGET || c->used-old>LUA_BUDGET-size)return NULL;
    void *p=realloc(ptr,size);if(!p)return NULL;
    c->used=c->used-old+size;if(c->used>c->peak)c->peak=c->used;return p;
}
static context *get_context(lua_State *L){void *p;lua_getallocf(L,&p);return p;}
static void append(context *c,const char *s,size_t n)
{
    pico_lua_result *r=c->result;size_t left=sizeof r->output-1-r->length;if(n>left)n=left;
    memcpy(r->output+r->length,s,n);r->length+=n;r->output[r->length]=0;
}
static int print_line(lua_State *L)
{
    context *c=get_context(L);int count=lua_gettop(L);
    for(int i=1;i<=count;++i){size_t n;const char *s=luaL_tolstring(L,i,&n);if(i>1)append(c,"\t",1);append(c,s,n);lua_pop(L,1);}
    append(c,"\n",1);return 0;
}
static int calculator(lua_State *L)
{
    const char *source=luaL_checkstring(L,1);char text[256];
    if(!pico_lua_eval(source,text,sizeof text))return luaL_error(L,"%s",text);
    lua_pushstring(L,text);return 1;
}
static void hook(lua_State *L,lua_Debug *ar)
{
    (void)ar;if(pico_lua_cancel()){get_context(L)->result->cancelled=1;luaL_error(L,"Interrupted");}
}
static int interrupted(lua_State *L)
{get_context(L)->result->cancelled=1;return luaL_error(L,"Interrupted");}
static int bounded(lua_State *L,int n,int low,int high)
{lua_Integer v=luaL_checkinteger(L,n);luaL_argcheck(L,v>=low&&v<=high,n,"argument out of range");return (int)v;}
static int coordinate(lua_State *L,int arg,int low,int high)
{lua_Number n=luaL_checknumber(L,arg);luaL_argcheck(L,isfinite(n)&&n>=low&&n<=high,arg,"coordinate out of range");return (int)floor(n+0.5);}
static uint32_t colour(lua_State *L,int n,uint32_t fallback)
{
    if(lua_isnoneornil(L,n))return fallback;
    if(lua_type(L,n)==LUA_TNUMBER)return (uint32_t)bounded(L,n,0,0xffffff);
    const char *s=luaL_checkstring(L,n);
    const struct {const char *name;uint32_t rgb;} colours[]={
        {"black",0},{"white",0xffffff},{"red",0xff4040},{"green",0x40e080},
        {"blue",0x4080ff},{"cyan",0x40e0ff},{"magenta",0xe060e0},
        {"yellow",0xffff40},{"orange",0xffa040},{"gray",0x808080}};
    for(size_t i=0;i<sizeof colours/sizeof *colours;++i)if(!strcmp(s,colours[i].name))return colours[i].rgb;
    if(s[0]=='#'&&strlen(s)==7){int valid=1;for(int i=1;i<7;++i)if(!isxdigit((unsigned char)s[i]))valid=0;if(valid){char *end;unsigned long v=strtoul(s+1,&end,16);if(!*end&&v<=0xffffff)return (uint32_t)v;}}
    luaL_argerror(L,n,"unknown colour (use name, #RRGGBB or integer)");return 0;
}
static int draw(lua_State *L)
{
    int op=(int)lua_tointeger(L,lua_upvalueindex(1)),a[6]={0};uint32_t c=0xffffff;
    const char *text=NULL;size_t length=0;
    if(op==PICO_LUA_CLEAR)c=colour(L,1,0);
    else if(op!=PICO_LUA_SHOW)
    {
        a[0]=coordinate(L,1,-4096,4096);a[1]=coordinate(L,2,-4096,4096);
        if(op==PICO_LUA_TEXT){a[5]=get_context(L)->font;text=luaL_checklstring(L,3,&length);luaL_argcheck(L,length<=1024,3,"text exceeds 1024 bytes");c=colour(L,4,0xffffff);}
        else if(op==PICO_LUA_PIXEL)c=colour(L,3,0xffffff);
        else if(op==PICO_LUA_CIRCLE){a[2]=coordinate(L,3,0,640);c=colour(L,4,0xffffff);a[4]=lua_toboolean(L,5);}
        else
        {
            int low=op==PICO_LUA_RECT?0:-4096,high=op==PICO_LUA_RECT?640:4096;
            a[2]=coordinate(L,3,low,high);a[3]=coordinate(L,4,low,high);c=colour(L,5,0xffffff);
            a[4]=lua_isnone(L,6)?1:lua_toboolean(L,6);
        }
    }
    pico_lua_draw(op,a,c,text,length);get_context(L)->result->graphics=1;if(pico_lua_cancel())return interrupted(L);return 0;
}
static const char *const font_names[]={"compact","mono","legacy",NULL};
static int screen_font(lua_State *L)
{
    context *c=get_context(L);int before=c->font;
    if(!lua_isnoneornil(L,1))c->font=luaL_checkoption(L,1,NULL,font_names);
    lua_pushstring(L,font_names[before]);return 1;
}
static int screen_measure(lua_State *L)
{
    size_t length;const char *text=luaL_checklstring(L,1,&length);luaL_argcheck(L,length<=1024,1,"text exceeds 1024 bytes");
    int width=0,height=0;pico_lua_text_measure(get_context(L)->font,text,length,&width,&height);
    lua_pushinteger(L,width);lua_pushinteger(L,height);return 2;
}
static int screen_sprite(lua_State *L)
{
    size_t length;const char *name=luaL_checklstring(L,1,&length);
    int x=bounded(L,2,-4096,4096),y=bounded(L,3,-4096,4096);
    if(length>80||memchr(name,0,length)||!pico_lua_sprite(name,x,y)){lua_pushnil(L);lua_pushliteral(L,"Invalid or unavailable sprite; use a filename under /db48x/sprites");return 2;}
    get_context(L)->result->graphics=1;lua_pushboolean(L,1);return 1;
}
static int key_code(lua_State *L,int arg)
{
    const char *s=luaL_checkstring(L,arg);if(strlen(s)==1)return (unsigned char)s[0];
    const struct {const char *name;int code;} names[]={
        {"left",0xb4},{"right",0xb7},{"up",0xb5},{"down",0xb6},{"enter",13},{"space",32},{"backspace",8},
        {"f1",0x81},{"f2",0x82},{"f3",0x83},{"f4",0x84},{"f5",0x85}};
    for(size_t i=0;i<sizeof names/sizeof *names;++i)if(!strcmp(s,names[i].name))return names[i].code;
    return luaL_argerror(L,arg,"unknown key");
}
static int push_key(lua_State *L,int code)
{
    if(code==-2)return interrupted(L);if(code<0){lua_pushnil(L);return 1;}
    const char *name=NULL;char single[2]={(char)code,0};
    switch(code){case 10:case 13:name="enter";break;case 32:name="space";break;case 8:case 127:name="backspace";break;
    case 0xb4:name="left";break;case 0xb7:name="right";break;case 0xb5:name="up";break;case 0xb6:name="down";break;
    case 0x81:name="f1";break;case 0x82:name="f2";break;case 0x83:name="f3";break;case 0x84:name="f4";break;case 0x85:name="f5";break;}
    if(!name && (code<32||code>=127)){lua_pushnil(L);return 1;}lua_pushstring(L,name?name:single);return 1;
}
static int key_poll(lua_State *L){return push_key(L,pico_lua_key_poll());}
static int key_down(lua_State *L)
{int code=key_code(L,1),down=pico_lua_key_down(code);if(down<0)return interrupted(L);lua_pushboolean(L,down);return 1;}
static int key_wait(lua_State *L)
{
    unsigned timeout=lua_isnoneornil(L,1)?0:(unsigned)bounded(L,1,0,600000);uint32_t start=pico_lua_millis();
    for(;;){int code=pico_lua_key_poll();if(code!=-1)return push_key(L,code);if(timeout&&(uint32_t)(pico_lua_millis()-start)>=timeout)return push_key(L,-1);if(pico_lua_sleep(5))return interrupted(L);}
}
static int sleep_ms(lua_State *L){unsigned n=(unsigned)bounded(L,1,0,600000);if(pico_lua_sleep(n))return interrupted(L);return 0;}
static int millis(lua_State *L){lua_pushinteger(L,pico_lua_millis());return 1;}
static int beep(lua_State *L){unsigned hz=(unsigned)bounded(L,1,0,16000),ms=(unsigned)bounded(L,2,0,10000);luaL_argcheck(L,hz==0||hz>=20,1,"frequency must be zero or 20..16000 Hz");if(pico_lua_beep(hz,ms))return interrupted(L);return 0;}
static int stop_sound(lua_State *L){pico_lua_beep(0,0);return 0;}
/* Native plots own no Lua objects. Every sample enters Lua through pcall. */
static void plot_defaults(context *c)
{
    memset(&c->plot,0,sizeof c->plot);
    double window[]={-10,10,-6,6},bounds[]={-3,3,-3,3};
    memcpy(c->plot.window,window,sizeof window);memcpy(c->plot.bounds3d,bounds,sizeof bounds);
    c->plot.colour[0]=0x3080ef;c->plot.colour[1]=0xf09030;c->plot.colour[2]=0xc050d0;
    c->plot.yaw=0.65;c->plot.pitch=0.55;c->plot.zoom=1;
}
static int plot_idle(lua_State *L)
{if(get_context(L)->plot_busy)return luaL_error(L,"plot cannot be changed inside a sampling callback");return 0;}
static int plot_clear(lua_State *L)
{
    plot_idle(L);context *c=get_context(L);
    for(int i=0;i<4;++i){if(c->plot_ref[i]>0)luaL_unref(L,LUA_REGISTRYINDEX,c->plot_ref[i]);c->plot_ref[i]=0;}
    plot_defaults(c);return 0;
}
static int valid_bounds(const double *v)
{for(int i=0;i<4;++i)if(!isfinite(v[i])||fabs(v[i])>1e100)return 0;return v[0]<v[1]&&v[2]<v[3];}
static int plot_window(lua_State *L)
{
    plot_idle(L);double bounds[4];for(int i=0;i<4;++i)bounds[i]=luaL_checknumber(L,i+1);
    if(!valid_bounds(bounds))return luaL_error(L,"bounds must be finite, ordered and within +/-1e100");
    memcpy(get_context(L)->plot.window,bounds,sizeof bounds);return 0;
}
static void plot_field(lua_State *L,const char *key)
{lua_pushstring(L,key);lua_rawget(L,2);}
static double plot_option(lua_State *L,const char *key,double fallback)
{
    if(lua_isnoneornil(L,2))return fallback;
    plot_field(L,key);double value=lua_isnil(L,-1)?fallback:luaL_checknumber(L,-1);lua_pop(L,1);return value;
}
static int plot_function(lua_State *L)
{
    plot_idle(L);luaL_checktype(L,1,LUA_TFUNCTION);if(!lua_isnoneornil(L,2))luaL_checktype(L,2,LUA_TTABLE);
    context *c=get_context(L);int surface=lua_toboolean(L,lua_upvalueindex(1)),slot=3;
    pico_plot_request candidate=c->plot;
    if(!surface)
    {
        for(slot=0;slot<3&&c->plot_ref[slot]>0;++slot){}
        if(slot==3)return luaL_error(L,"at most three 2D curves; use plot.clear()");
        uint32_t rgb=candidate.colour[slot];char label[80];snprintf(label,sizeof label,"Lua function %d",slot+1);
        if(!lua_isnoneornil(L,2))
        {
            plot_field(L,"colour");rgb=colour(L,-1,rgb);lua_pop(L,1);
            plot_field(L,"label");if(!lua_isnil(L,-1))
            {
                size_t n;const char *s=luaL_checklstring(L,-1,&n);luaL_argcheck(L,n<sizeof label,2,"label exceeds 79 bytes");
                for(size_t i=0;i<n;++i)luaL_argcheck(L,(unsigned char)s[i]>=32&&(unsigned char)s[i]<127,2,"label must be printable ASCII");
                memcpy(label,s,n);label[n]=0;
            }
            lua_pop(L,1);
        }
        candidate.colour[slot]=rgb;strcpy(candidate.label[slot],label);candidate.active[slot]=1;
    }
    else
    {
        const char *keys[]={"xmin","xmax","ymin","ymax"};
        for(int i=0;i<4;++i)candidate.bounds3d[i]=plot_option(L,keys[i],candidate.bounds3d[i]);
        if(!valid_bounds(candidate.bounds3d))return luaL_error(L,"invalid surface bounds");
        candidate.yaw=plot_option(L,"yaw",candidate.yaw);candidate.pitch=plot_option(L,"pitch",candidate.pitch);candidate.zoom=plot_option(L,"zoom",candidate.zoom);
        if(!isfinite(candidate.yaw)||fabs(candidate.yaw)>10000||!isfinite(candidate.pitch)||candidate.pitch<-1.4||candidate.pitch>1.4||!isfinite(candidate.zoom)||candidate.zoom<0.4||candidate.zoom>2.5)return luaL_error(L,"invalid surface camera");
    }
    lua_pushvalue(L,1);int ref=luaL_ref(L,LUA_REGISTRYINDEX);
    if(c->plot_ref[slot]>0)luaL_unref(L,LUA_REGISTRYINDEX,c->plot_ref[slot]);
    c->plot_ref[slot]=ref;candidate.surface=surface;c->plot=candidate;return 0;
}
struct lua_plot_call {lua_State *L;context *c;int failed,cancelled;char error[256];};
static int plot_sample(void *opaque,int curve,double x,double y,double *z)
{
    struct lua_plot_call *call=opaque;lua_State *L=call->L;context *c=call->c;
    if(pico_lua_cancel()){call->cancelled=1;return -2;}
    int top=lua_gettop(L),ref=c->plot_ref[c->plot.surface?3:curve];
    lua_rawgeti(L,LUA_REGISTRYINDEX,ref);lua_pushnumber(L,x);if(c->plot.surface)lua_pushnumber(L,y);
    int status=lua_pcall(L,c->plot.surface?2:1,1,0);
    if(status!=LUA_OK)
    {
        const char *error=lua_type(L,-1)==LUA_TSTRING?lua_tostring(L,-1):"Lua callback error (non-string value)";
        snprintf(call->error,sizeof call->error,"%s",error);call->failed=1;
        call->cancelled=c->result->cancelled;lua_settop(L,top);return call->cancelled?-2:-1;
    }
    int result=0;
    if(lua_type(L,-1)==LUA_TNUMBER){*z=lua_tonumber(L,-1);result=isfinite(*z)?1:0;}
    else if(!lua_isnil(L,-1)){strcpy(call->error,"plot function must return a number or nil");call->failed=1;result=-1;}
    lua_settop(L,top);return result;
}
static int plot_show(lua_State *L)
{
    plot_idle(L);context *c=get_context(L);
    if(c->plot.surface?c->plot_ref[3]<=0:!(c->plot.active[0]||c->plot.active[1]||c->plot.active[2]))return luaL_error(L,"no plot functions; add function2d or surface first");
    if(!lua_checkstack(L,8))return luaL_error(L,"not enough Lua stack for plot");
    struct lua_plot_call call={L,c,0,0,{0}};c->plot_busy=1;
    int ok=pico_native_plot(&c->plot,plot_sample,&call);c->plot_busy=0;c->result->graphics=0;
    pico_lua_device_reset();
    if(call.cancelled||ok==-2)return interrupted(L);
    if(call.failed)return luaL_error(L,"%s",call.error);
    if(ok!=1)return luaL_error(L,"native plot unavailable (memory or no finite surface samples)");
    return 0;
}
static void initialize_plot(lua_State *L)
{
    plot_defaults(get_context(L));lua_newtable(L);
    lua_pushcfunction(L,plot_clear);lua_setfield(L,-2,"clear");
    lua_pushcfunction(L,plot_window);lua_setfield(L,-2,"window");
    lua_pushboolean(L,0);lua_pushcclosure(L,plot_function,1);lua_setfield(L,-2,"function2d");
    lua_pushboolean(L,1);lua_pushcclosure(L,plot_function,1);lua_setfield(L,-2,"surface");
    lua_pushcfunction(L,plot_show);lua_setfield(L,-2,"show");lua_setglobal(L,"plot");
}
static int initialize(lua_State *L)
{
    const struct {const char *name;lua_CFunction open;} libs[]={
        {LUA_GNAME,luaopen_base},{LUA_TABLIBNAME,luaopen_table},
        {LUA_STRLIBNAME,luaopen_string},{LUA_MATHLIBNAME,luaopen_math},
        {LUA_UTF8LIBNAME,luaopen_utf8},{LUA_COLIBNAME,luaopen_coroutine}};
    for(size_t i=0;i<sizeof libs/sizeof *libs;++i){luaL_requiref(L,libs[i].name,libs[i].open,1);lua_pop(L,1);}
    lua_pushnil(L);lua_setglobal(L,"dofile");lua_pushnil(L);lua_setglobal(L,"loadfile");
    lua_pushcfunction(L,print_line);lua_setglobal(L,"print");
    lua_newtable(L);lua_pushcfunction(L,calculator);lua_setfield(L,-2,"eval");lua_setglobal(L,"calc");
    lua_newtable(L);const char *draws[]={"clear","pixel","line","rect","circle","text","show"};
    for(int i=0;i<7;++i){lua_pushinteger(L,i);lua_pushcclosure(L,draw,1);lua_setfield(L,-2,draws[i]);}
    lua_pushcfunction(L,screen_sprite);lua_setfield(L,-2,"sprite");
    lua_pushcfunction(L,screen_font);lua_setfield(L,-2,"font");lua_pushcfunction(L,screen_measure);lua_setfield(L,-2,"measure");
    lua_pushinteger(L,320);lua_setfield(L,-2,"width");lua_pushinteger(L,320);lua_setfield(L,-2,"height");lua_setglobal(L,"screen");
    lua_newtable(L);lua_pushcfunction(L,key_poll);lua_setfield(L,-2,"poll");lua_pushcfunction(L,key_wait);lua_setfield(L,-2,"wait");lua_pushcfunction(L,key_down);lua_setfield(L,-2,"down");lua_setglobal(L,"keyboard");
    lua_newtable(L);lua_pushcfunction(L,sleep_ms);lua_setfield(L,-2,"sleep");lua_pushcfunction(L,millis);lua_setfield(L,-2,"millis");lua_setglobal(L,"system");
    lua_newtable(L);lua_pushcfunction(L,beep);lua_setfield(L,-2,"beep");lua_pushcfunction(L,stop_sound);lua_setfield(L,-2,"stop");lua_setglobal(L,"sound");initialize_plot(L);return 0;
}
// Capture a real Lua frame before pcall unwinds it. Error text alone can be
// user-supplied (error(message, 0)) and is not a reliable runtime source map.
static int locate_error(lua_State *L)
{
    context *c=get_context(L);lua_Debug frame;
    for(int level=0;level<40&&lua_getstack(L,level,&frame);++level)
        if(lua_getinfo(L,"Sl",&frame)&&frame.source&&!strcmp(frame.source,"@script")&&frame.currentline>0){c->storage.error_line=(unsigned)frame.currentline;break;}
    lua_pushvalue(L,1);return 1;
}
static int run_script(context *c,int results)
{
    int base=lua_gettop(c->L);lua_pushcfunction(c->L,locate_error);lua_insert(c->L,base);
    int status=lua_pcall(c->L,0,results,base);lua_remove(c->L,base);return status;
}
static void failure(context *c)
{
    const char *s=lua_type(c->L,-1)==LUA_TSTRING?lua_tostring(c->L,-1):"Lua error (non-string value)";snprintf(c->storage.error,sizeof c->storage.error,"%s",s);
    if(!c->storage.error_line&&!strncmp(s,"script:",7)){char *end;unsigned long line=strtoul(s+7,&end,10);if(end!=s+7&&*end==':'&&line>0&&line<=16384)c->storage.error_line=(unsigned)line;}
    append(c,s,strlen(s));append(c,"\n",1);
}
pico_lua_session *pico_lua_open(pico_lua_result *result)
{
    memset(result,0,sizeof *result);context *c=calloc(1,sizeof *c);
    if(!c){strcpy(result->output,"Not enough Lua memory");result->length=strlen(result->output);return NULL;}
    c->result=&c->storage;pico_lua_device_begin();c->L=lua_newstate(allocate,c);
    if(!c->L){strcpy(result->output,"Not enough Lua memory");result->length=strlen(result->output);result->peak=c->peak;pico_lua_device_end();free(c);return NULL;}
    lua_sethook(c->L,hook,LUA_MASKCOUNT,1000);lua_pushcfunction(c->L,initialize);
    if(lua_pcall(c->L,0,0,0)!=LUA_OK){failure(c);c->storage.peak=c->peak;*result=c->storage;lua_close(c->L);pico_lua_device_end();free(c);return NULL;}
    c->storage.peak=c->peak;*result=c->storage;return c;
}
struct chunk_reader {const char *source;size_t length;int phase;};
static const char *read_expression(lua_State *L,void *ud,size_t *size)
{
    (void)L;struct chunk_reader *r=ud;
    if(r->phase++==0){*size=7;return "return ";}if(r->phase==2){*size=r->length;return r->source;}*size=0;return NULL;
}
int pico_lua_submit(pico_lua_session *c,const char *source,size_t length,int interactive,pico_lua_result *result)
{
    memset(&c->storage,0,sizeof c->storage);pico_lua_device_reset();lua_settop(c->L,0);int status;
    if(interactive)
    {
        struct chunk_reader reader={source,length,0};status=lua_load(c->L,read_expression,&reader,"@console","t");
        if(status==LUA_ERRSYNTAX){lua_pop(c->L,1);status=luaL_loadbufferx(c->L,source,length,"@console","t");}
    }
    else status=luaL_loadbufferx(c->L,source,length,"@script","t");
    if(status==LUA_ERRSYNTAX && interactive)
    {
        const char *error=lua_tostring(c->L,-1);
        if(error && strstr(error,"<eof>")){lua_settop(c->L,0);c->storage.peak=c->peak;*result=c->storage;return 2;}
    }
    if(status==LUA_OK)status=run_script(c,LUA_MULTRET);
    if(status==LUA_OK && interactive && lua_gettop(c->L))
    {
        // Printing results is protected too: __tostring can allocate or fail.
        int n=lua_gettop(c->L);if(n>16){lua_settop(c->L,16);n=16;}
        if(!lua_checkstack(c->L,1)){append(c,"Not enough Lua memory\n",22);c->storage.peak=c->peak;*result=c->storage;lua_settop(c->L,0);return 0;}
        lua_pushcfunction(c->L,print_line);lua_insert(c->L,1);status=lua_pcall(c->L,n,0,0);
    }
    if(status!=LUA_OK)failure(c);c->storage.peak=c->peak;*result=c->storage;lua_settop(c->L,0);return status==LUA_OK;
}
struct stream_reader {pico_lua_reader read;void *context;};
static const char *read_stream(lua_State *L,void *opaque,size_t *size)
{
    struct stream_reader *reader=opaque;const char *part=reader->read(reader->context,size);
    if(*size==SIZE_MAX)luaL_error(L,"SD source read failed");return part;
}
int pico_lua_run_stream(pico_lua_reader read,void *opaque,pico_lua_result *result)
{
    pico_lua_session *c=pico_lua_open(result);if(!c)return 0;
    struct stream_reader reader={read,opaque};int status=lua_load(c->L,read_stream,&reader,"@script","t");
    if(status==LUA_OK)status=run_script(c,0);
    if(status!=LUA_OK)failure(c);c->storage.peak=c->peak;*result=c->storage;pico_lua_close(c);return status==LUA_OK;
}
void pico_lua_close(pico_lua_session *c)
{if(c){lua_close(c->L);pico_lua_device_end();free(c);}}
int pico_lua_run(const char *source,size_t length,pico_lua_result *result)
{
    pico_lua_session *c=pico_lua_open(result);if(!c)return 0;
    int status=pico_lua_submit(c,source,length,0,result);pico_lua_close(c);return status;
}
