#include "form_document.h"
#include <cassert>
#include <cstdio>
int main()
{
    pico_form_document form,loaded;uint8_t data[pico_form_document::bytes];assert(form.valid());size_t size=form.encode(data,sizeof data);assert(size==pico_form_document::bytes);
    assert(loaded.decode(data,size)&&!strcmp(loaded.formula,form.formula));
    for(size_t i=0;i<size;++i){data[i]^=1;assert(!loaded.decode(data,size));data[i]^=1;}
    assert(!loaded.decode(data,size-1));strcpy(loaded.title,"Keep this");data[4]=9;assert(!loaded.decode(data,size)&&!strcmp(loaded.title,"Keep this"));data[4]=2;
    strcpy(form.fields[1].name,"voltage");assert(!form.valid());strcpy(form.fields[1].name,"RESISTANCE");
    strcpy(form.fields[0].name,"2INPUT");assert(!form.valid());strcpy(form.fields[0].name,"VOLTAGE");
    memset(form.fields[0].input,'1',sizeof form.fields[0].input);assert(!form.valid());strcpy(form.fields[0].input,"12");
    form.count=7;assert(!form.valid());form.count=0;assert(form.valid());
    form.circle();assert(form.valid()&&form.count==1&&!strcmp(form.formula,"PI*RADIUS^2"));
    assert(form.encode(data,sizeof data)==size);assert(loaded.decode(data,size)&&!strcmp(loaded.fields[0].name,"RADIUS"));
    FILE *legacy=fopen("resources/forms/OHMS.form","rb");if(!legacy)legacy=fopen("resources/forms/OHM.form","rb");assert(legacy);uint8_t old[pico_form_document::legacy_bytes];assert(fread(old,1,sizeof old,legacy)==sizeof old);fclose(legacy);assert(loaded.decode(old,sizeof old)&&!loaded.fields[0].units[0]&&!loaded.output_units[0]);
    strcpy(form.fields[0].units,"cm");strcpy(form.output_units,"m^2");assert(form.encode(data,sizeof data)==size&&loaded.decode(data,size)&&!strcmp(loaded.fields[0].units,"cm")&&!strcmp(loaded.output_units,"m^2"));
    puts("PASS: form names, field limits, codec/version/checksum rejection, atomic load and examples");
}
