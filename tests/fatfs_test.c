// Exercise the actual writable FatFS and transaction layer on a FAT16 RAM disk.
#include "storage.h"
#include "diskio.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define SECTORS 16384
static uint8_t *disk;
static int missing,fail_writes;
DSTATUS disk_initialize(BYTE d) { return d||missing?STA_NOINIT:0; }
DSTATUS disk_status(BYTE d) { return d||missing?STA_NOINIT:0; }
DRESULT disk_read(BYTE d,BYTE *p,LBA_t sector,UINT count)
{
    if(d||missing) return RES_NOTRDY;
    if(sector+count>SECTORS) return RES_PARERR;
    memcpy(p,disk+sector*512,count*512);return RES_OK;
}
DRESULT disk_write(BYTE d,const BYTE *p,LBA_t sector,UINT count)
{
    if(d||missing) return RES_NOTRDY;
    if(fail_writes) return RES_ERROR;
    if(sector+count>SECTORS) return RES_PARERR;
    memcpy(disk+sector*512,p,count*512);return RES_OK;
}
DRESULT disk_ioctl(BYTE d,BYTE cmd,void *p)
{
    if(d||missing) return RES_NOTRDY;
    if(cmd==CTRL_SYNC) return fail_writes?RES_ERROR:RES_OK;
    if(cmd==GET_SECTOR_COUNT) { *(LBA_t*)p=SECTORS;return RES_OK; }
    if(cmd==GET_BLOCK_SIZE) { *(DWORD*)p=1;return RES_OK; }
    return RES_PARERR;
}
static void word(unsigned offset,unsigned value)
{ disk[offset]=value;disk[offset+1]=value>>8; }
static void make_disk(void)
{
    disk=(uint8_t*)calloc(SECTORS,512);assert(disk);
    disk[0]=0xeb;disk[1]=0x3c;disk[2]=0x90;
    memcpy(disk+3,"MSDOS5.0",8);
    word(11,512);disk[13]=1;word(14,1);disk[16]=2;
    word(17,512);word(19,SECTORS);disk[21]=0xf8;word(22,64);
    word(24,32);word(26,64);disk[38]=0x29;
    memcpy(disk+54,"FAT16   ",8);word(510,0xaa55);
    word(512,0xfff8);word(514,0xffff);
    word(65*512,0xfff8);word(65*512+2,0xffff);
}
static void put(const char *path,const char *text)
{
    FIL f={0};UINT bytes;
    assert(pico_f_open(&f,path,FA_WRITE|FA_CREATE_ALWAYS)==FR_OK);
    assert(pico_f_write(&f,text,strlen(text),&bytes)==FR_OK);
    assert(bytes==strlen(text));assert(pico_f_close(&f)==FR_OK);
}
static void expect(const char *path,const char *expected)
{
    FIL f={0};UINT bytes;char buffer[64]={0};
    assert(pico_f_open(&f,path,FA_READ)==FR_OK);
    assert(pico_f_read(&f,buffer,sizeof buffer-1,&bytes)==FR_OK);
    assert(!strcmp(buffer,expected));assert(pico_f_close(&f)==FR_OK);
}
int main(void)
{
    make_disk();FATFS fs;
    assert(f_mount(&fs,"",1)==FR_OK);
    assert(f_mkdir("/db48x")==FR_OK);
    assert(pico_f_mkdir("state")==FR_OK);
    pico_io_reset();put("state/Session.48S","old session");
    put("state/Session.48S.tmp","new session");
    assert(pico_commit_state("state/Session.48S.tmp","state/Session.48S"));
    expect("state/Session.48S","new session");
    expect("state/Session.48S.bak","old session");
    // Commit requires successful data/directory flushing.
    pico_io_reset();FIL f={0};UINT bytes;
    assert(pico_f_open(&f,"state/Session.48S.tmp",FA_WRITE|FA_CREATE_ALWAYS)==FR_OK);
    fail_writes=1;
    pico_f_write(&f,"broken",6,&bytes);
    pico_f_close(&f);
    assert(pico_io_failed());
    assert(!pico_commit_state("state/Session.48S.tmp","state/Session.48S"));
    fail_writes=0;
    assert(f_mount(NULL,"",0)==FR_OK);
    assert(f_mount(&fs,"",1)==FR_OK);
    expect("state/Session.48S","new session");
    expect("state/Session.48S.bak","old session");
    missing=1;
    assert(f_mount(NULL,"",0)==FR_OK);
    assert(f_mount(&fs,"",1)==FR_NOT_READY);
    free(disk);
    puts("PASS: actual FatFS FAT16 save/backup, failed flush, remount and missing media");
    return 0;
}
