#include "storage.h"
#include "diskio.h"
#include "sd_read.h"
extern "C" {
#define main fatfs_fixture_main
#define disk_read baseline_disk_read
#include "fatfs_test.c"
#undef disk_read
#undef main
}
#include "workbench_file.h"
static uint32_t selected_sector;static unsigned commands,packets,finishes;
static int read_command(void *,uint32_t address){selected_sector=address;++commands;return address<SECTORS;}
static int read_packet(void *,uint8_t *buffer,unsigned count)
{assert(count==512);++packets;memcpy(buffer,disk+selected_sector*512,512);return 1;}
static void read_finish(void *){++finishes;}
extern "C" DRESULT disk_read(BYTE device,BYTE *buffer,LBA_t sector,UINT count)
{
    if(device||missing)return RES_NOTRDY;
    return pico_sd_read_sectors(buffer,uint32_t(sector),count,1,nullptr,read_command,read_packet,read_finish)?RES_OK:RES_ERROR;
}
int main()
{
    make_disk();FATFS fs;assert(f_mount(&fs,"",1)==FR_OK);
    assert(f_mkdir("/db48x")==FR_OK&&pico_f_mkdir("workbenches")==FR_OK);
    pico_workbench work,loaded;work.count=3;bool backup;pico_workbench_diagnostic diagnostic;
    const char *path="workbenches/SYSTEM3.workbench";assert(pico_workbench_save(path,work));
    for(int attempt=0;attempt<8;++attempt)
    {
        assert(pico_workbench_load(path,loaded,backup,&diagnostic)&&!backup&&loaded.count==3);
        assert(diagnostic.primary.bytes==1564&&!strcmp(diagnostic.primary_decode,"Valid"));
        DIR dir;assert(pico_f_opendir(&dir,"workbenches")==FR_OK);FILINFO info;
        assert(f_readdir(&dir,&info)==FR_OK&&*info.fname);assert(f_closedir(&dir)==FR_OK);
    }
    assert(commands==packets&&packets==finishes&&commands>=32);
    free(disk);
    puts("PASS: actual FatFS workbench saves/repeated 1564-byte loads and directory reads over checked sector transport");
}
