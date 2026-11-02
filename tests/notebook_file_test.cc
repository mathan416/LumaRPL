extern "C" {
#define main fatfs_baseline_main
#include "fatfs_test.c"
#undef main
}
#include "notebook_file.h"
int main()
{
    make_disk();FATFS fs;assert(f_mount(&fs,"",1)==FR_OK);
    assert(f_mkdir("/db48x")==FR_OK&&pico_f_mkdir("notebooks")==FR_OK);
    pico_notebook old,next,loaded;assert(next.set_expression(0,"7+8"));bool backup;
    const char *path="notebooks/TEST.notebook";
    assert(pico_notebook_save(path,old)&&pico_notebook_save(path,next));
    assert(pico_notebook_load(path,loaded,backup)&&!backup&&!strcmp(loaded.cells[0].expression,"7+8"));
    fail_writes=1;assert(!pico_notebook_save(path,old));fail_writes=0;
    assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_OK);
    assert(pico_notebook_load(path,loaded,backup)&&!strcmp(loaded.cells[0].expression,"7+8"));
    put(path,"corrupt primary");assert(pico_notebook_load(path,loaded,backup)&&backup&&!strcmp(loaded.cells[0].expression,"2+3"));
    assert(pico_f_unlink(path)==FR_OK&&pico_notebook_load(path,loaded,backup)&&backup);
    assert(pico_f_unlink("notebooks/TEST.notebook.bak")==FR_OK);assert(loaded.set_expression(0,"9+9"));
    strcpy(loaded.cells[0].preview,"18");loaded.cells[0].status=pico_notebook::state::complete;
    assert(!pico_notebook_load(path,loaded,backup)&&!strcmp(loaded.cells[0].expression,"9+9")&&!strcmp(loaded.cells[0].preview,"18"));
    missing=1;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_NOT_READY);
    assert(!pico_notebook_save(path,old)&&!pico_notebook_load(path,loaded,backup));
    free(disk);puts("PASS: notebook actual FatFS transactions, failed writes, backup recovery, missing card and current state preserved");
}
