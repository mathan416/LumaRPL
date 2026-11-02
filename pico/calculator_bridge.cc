#include "program.h"
#include "expression.h"
#include "settings.h"
#include "runtime.h"
#include "lua_runner.h"
#include <cstdio>
#include <cstring>

extern "C" int pico_lua_eval(const char *source,char *output,size_t capacity)
{
    char wrapped[256];bool ok=false;
    if(strlen(source)>240||strchr(source,'\'')){snprintf(output,capacity,"Use one expression, without quotes");return 0;}
    stack_depth_restore stack;save<bool> halted(program::halted,false);settings::SaveDecimalComma comma(false);
    snprintf(wrapped,sizeof wrapped,"'%s'",source);program_g parsed=program::parse(utf8(wrapped),strlen(wrapped));program_g expression;
    if(parsed)for(object_p item:*parsed){if(expression||item->type()!=object::ID_expression){expression=nullptr;break;}expression=expression_p(item);}
    if(expression && expression->run()==object::OK && rt.depth()==stack.depth+1)
    {
        object_g value=rt.pop();size_t n=value->render(output,capacity);output[n<capacity?n:capacity-1]=0;ok=n<capacity;
        if(!ok)snprintf(output,capacity,"Calculator result is too long");
    }
    else snprintf(output,capacity,"%s",rt.error()?cstring(rt.error()):"Expression must return one result");
    rt.clear_error();return ok;
}
