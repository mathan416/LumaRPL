extern "C" {
#define main fatfs_baseline_main
#include "fatfs_test.c"
#undef main
}
#include "currency_file.h"
int main()
{
    make_disk();FATFS fs;assert(f_mount(&fs,"",1)==FR_OK&&f_mkdir("/db48x")==FR_OK&&pico_f_mkdir("currency")==FR_OK);
    pico_currency_rates old,next,loaded;old.count=1;strcpy(old.rows[0].code,"CAD");strcpy(old.rows[0].date,"2026-10-30");strcpy(old.rows[0].rate,"1.5");next=old;strcpy(next.rows[0].rate,"1.6");bool backup;
    const char *path="currency/CURRENT.rates";assert(pico_currency_save(path,old)&&pico_currency_save(path,next));assert(pico_currency_load(path,loaded,backup)&&!backup&&!strcmp(loaded.rows[0].rate,"1.6"));
    fail_writes=1;assert(!pico_currency_save(path,old));fail_writes=0;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_OK);assert(pico_currency_load(path,loaded,backup)&&!strcmp(loaded.rows[0].rate,"1.6"));
    put(path,"corrupt");assert(pico_currency_load(path,loaded,backup)&&backup&&!strcmp(loaded.rows[0].rate,"1.5"));fail_writes=1;assert(!pico_currency_save(path,next));fail_writes=0;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_OK);assert(pico_currency_load(path,loaded,backup)&&backup&&!strcmp(loaded.rows[0].rate,"1.5"));
    assert(pico_currency_save(path,next));assert(pico_f_unlink(path)==FR_OK&&pico_f_unlink("currency/CURRENT.rates.bak")==FR_OK);strcpy(loaded.provider,"Kept");assert(!pico_currency_load(path,loaded,backup)&&!strcmp(loaded.provider,"Kept"));
    missing=1;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_NOT_READY);assert(!pico_currency_save(path,old)&&!pico_currency_load(path,loaded,backup));free(disk);puts("PASS: currency actual FatFS roundtrip, corrupt recovery, failed saves and missing SD");
}
