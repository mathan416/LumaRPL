extern "C" {
#define main fatfs_baseline_main
#include "fatfs_test.c"
#undef main
}
#include "form_file.h"
int main()
{
    make_disk();FATFS fs;assert(f_mount(&fs,"",1)==FR_OK);assert(f_mkdir("/db48x")==FR_OK&&pico_f_mkdir("forms")==FR_OK);
    pico_form_document first,second,loaded;second.circle();bool backup=false;const char *path="forms/TEST.form";
    assert(pico_form_save(path,first)&&pico_form_save(path,second));assert(pico_form_load(path,loaded,backup)&&!backup&&!strcmp(loaded.title,"Circle area"));
    fail_writes=1;assert(!pico_form_save(path,first));fail_writes=0;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_OK);
    assert(pico_form_load(path,loaded,backup)&&!strcmp(loaded.title,"Circle area"));put(path,"bad primary");
    assert(pico_form_load(path,loaded,backup)&&backup&&!strcmp(loaded.title,first.title));
    assert(pico_f_unlink(path)==FR_OK);assert(pico_form_load(path,loaded,backup)&&backup);
    assert(pico_f_unlink("forms/TEST.form.bak")==FR_OK);strcpy(loaded.title,"Keep current");assert(!pico_form_load(path,loaded,backup)&&!strcmp(loaded.title,"Keep current"));
    missing=1;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_NOT_READY);assert(!pico_form_save(path,first)&&!pico_form_load(path,loaded,backup));
    free(disk);puts("PASS: actual FatFS form save, failed replacement, corrupt/missing primary recovery and missing card");
}
