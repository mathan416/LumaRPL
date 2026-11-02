#include "workbench.h"
#include "native_menu.h"
#include <cassert>
#include <cstdio>
int main()
{
    assert(pico_native_menu::shortcut(0x81,8,10)==-1); // F keys are actions, never row shortcuts.
    assert(pico_native_menu::shortcut('1',8,10)==8);
    assert(pico_native_menu::shortcut('2',9,10)==9);
    assert(pico_native_menu::shortcut('3',8,10)==-1);
    assert(pico_native_menu::shortcut(0x85,8,10)==-1);
    assert(pico_native_menu::shortcut('8',7,10)==7);
    pico_workbench work;uint8_t data[2048];assert(work.valid());
    auto length=work.encode(data,sizeof data);assert(length==pico_workbench::bytes);
    pico_workbench copy;strcpy(copy.single,"X=2");assert(copy.decode(data,length)&&!strcmp(copy.single,work.single));
    data[90]^=1;assert(!copy.decode(data,length)&&!strcmp(copy.single,work.single));data[90]^=1;
    assert(!copy.decode(data,length-1));data[4]=2;assert(!copy.decode(data,length));data[4]=1;
    work.parameters[0].value=4.99;work.adjust(0,1);assert(work.parameters[0].value==5);work.adjust(0,1);assert(work.parameters[0].value==5);
    work.parameters[0].value=-4.99;work.adjust(0,-1);assert(work.parameters[0].value==-5);
    work.parameters[1].step=0;assert(!work.valid()&&!work.encode(data,sizeof data));
    work.parameters[1].step=.1;work.count=4;assert(!work.valid());work.count=3;
    memset(work.single,'X',160);assert(!work.valid());
    puts("PASS: workbench codec, checksum/version/length rejection, atomic load validation and slider clamps");
}
