extern "C" {
#define main fatfs_fixture_main
#include "fatfs_test.c"
#undef main
}
#include "form_file.h"
static void raw_form(const char *path,const pico_form_document &form)
{uint8_t data[pico_form_document::bytes];size_t n=form.encode(data,sizeof data);FIL f;UINT wrote;assert(pico_f_open(&f,path,FA_WRITE|FA_CREATE_ALWAYS)==FR_OK&&pico_f_write(&f,data,n,&wrote)==FR_OK&&wrote==n&&pico_f_close(&f)==FR_OK);}
int main()
{
    make_disk();FATFS fs;assert(f_mount(&fs,"",1)==FR_OK&&f_mkdir("/db48x")==FR_OK&&pico_f_mkdir("forms")==FR_OK);
    pico_form_document first,second,loaded;second.circle();bool backup;
    assert(pico_form_save("forms/T.form",first)&&pico_form_save("forms/T.form",second));
    put("forms/T.form","corrupt primary");assert(pico_form_load("forms/T.form",loaded,backup)&&backup&&!strcmp(loaded.title,first.title));
    fail_writes=1;assert(!pico_form_save("forms/T.form",second));fail_writes=0;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_OK);
    assert(pico_form_load("forms/T.form",loaded,backup)&&backup&&!strcmp(loaded.title,first.title));
    assert(pico_form_save("forms/T.form",second));put("forms/T.form","corrupt again");assert(pico_form_load("forms/T.form",loaded,backup)&&backup&&!strcmp(loaded.title,first.title));
    raw_form("forms/INT.form.rollback",second);raw_form("forms/INT.form.bak",first);
    assert(pico_form_load("forms/INT.form",loaded,backup)&&backup&&!strcmp(loaded.title,second.title));FILINFO info;assert(pico_f_stat("forms/INT.form",&info)==FR_OK&&pico_f_stat("forms/INT.form.rollback",&info)==FR_NO_FILE);
    raw_form("forms/INT.form.rollback",first);put("forms/INT.form","broken");assert(pico_form_load("forms/INT.form",loaded,backup)&&backup&&!strcmp(loaded.title,first.title));
    raw_form("forms/INT.form.rollback",second);put("forms/INT.form.rollback","bad rollback");put("forms/INT.form","bad primary");assert(pico_form_load("forms/INT.form",loaded,backup)&&backup&&!strcmp(loaded.title,first.title));
    free(disk);puts("PASS: actual FatFS recovered-backup failed/new saves and interrupted-commit rollback recovery");
}
