#include "notebook.h"
#include <cstdio>
#include <cstring>
int main(int argc,char **argv)
{
    if(argc!=3)return 2;pico_notebook book;
    if(!strcmp(argv[2],"fractions")){strcpy(book.title,"Exact fractions");strcpy(book.cells[0].expression,"1/3+1/6");strcpy(book.cells[1].expression,"R1^2");strcpy(book.cells[2].expression,"1/R2");strcpy(book.cells[0].note,"Use exact arithmetic for a rational result");}
    else if(strcmp(argv[2],"chain"))return 2;
    unsigned char data[pico_notebook::bytes];size_t n=book.encode(data,sizeof data);if(!n)return 1;
    FILE *file=fopen(argv[1],"wb");if(!file)return 1;bool ok=fwrite(data,1,n,file)==n;if(fclose(file))ok=false;return ok?0:1;
}
