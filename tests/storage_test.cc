#include "storage.h"
#include <cassert>
#include <cstring>
#include <map>
#include <string>
#include <cstdio>
static std::map<std::string,std::string> files;
static std::string opened;
static size_t read_position;
static int fail_rename_at,rename_calls,fail_unlink_at,unlink_calls;
static bool short_write,close_error,rename_error;
extern "C" {
FRESULT f_open(FIL *f,const TCHAR *path,BYTE mode)
{ opened=path;read_position=0;f->flag=mode;return FR_OK; }
FRESULT f_read(FIL*,void *data,UINT bytes,UINT *read) { const auto &s=files[opened];*read=std::min(size_t(bytes),s.size()-read_position);memcpy(data,s.data()+read_position,*read);read_position+=*read;return FR_OK; }
FRESULT f_write(FIL*,const void*,UINT bytes,UINT *written)
{ *written=short_write?bytes-1:bytes;return FR_OK; }
FRESULT f_close(FIL*) { return close_error?FR_DISK_ERR:FR_OK; }
FRESULT f_stat(const TCHAR *path,FILINFO*) { return files.count(path)?FR_OK:FR_NO_FILE; }
FRESULT f_unlink(const TCHAR *path) { if(fail_unlink_at&&++unlink_calls==fail_unlink_at)return FR_DISK_ERR;return files.erase(path)?FR_OK:FR_NO_FILE; }
FRESULT f_mkdir(const TCHAR*) { return FR_OK; }
FRESULT f_opendir(DIR*,const TCHAR*) { return FR_OK; }
FRESULT f_rename(const TCHAR *from,const TCHAR *to)
{
    if(fail_rename_at&&++rename_calls==fail_rename_at)return FR_DISK_ERR;
    if(rename_error && std::string(from).find(".tmp")!=std::string::npos) return FR_DISK_ERR;
    if(!files.count(from)) return FR_NO_FILE;
    if(files.count(to)) return FR_EXIST;
    files[to]=files[from];files.erase(from);return FR_OK;
}
}
int main()
{
    FIL file={};
    assert(pico_f_open(&file,"/help/db48x.md",FA_READ)==FR_OK);
    assert(opened=="/db48x/help/db48x.md");
    assert(pico_f_open(&file,"/db48x/state/a.48S",FA_READ)==FR_OK);
    assert(opened=="/db48x/state/a.48S");
    assert(pico_f_open(&file,"../secret",FA_WRITE)==FR_INVALID_NAME);
    assert(pico_f_open(&file,"state/../../secret",FA_WRITE)==FR_INVALID_NAME);
    assert(pico_f_open(&file,"0:/secret",FA_WRITE)==FR_INVALID_NAME);
    assert(pico_f_open(&file,"a\\b",FA_WRITE)==FR_INVALID_NAME);
    std::string huge(600,'a');
    assert(pico_f_open(&file,huge.c_str(),FA_WRITE)==FR_INVALID_NAME);
    pico_io_reset();short_write=true;UINT written=0;
    pico_f_write(&file,"hello",5,&written);
    assert(pico_io_failed());short_write=false;
    pico_io_reset();close_error=true;pico_f_close(&file);
    assert(pico_io_failed());close_error=false;
    pico_io_reset();
    files["/db48x/state/a.tmp"]="new";files["/db48x/state/a"]="old";
    assert(pico_commit_state("state/a.tmp","state/a"));
    assert(files["/db48x/state/a"]=="new");
    assert(files["/db48x/state/a.bak"]=="old");
    files["/db48x/state/a.tmp"]="replacement";rename_error=true;
    assert(!pico_commit_state("state/a.tmp","state/a"));
    assert(files["/db48x/state/a"]=="new");
    rename_error=false;
    pico_io_reset();short_write=true;pico_f_write(&file,"x",1,&written);
    assert(!pico_commit_state("state/a.tmp","state/a"));
    assert(files["/db48x/state/a"]=="new");
    short_write=false;rename_error=false;
    for(int failure=1;failure<=3;++failure)
    {
        files.clear();pico_io_reset();
        files["/db48x/forms/TEST.form"]="corrupt";files["/db48x/forms/TEST.form.bak"]="last valid";files["/db48x/forms/TEST.form.tmp"]="new valid";
        pico_storage_recovered("forms/TEST.form","forms/TEST.form.bak");
        fail_rename_at=failure;rename_calls=0;bool committed=pico_commit_state("forms/TEST.form.tmp","forms/TEST.form");fail_rename_at=0;
        assert(files["/db48x/forms/TEST.form.bak"]=="last valid");
        if(committed)assert(files["/db48x/forms/TEST.form"]=="new valid");
    }
    for(int step=1;step<=3;++step)
    {
        files.clear();pico_io_reset();files["/db48x/forms/V"]="validated";files["/db48x/forms/V.bak"]="older";files["/db48x/forms/V.tmp"]="replacement";
        pico_storage_verified("forms/V");fail_rename_at=step;rename_calls=0;bool ok=pico_commit_state("forms/V.tmp","forms/V");fail_rename_at=0;
        assert(ok?files["/db48x/forms/V"]=="replacement":files["/db48x/forms/V"]=="validated"||files["/db48x/forms/V.rollback"]=="validated");
    }
    files.clear();pico_io_reset();files["/db48x/forms/U"]="validated";files["/db48x/forms/U.bak"]="older";files["/db48x/forms/U.tmp"]="replacement";pico_storage_verified("forms/U");
    fail_unlink_at=1;unlink_calls=0;assert(pico_commit_state("forms/U.tmp","forms/U"));fail_unlink_at=0;
    assert(files["/db48x/forms/U"]=="replacement"&&files["/db48x/forms/U.bak"]=="older"&&files["/db48x/forms/U.rollback"]=="validated");
    files.clear();pico_io_reset();files["/db48x/forms/T.rollback"]="valid interrupted";
    pico_storage_recovered("forms/T","forms/T.rollback");assert(files["/db48x/forms/T"]=="valid interrupted");
    puts("PASS: SD path isolation, short writes, close failures, state commit and rollback");
}
