#include "notebook.h"
#include <cassert>
#include <cstdio>
int main()
{
    pico_notebook book;uint8_t bytes[pico_notebook::bytes];assert(book.valid());
    assert(book.encode(bytes,sizeof bytes)==sizeof bytes);
    book.cells[0].status=pico_notebook::state::complete;strcpy(book.cells[0].preview,"5");
    pico_notebook copy;assert(copy.decode(bytes,sizeof bytes));assert(!copy.cells[0].preview[0]&&copy.cells[0].status==pico_notebook::state::not_run);
    assert(!copy.decode(bytes,sizeof bytes-1));bytes[100]^=1;assert(!copy.decode(bytes,sizeof bytes)&&!strcmp(copy.cells[0].expression,"2+3"));bytes[100]^=1;
    copy.cells[1].status=pico_notebook::state::complete;strcpy(copy.cells[1].preview,"25");assert(copy.set_expression(0,"3+4"));assert(copy.cells[1].status==pico_notebook::state::not_run&&!copy.cells[1].preview[0]);
    char long_input[162];memset(long_input,'X',161);long_input[161]=0;assert(!copy.set_expression(0,long_input));
    assert(copy.set_note(0,"Line one\nLine two"));assert(!copy.set_note(0,"Bad\x01"));
    assert(!pico_notebook::prior_results_only("R1+2",0));assert(pico_notebook::prior_results_only("R1+2",1));assert(!pico_notebook::prior_results_only("R12+2",11));
    assert(pico_notebook::prior_results_only("R12+2",12));assert(pico_notebook::prior_results_only("R123+2",0));assert(pico_notebook::prior_results_only("AR1+2",0));assert(pico_notebook::prior_results_only("\"R12\"",0));
    assert(!pico_notebook::prior_results_only("r2",1));assert(pico_notebook::prior_results_only("r2",1,false));
    while(copy.count<pico_notebook::capacity)assert(copy.append());assert(!copy.append());
    assert(copy.erase(0)&&copy.count==11);while(copy.count>1)assert(copy.erase(0));assert(!copy.erase(0));
    bytes[4]=2;assert(!book.decode(bytes,sizeof bytes));
    puts("PASS: notebook codec, volatile results, invalidation, bounds, notes and previous-result references");
}
