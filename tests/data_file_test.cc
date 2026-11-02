extern "C" {
#define main fatfs_baseline_main
#include "fatfs_test.c"
#undef main
}
#include "data_file.h"
int main()
{
    make_disk();FATFS fs;assert(f_mount(&fs,"",1)==FR_OK);assert(f_mkdir("/db48x")==FR_OK&&pico_f_mkdir("datasets")==FR_OK);
    pico_data_table old,next,loaded;old.demo();next=old;next.points[0][1]=99;bool backup;const char *path="datasets/TEST.csv";
    assert(pico_data_save(path,old)&&pico_data_save(path,next));assert(pico_data_load(path,loaded,backup)&&!backup&&loaded.points[0][1]==99);
    fail_writes=1;assert(!pico_data_save(path,old));fail_writes=0;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_OK);
    assert(pico_data_load(path,loaded,backup)&&loaded.points[0][1]==99);put(path,"invalid");assert(pico_data_load(path,loaded,backup)&&backup&&loaded.points[0][1]==1);
    missing=1;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_NOT_READY);assert(!pico_data_load(path,loaded,backup)&&loaded.rows==6);free(disk);
    puts("PASS: actual FatFS CSV replacement/backup, failed writes, invalid primary recovery and missing card preservation");
}
