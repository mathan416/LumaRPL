#pragma once
namespace pico_native_menu {
inline int offset(int selected){return selected/8*8;}
inline int shortcut(int key,int selected,int count)
{
    int row=key>='1'&&key<='8'?key-'1':-1;
    int index=offset(selected)+row;
    return row>=0&&index<count?index:-1;
}
}
