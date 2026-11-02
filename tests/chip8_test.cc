#include "chip8.h"
#include "chip8_assembler.h"
#include "source_style.h"
#include <cassert>
#include <cstdio>
#include <vector>
#include <fstream>
#include <iterator>
using namespace pico_chip8;
static void opcode(machine &m,unsigned op){m.pc=0x200;m.memory[0x200]=op>>8;m.memory[0x201]=op;m.error=nullptr;m.step();}
int main()
{
    frame_clock clock{0};unsigned ticks=0;
    for(uint64_t now=0;now<1000000;now+=30000)ticks+=clock.due(now);
    assert(ticks>=59&&ticks<=61);assert(clock.due(3000000)==6);assert(clock.due(3000001)==0);
    machine m;uint8_t dummy[]={0x12,0x00};assert(m.load(dummy,2,schip));
    m.v[0]=250;m.v[15]=10;opcode(m,0x80f4);assert(m.v[0]==4&&m.v[15]==1);
    m.v[15]=5;m.v[1]=6;opcode(m,0x8f14);assert(m.v[15]==0);
    m.v[0]=1;m.v[1]=8;opcode(m,0x8016);assert(m.v[0]==0&&m.v[15]==1);
    m.mode=vip;m.v[0]=1;opcode(m,0x8016);assert(m.v[0]==4&&m.v[15]==0);
    m.index=4095;opcode(m,0xf133);assert(m.error);
    m.sp=0;opcode(m,0xee);assert(m.error);
    m.sp=16;opcode(m,0x2200);assert(m.error);
    m.load(dummy,2,schip);opcode(m,0xff);assert(m.high);m.index=0x50;m.v[0]=127;m.v[1]=63;opcode(m,0xd015);assert(!m.error&&m.pixel(127,63));opcode(m,0xd015);assert(m.v[15]==1&&!m.pixel(127,63));
    m.load(dummy,2,schip);opcode(m,0xf00a);assert(m.waiting==0);m.set_keys(1<<5);assert(m.waiting==0);m.set_keys(0);assert(m.waiting==-1&&m.v[0]==5&&m.pc==0x202);
    m.load(dummy,2,schip);m.set_pixel(0,0,true);opcode(m,0x00c1);assert(m.pixel(0,2)&&!m.pixel(0,0));
    m.load(dummy,2,schip_legacy);m.set_pixel(0,0,true);opcode(m,0x00c1);assert(m.pixel(0,1)&&!m.pixel(0,0));
    m.load(dummy,2,schip);opcode(m,0xff);m.set_pixel(0,0,true);opcode(m,0xfb);assert(m.pixel(4,0));opcode(m,0xfc);assert(m.pixel(0,0));
    m.index=0x300;memset(m.memory+0x300,0xff,32);opcode(m,0xd010);assert(m.pixel(15,15));opcode(m,0xd010);assert(m.v[15]==1&&!m.pixel(15,15));
    assert(key_index('q')==4&&key_index('V')==15&&key_index(27)==-1);
    assembler a;uint8_t rom[3584];size_t n;
    const char *source="start:\nCLS\nLD V0, 7\nLD V1, 8\nADD V0, V1\nLD I, image\nDRW V0, V1, 1\nEXIT\nimage:\nDB 0x80, 0\n";
    assert(a.assemble(source,rom,sizeof rom,n));assert(n==16&&rom[8]==0xa2&&rom[9]==14);
    m.load(rom,n,schip);for(int i=0;i<8&&!m.exited;++i)m.step();assert(m.exited&&m.v[0]==15&&m.pixel(30,16));
    assert(!a.assemble("JP missing\n",rom,sizeof rom,n)&&a.line==1);
    assert(!a.assemble("LD V0, 256\n",rom,sizeof rom,n));
    assert(!a.assemble("x:\nx:\nCLS\n",rom,sizeof rom,n));
    assert(!a.assemble("DB 1\nCLS\n",rom,sizeof rom,n));
    assert(!a.assemble("DW 0xFFFF\n",rom,1,n));
    pico_source_style style;assert(style.next('@')==pico_source_style::comment);assert(style.next('x')==pico_source_style::comment);style.next('\n');assert(style.next('2')==pico_source_style::number);
    // Published opcode/flag ROMs: count their success and failure sprites.
    for(auto test:{"3-corax+","4-flags"})
    {
        std::ifstream f(std::string("dependencies/chip8-test-suite/bin/")+test+".ch8",std::ios::binary);
        std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(f)),{});assert(!bytes.empty());
        for(auto p:{vip,chip48,schip,schip_legacy})
        {
            m.load(bytes.data(),bytes.size(),p);unsigned good=0,bad=0;
            for(int step=0;step<20000;++step)
            {
                if(step%20==0)m.tick();
                unsigned op=(m.memory[m.pc]<<8)|m.memory[m.pc+1];
                if((op&0xf000)==0xd000 && ((op&15)==3 || ((op&15)==4 && m.memory[m.index]==0)) && m.index<=4092 && !m.draw_wait)
                {
                    auto *s=m.memory+m.index;
                    if((op&15)==4)++s;
                    if(s[0]==0xa0&&s[1]==0xc0&&s[2]==0x80)++good;
                    if(s[0]==0xa0&&s[1]==0x40&&s[2]==0xa0)++bad;
                }
                m.step();assert(!m.error);
            }
            printf("%s profile %d: success sprites %u, failure sprites %u\n",test,int(p),good,bad);assert(good>10&&bad==0);
        }
    }
    {
        std::ifstream f("dependencies/chip8-test-suite/bin/5-quirks.ch8",std::ios::binary);
        std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(f)),{});assert(!bytes.empty());
        for(auto p:{vip,schip,schip_legacy})
        {
            m.load(bytes.data(),bytes.size(),p);m.memory[0x1ff]=p==vip?1:p==schip?2:4;unsigned good=0,bad=0;
            for(int step=0;step<200000;++step)
            {
                if(step%100==0)m.tick();assert(m.pc<=4094);
                unsigned op=(m.memory[m.pc]<<8)|m.memory[m.pc+1];
                if((op&0xf00f)==0xd003 && m.index<=4093 && !m.draw_wait)
                {
                    auto *s=m.memory+m.index;
                    if(s[0]==0xa0&&s[1]==0xc0&&s[2]==0x80)++good;
                    if(s[0]==0xa0&&s[1]==0x40&&s[2]==0xa0)++bad;
                }
                m.step();assert(!m.error);
            }
            printf("5-quirks profile %d: success sprites %u, failure sprites %u\n",int(p),good,bad);assert(good==6&&bad==0);
        }
    }
    // Malformed-ROM fuzz exercises all checked address/stack paths under sanitizers.
    uint32_t seed=7;
    for(int trial=0;trial<400;++trial)
    {
        for(auto &b:rom){seed=seed*1664525+1013904223;b=seed>>24;}
        m.load(rom,sizeof rom,schip);for(int step=0;step<100&&!m.error;++step){m.tick();m.step();}
    }
    puts("PASS: CHIP-8 profiles, flags, sprites, key release, assembler errors, syntax colours, published ROMs and malformed inputs");
}
