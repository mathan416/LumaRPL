#include "data_table.h"
#include <cassert>
#include <cstdio>
int main()
{
    pico_data_table table;table.demo();char output[16385];auto n=table.csv(output,sizeof output);assert(n);
    pico_data_table loaded;assert(loaded.parse_csv(output,n)&&loaded.rows==6&&loaded.points[5][1]==11);
    const char *good="\xef\xbb\xbfX,Y\r\n\"1\",\"2\"\r\n 3 , 4 \n";
    assert(loaded.parse_csv(good,strlen(good))&&loaded.rows==2&&loaded.points[1][1]==4);
    const char *bad[]={"1\n","1,2,3\n","nan,2\n","1,inf\n","1e999,2\n","\"1,2\n","1,\n","1,2junk\n"};
    for(auto text:bad){assert(!loaded.parse_csv(text,strlen(text))&&loaded.rows==2&&loaded.points[1][1]==4);}
    for(unsigned i=0;i<pico_data_table::capacity;++i){table.points[i][0]=i;table.points[i][1]=-i*.5;}table.rows=pico_data_table::capacity;
    n=table.csv(output,sizeof output);assert(loaded.parse_csv(output,n)&&loaded.rows==128);strcat(output,"1,2\n");assert(!loaded.parse_csv(output,strlen(output))&&loaded.rows==128);
    table.erase(0);assert(table.rows==127&&table.points[0][0]==1);
    puts("PASS: bounded CSV import/export, quoted values, CRLF/BOM, malformed/ragged/nonfinite rejection, atomic import and row limits");
}
