extern "C" {
#define main fatfs_baseline_main
#include "fatfs_test.c"
#undef main
}
#include "timezone_file.h"
int main()
{
    make_disk();FATFS fs;assert(f_mount(&fs,"",1)==FR_OK&&f_mkdir("/db48x")==FR_OK&&pico_f_mkdir("network")==FR_OK);
    pico_timezone toronto,utc;utc.minutes=0;utc.rule=0;assert(pico_timezone_save(toronto)&&pico_timezone_save(utc));pico_timezone loaded;assert(pico_timezone_load(loaded)&&loaded.minutes==0&&loaded.rule==0);
    put("network/TIMEZONE.cfg","corrupt");assert(pico_timezone_load(loaded)&&loaded.minutes==-300&&loaded.rule==pico_timezone::north_america);
    fail_writes=1;assert(!pico_timezone_save(utc));fail_writes=0;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_OK);assert(pico_timezone_load(loaded)&&loaded.minutes==-300);
    assert(pico_timezone_save(utc)&&pico_timezone_load(loaded)&&loaded.minutes==0);
    missing=1;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_NOT_READY);assert(!pico_timezone_save(toronto));assert(!pico_timezone_load(loaded)&&loaded.minutes==0);free(disk);puts("PASS: timezone actual FatFS persistence, backup recovery, failed saves and missing SD");
}
