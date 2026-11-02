extern "C" {
#define main fatfs_baseline_main
#include "fatfs_test.c"
#undef main
}
#include "workbench_file.h"
int main()
{
    make_disk();FATFS fs;assert(f_mount(&fs,"",1)==FR_OK);
    assert(f_mkdir("/db48x")==FR_OK&&pico_f_mkdir("workbenches")==FR_OK);
    pico_workbench old,next,loaded;strcpy(next.single,"X^2=4");bool backup;
    const char *path="workbenches/TEST.workbench";
    assert(pico_workbench_save(path,old));assert(pico_workbench_save(path,next));
    assert(pico_workbench_load(path,loaded,backup)&&!backup&&!strcmp(loaded.single,"X^2=4"));
    fail_writes=1;assert(!pico_workbench_save(path,old));fail_writes=0;
    assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_OK);
    assert(pico_workbench_load(path,loaded,backup)&&!strcmp(loaded.single,"X^2=4"));
    put(path,"corrupt primary");
    assert(pico_workbench_load(path,loaded,backup)&&backup&&!strcmp(loaded.single,old.single));
    assert(pico_f_unlink(path)==FR_OK);
    assert(pico_workbench_load(path,loaded,backup)&&backup);
    assert(pico_f_unlink("workbenches/TEST.workbench.bak")==FR_OK);
    strcpy(loaded.single,"X=99");assert(!pico_workbench_load(path,loaded,backup)&&!strcmp(loaded.single,"X=99"));
    missing=1;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_NOT_READY);
    assert(!pico_workbench_save(path,old)&&!pico_workbench_load(path,loaded,backup));
    free(disk);
    puts("PASS: workbench actual FatFS save/load, backup recovery, failed write, missing files/card and current-state preservation");
}
