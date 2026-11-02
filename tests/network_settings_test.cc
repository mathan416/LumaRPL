extern "C" {
#define main fatfs_baseline_main
#include "fatfs_test.c"
#undef main
}
#include "network_settings.h"
int main()
{
    make_disk();FATFS fs;assert(f_mount(&fs,"",1)==FR_OK&&f_mkdir("/db48x")==FR_OK&&pico_f_mkdir("network")==FR_OK);
    pico_network_settings first,second,loaded;strcpy(first.ssid,"Unit test SSID");strcpy(first.password,"test-password");assert(first.valid());second=first;strcpy(second.ssid,"Second SSID");assert(first.save()&&second.save()&&loaded.load()&&!strcmp(loaded.ssid,"Second SSID"));
    memset(second.password,'x',63);second.password[63]=0;assert(second.save()&&loaded.load()&&strlen(loaded.password)==63);assert(first.save()&&first.save());
    put("network/WIFI.cfg","corrupt");assert(loaded.load()&&!strcmp(loaded.ssid,first.ssid));fail_writes=1;assert(!second.save());fail_writes=0;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_OK);assert(loaded.load()&&!strcmp(loaded.ssid,first.ssid));
    loaded.clear();assert(!*loaded.ssid&&!*loaded.password&&!loaded.valid());strcpy(loaded.ssid,"Open network");assert(!loaded.valid());loaded.open=true;assert(loaded.valid());strcpy(loaded.password,"short");assert(!loaded.valid());missing=1;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_NOT_READY);assert(!first.save());free(disk);puts("PASS: Wi-Fi credential validation, actual FatFS persistence/recovery/failures and clearing");
}
