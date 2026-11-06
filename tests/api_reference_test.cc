// Companion-reference contracts, using the production assembler and VM.
#include "chip8.h"
#include "chip8_assembler.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
using namespace pico_chip8;
static std::string source(const char *dir,const char *name){std::ifstream f(std::string(dir)+"/"+name);assert(f);std::ostringstream s;s<<f.rdbuf();return s.str();}
static machine load(const std::string &s){assembler a;uint8_t rom[3584];size_t n=0;assert(a.assemble(s.c_str(),rom,sizeof rom,n));machine m;assert(m.load(rom,n,schip));return m;}
static void run(machine &m,bool stop_at_wait=false,unsigned key=0){for(unsigned i=0;i<20000&&!m.exited&&!m.error;++i){if(m.waiting>=0){if(stop_at_wait)return;m.set_keys(1u<<key);m.set_keys(0);}else m.step();if(i%20==0)m.tick();}assert(!m.error);assert(m.exited);assert(m.sp==0);}
static void glyph(const machine &m,unsigned digit,int x,int y){for(int row=0;row<5;++row)for(int col=0;col<8;++col){bool bit=machine::font[digit*5+row]&(0x80>>col);for(int yy=0;yy<2;++yy)for(int xx=0;xx<2;++xx)assert(m.pixel((x+col)*2+xx,(y+row)*2+yy)==bit);}}
int main(int argc,char **argv){assert(argc==2);const char *dir=argv[1];
 {auto m=load(source(dir,"digit.asm"));run(m,true);assert(m.waiting==0&&m.sp==0&&m.index==0x50+10*5);assert(m.v[0]==10&&m.v[1]==12&&m.v[2]==8);glyph(m,10,12,8);run(m);}
 {std::string s=source(dir,"decimal.asm");assembler a;uint8_t rom[3584];size_t n;assert(a.assemble(s.c_str(),rom,sizeof rom,n));unsigned scratch=0;for(unsigned i=0;i<a.count;++i)if(!strcmp(a.labels[i].name,"decimal_digits"))scratch=a.labels[i].address;assert(scratch);auto m=load(s);run(m,true);assert(m.sp==0&&m.v[0]==7&&m.v[1]==18&&m.v[2]==10);assert(m.memory[scratch]==2&&m.memory[scratch+1]==3&&m.memory[scratch+2]==7);
  // Three adjacent four-pixel digits, with a one-pixel gap.
  for(int row=0;row<5;++row)for(int col=0;col<15;++col){unsigned digit=col/5,bit=col%5;unsigned ds[]={2,3,7};bool lit=bit<4&&(machine::font[ds[digit]*5+row]&(0x80>>bit));assert(m.pixel((8+col)*2,(10+row)*2)==lit);}run(m);
  // Leading zeroes are deliberate; verify the actual routine for input 7 too.
  s.replace(s.find("LD V0, 237"),10,"LD V0, 7");m=load(s);run(m,true);assert(m.memory[scratch]==0&&m.memory[scratch+1]==0&&m.memory[scratch+2]==7);}
 {std::string s=source(dir,"timing.asm");auto m=load(s);bool sounded=false;unsigned max_depth=0;for(unsigned i=0;i<20000&&!m.exited&&!m.error;++i){m.step();sounded|=m.sound>0;if(m.sp>max_depth)max_depth=m.sp;if(i%20==0)m.tick();}assert(m.exited&&!m.error&&sounded&&m.delay==0&&m.sound==0&&m.sp==0&&max_depth==2);}
 {auto m=load(source(dir,"key.asm"));run(m,false,10);glyph(m,10,8,8);}
 {std::string routine=source(dir,"saturating.asm");routine=routine.substr(routine.find("add_saturating:"));for(auto pair:{std::pair<unsigned,unsigned>{250,20},{10,20},{255,1},{0,0},{255,0}}){auto m=load("LD V0, "+std::to_string(pair.first)+"\nLD V1, "+std::to_string(pair.second)+"\nCALL add_saturating\nEXIT\n"+routine);run(m);unsigned sum=pair.first+pair.second;assert(m.v[0]==(sum>255?255:sum)&&m.v[1]==pair.second&&m.v[15]==(sum>255));}}
 {auto m=load("DW 0x0123\n");m.step();assert(m.error&&!strcmp(m.error,"Unsupported system instruction"));}
 {auto m=load("recurse: CALL recurse\n");for(int i=0;i<17;++i)m.step();assert(m.error&&!strcmp(m.error,"Return stack overflow"));}
 puts("PASS: API reference digit pixels, decimal/leading zeros, keypad, timer/sound, nested calls, saturating boundaries and unsupported system calls");
}
