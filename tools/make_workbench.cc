#include "workbench.h"
#include <cstdio>
#include <cstring>
int main(int argc,char **argv)
{
    if(argc!=3)return 2;pico_workbench work;
    if(!strcmp(argv[2],"system"))
    {
        work.count=3;strcpy(work.equations[0],"X+Y+Z=6");strcpy(work.equations[1],"X-Y=0");strcpy(work.equations[2],"Z=2");
    }
    else if(strcmp(argv[2],"waves")&&strcmp(argv[2],"solve"))return 2;
    uint8_t data[pico_workbench::bytes];size_t n=work.encode(data,sizeof data);if(!n)return 1;
    FILE *file=fopen(argv[1],"wb");if(!file)return 1;bool ok=fwrite(data,1,n,file)==n;if(fclose(file))ok=false;return ok?0:1;
}
