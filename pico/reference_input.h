#pragma once
#include "input.h"
// Native readers consume their own raw keys; Ctrl+K opens Find from the stack.
template<class Interface,class Find>
bool pico_reference_input(Interface &,int key,Find find)
{
 if(key!=PICO_REFERENCE_FIND)return false;
 find();return true;
}
