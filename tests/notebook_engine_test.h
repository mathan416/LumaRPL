#pragma once
#include "notebook_engine.h"
inline void pico_notebook_engine_tests(void (*check)(bool,const char *))
{
    auto precision=Settings.Precision();bool hardware=Settings.HardwareFloatingPoint();auto directories=rt.directories();size_t depth=rt.depth(),variables=rt.variables(0)->count();
    symbol_g result_name=symbol::make("R1");object_g old_result=directory::recall_all(result_name,false);
    if(old_result)old_result=rt.clone(old_result);
    object_g sentinel=integer::make(77);check(result_name&&sentinel&&directory::store_here(result_name,sentinel),"prepare existing notebook result variable");
    pico_notebook book;int failed;bool cancelled;
    {
        settings::SaveHardwareFloatingPoint h(false);settings::SaveNumericalResults n(false);settings::SaveNumericalConstants c(false);settings::SavePrecision p(25);
        object_g result=pico_notebook_run(book,2,failed,cancelled);
        check(result&&failed==-1&&!cancelled&&book.cells[2].status==pico_notebook::state::complete,"notebook chained exact run");
        check(result->is_fraction(),"notebook preserves exact fractions");
        rt.gc();check(result->is_fraction(),"notebook result survives scope cleanup and GC");
        check(Settings.Precision()==25&&!Settings.HardwareFloatingPoint(),"notebook preserves user arithmetic settings");
    }
    strcpy(book.cells[1].expression,"R1");object_g alias=pico_notebook_run(book,1,failed,cancelled);rt.gc();
    check(alias&&alias->as_int32(0,false)==5,"notebook copies a global-bound final result before scope cleanup");
    char preview[8];const char *unicode="αβγδε";pico_notebook_preview(preview,sizeof preview,unicode,strlen(unicode));
    check(strlen(preview)<sizeof preview&&pico_text_editor::valid_utf8(preview,strlen(preview)),"notebook preview clipping preserves UTF-8");
    strcpy(book.cells[0].expression,"R2+1");object_g bad=pico_notebook_run(book,2,failed,cancelled);
    check(!bad&&failed==0&&book.cells[0].status==pico_notebook::state::error&&book.cells[1].status==pico_notebook::state::not_run,"notebook rejects stale future result");
    strcpy(book.cells[0].expression,"2+3");strcpy(book.cells[1].expression,"1/0");bad=pico_notebook_run(book,2,failed,cancelled);
    check(!bad&&failed==1&&book.cells[0].status==pico_notebook::state::complete&&book.cells[2].status==pico_notebook::state::not_run,"notebook stops at first failed expression");rt.clear_error();
    bool stop=true;auto poll=[](void *opaque)->bool{return *static_cast<bool *>(opaque);};bad=pico_notebook_run(book,2,failed,cancelled,poll,&stop);
    check(!bad&&cancelled&&failed==0,"notebook cancellation before cell evaluation");
    auto exhaust=[](void *)->bool{rt.allocate(1024*1024);return false;};bad=pico_notebook_run(book,0,failed,cancelled,exhaust,nullptr);
    check(!bad&&failed==0,"notebook controlled allocation failure");rt.clear_error();
    object_g preserved=directory::recall_all(result_name,false);check(preserved&&preserved->as_int32(0,false)==77,"notebook keeps preexisting R1 variable");
    if(old_result)check(directory::store_here(result_name,old_result),"restore existing result variable");
    else const_cast<directory *>(rt.variables(0))->purge(result_name,true);
    check(rt.depth()==depth&&rt.directories()==directories&&rt.variables(0)->count()==variables,"notebook restores stack and temporary bindings");
    check(Settings.Precision()==precision&&Settings.HardwareFloatingPoint()==hardware,"notebook restores caller settings");
    puts("PASS: notebook exact chained evaluation, failure/cancellation, GC and calculator-state preservation");
}
