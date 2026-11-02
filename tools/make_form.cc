#include "form_document.h"
#include <cstdio>
#include <cstring>
int main(int argc,char **argv)
{
    if(argc!=3)return 2;pico_form_document form;if(!strcmp(argv[2],"circle"))form.circle();else if(!strcmp(argv[2],"unitohm")){strcpy(form.fields[0].units,"V");strcpy(form.fields[1].units,"ohm");strcpy(form.output_units,"A");}else if(strcmp(argv[2],"ohm"))return 2;
    uint8_t data[pico_form_document::bytes];size_t size=form.encode(data,sizeof data);if(!size)return 1;
    FILE *file=fopen(argv[1],"wb");if(!file)return 1;bool ok=fwrite(data,1,size,file)==size;if(fclose(file))ok=false;return ok?0:1;
}
