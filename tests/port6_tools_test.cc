#include "editor_tools.h"
#include "graph_scene.h"
#include "lua_input.h"
#include "source_style.h"
#include <cassert>
#include <cstdio>
#include <cstring>
int main()
{
    char source[1024]="alpha beta\n  print('x')",clip[257]="";pico_text_editor e{source,sizeof source,strlen(source),5};pico_editor_tools t(e,clip);
    assert(t.insert("!",1,true));assert(t.insert("!",1,true));assert(!strncmp(source,"alpha!!",7));assert(t.undo());assert(!strncmp(source,"alpha beta",10));assert(t.redo());
    t.anchor=0;e.cursor=5;assert(t.copy());assert(!strcmp(clip,"alpha"));assert(t.insert("hello",5));assert(!strncmp(source,"hello",5));assert(t.undo());assert(!strncmp(source,"alpha",5));
    t.anchor=pico_editor_tools::none;e.cursor=0;assert(t.find("beta"));assert(t.selected());assert(t.insert("B",1));assert(strstr(source,"B"));
    assert(t.replace_all("alpha","word")==1);assert(t.undo());assert(strstr(source,"alpha"));
    e.cursor=e.length;assert(t.newline());assert(e.cursor>=3&&!memcmp(source+e.cursor-3,"\n  ",3));assert(t.unindent());assert(source[e.cursor-1]=='\n');
    for(int i=0;i<20;++i){t.break_group();assert(t.insert("x",1));}for(int i=0;i<8;++i)assert(t.undo());assert(!t.undo());
    strcpy(source,"«abc»");e.length=strlen(source);e.cursor=2;t.anchor=pico_editor_tools::none;assert(t.insert("X",1));assert(t.undo());assert(!strcmp(source,"«abc»"));
    {
        char small[16]="aaaa aaaa",copied[257]="";pico_text_editor ed{small,sizeof small,strlen(small),0};pico_editor_tools tools(ed,copied);
        assert(tools.replace_all("a","0123456789")==-1&&!strcmp(small,"aaaa aaaa"));
        assert(tools.replace_all("aaaa","")==2&&!strcmp(small," "));assert(tools.undo());assert(!strcmp(small," aaaa"));
    }
    pico_graph_scene scene,loaded;strcpy(scene.curve[2],"SIN(X)+π");uint8_t data[1024];size_t n=scene.encode(data,sizeof data);assert(n>700&&n<1024);assert(loaded.decode(data,n));assert(!strcmp(loaded.curve[0],"X^2"));
    loaded.camera.rotate(0.5f,0.2f);loaded.colour[0]=0x123456;loaded.multiple=1;loaded.angle=1;strcpy(loaded.surface,"SIN(X)+COS(Y)");n=loaded.encode(data,sizeof data);assert(scene.decode(data,n));assert(scene.colour[0]==0x123456&&scene.angle==1&&!strcmp(scene.surface,"SIN(X)+COS(Y)"));
    pico_graph_scene previous=scene;data[40]^=1;assert(!scene.decode(data,n));assert(!memcmp(&previous,&scene,sizeof scene));data[40]^=1;
    assert(!scene.decode(data,n-1));scene.window[0]=scene.window[1];assert(!scene.encode(data,sizeof data));scene=previous;
    for(size_t i=0;i<n;++i){uint8_t old=data[i];data[i]^=0x80;assert(!loaded.decode(data,n));data[i]=old;}
    pico_lua_input input;input.feed('A',1);input.feed('d',1);assert(input.held('a')&&input.held('D'));assert(input.poll()=='a');assert(input.poll()=='d');input.feed('A',3);assert(!input.held('a')&&input.held('d'));input.feed(27,1);assert(input.poll()==-2);
    pico_source_style lua(pico_source_style::lua);assert(lua.next('-', '-')==pico_source_style::comment);lua.next('\n');assert(lua.next('\'')==pico_source_style::string);assert(lua.next('@')==pico_source_style::string);assert(lua.next('\'')==pico_source_style::string);assert(pico_source_style::lua_keyword("function",8));assert(!pico_source_style::lua_keyword("variable",8));
    puts("PASS: editor history/selection/clipboard/indent/find/replace, scene integrity/rollback, held-key queue and language colours");
}
