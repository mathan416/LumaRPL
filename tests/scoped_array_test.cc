#include "scoped_array.h"
#include <cassert>
#include <cstdio>
#include <limits>
struct Root {static int live;Root(){++live;}~Root(){--live;}};int Root::live=0;
int main()
{
    {scoped_array<Root> roots(10240);assert(!!roots&&Root::live==10240);}
    assert(!Root::live);scoped_array<int> empty(0);assert(!!empty);
    scoped_array<int> overflow(SIZE_MAX);assert(!overflow);
    scoped_array<int> large(10240);assert(!!large);for(int i=0;i<10240;++i)large[i]=i;assert(large[10239]==10239);
    puts("PASS: fallible scratch arrays, 10240 elements, overflow and root lifetimes");
}
