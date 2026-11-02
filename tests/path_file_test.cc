extern "C" {
#define main fatfs_fixture_main
#include "fatfs_test.c"
#undef main
}
#include "path_file.h"
int main()
{
    make_disk();FATFS fs;assert(f_mount(&fs,"",1)==FR_OK&&f_mkdir("/db48x")==FR_OK&&pico_f_mkdir("paths")==FR_OK);
    pico_path_scene original,updated,loaded;updated.polar=1;bool backup;
    const char *path="paths/ROSE.path";assert(pico_path_save(path,original)&&pico_path_save(path,updated));
    assert(pico_path_load(path,loaded,backup)&&!backup&&loaded.polar==1);
    fail_writes=1;assert(!pico_path_save(path,original));fail_writes=0;
    assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_OK);
    assert(pico_path_load(path,loaded,backup)&&loaded.polar==1);
    put(path,"bad checksum");assert(pico_path_load(path,loaded,backup)&&backup&&loaded.polar==0);
    assert(pico_f_unlink(path)==FR_OK&&pico_path_load(path,loaded,backup)&&backup);
    missing=1;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_NOT_READY);
    assert(!pico_path_save(path,original)&&!pico_path_load(path,loaded,backup));free(disk);
    puts("PASS: native path actual FatFS save/load, failed writes, corrupt/missing primary recovery and missing card");
}
