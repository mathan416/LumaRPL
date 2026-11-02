#pragma once
#include "scoped_array.h"
inline void run_port16_engine_tests()
{
    stack_depth_restore stack;cleaner temporary;
    {scoped_array<algebraic_g> roots(64);surface_check(!!roots,"scratch root allocation");for(unsigned i=0;i<64;++i)roots[i]=integer::make(i);rt.gc();surface_check(roots[63]&&roots[63]->as_int32(0,false)==63,"heap scratch GC roots survive moving collection");}
    const char *source="{0 0 0 0 0 0 0 0 0 0} 1 10 START DUP + NEXT LNAME";
    program_g parsed=program::parse(utf8(source),strlen(source));surface_check(parsed&&parsed->run()==object::OK,"10240-element LNAME uses fallible scratch");
    object_g result=rt.pop();surface_check(result&&result->as<array>()&&list_p(+result)->items()==0,"large LNAME result");
    puts("PASS: real engine 10240-element names and moving-GC scratch roots");
}
