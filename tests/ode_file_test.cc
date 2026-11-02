extern "C" {
#define main fatfs_fixture_main
#include "fatfs_test.c"
#undef main
}
#include "ode_file.h"
#include "graph_export.h"
int main()
{
    make_disk();FATFS fs;assert(f_mount(&fs,"",1)==FR_OK&&f_mkdir("/db48x")==FR_OK&&pico_f_mkdir("odes")==FR_OK&&pico_f_mkdir("exports")==FR_OK);
    pico_ode_scene original,updated,loaded;updated.y0=2;bool backup;const char *path="odes/EXP.ode";
    assert(pico_ode_save(path,original)&&pico_ode_save(path,updated)&&pico_ode_load(path,loaded,backup)&&!backup&&loaded.y0==2);
    fail_writes=1;assert(!pico_ode_save(path,original));fail_writes=0;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_OK);assert(pico_ode_load(path,loaded,backup)&&loaded.y0==2);
    put(path,"bad");assert(pico_ode_load(path,loaded,backup)&&backup&&loaded.y0==1);
    {pico_graph_export file;assert(file.open("exports/G.svg"));file.svg.begin("test",0,0xffffff);assert(file.svg.finish()&&file.commit());}
    {pico_graph_export file;assert(file.open("exports/G.svg"));fail_writes=1;file.put("replacement");assert(!file.commit());fail_writes=0;}
    assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_OK);char data[1024];size_t n;assert(pico_resource_read("exports/G.svg",reinterpret_cast<uint8_t*>(data),sizeof data,n)&&n>100&&!memcmp(data,"<?xml",5));
    {pico_graph_export file;assert(file.open("exports/G.svg",[](){return true;}));assert(!file.put("cancelled")&&!file.commit()&&file.cancelled);}
    missing=1;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_NOT_READY);assert(!pico_ode_save(path,original));{pico_graph_export file;assert(!file.open("exports/G.svg"));}free(disk);
    puts("PASS: actual FatFS ODE and SVG export/failed replacement/backup/cancellation/missing SD");
}
