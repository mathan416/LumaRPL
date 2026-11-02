extern "C" {
#define main fatfs_baseline_main
#include "fatfs_test.c"
#undef main
}
#include "currency_preferences.h"
int main()
{
    make_disk();FATFS fs;assert(f_mount(&fs,"",1)==FR_OK&&f_mkdir("/db48x")==FR_OK&&pico_f_mkdir("currency")==FR_OK);
    pico_currency_preferences first,next,loaded;strcpy(first.from,"CAD");strcpy(first.to,"USD");next=first;strcpy(next.from,"USD");strcpy(next.to,"CAD");
    uint8_t bytes[first.bytes];assert(first.encode(bytes));
    for(size_t n=0;n<first.bytes;++n){loaded=next;assert(!loaded.decode(bytes,n)&&!memcmp(&loaded,&next,sizeof next));}
    for(size_t i=0;i<first.bytes;++i){bytes[i]^=1;loaded=next;assert(!loaded.decode(bytes,sizeof bytes)&&!memcmp(&loaded,&next,sizeof next));bytes[i]^=1;}
    assert(first.save()&&next.save());assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_OK);assert(loaded.load()&&!memcmp(&loaded,&next,sizeof next));
    fail_writes=1;assert(!first.save());fail_writes=0;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_OK);assert(loaded.load()&&!memcmp(&loaded,&next,sizeof next));
    put(first.path,"corrupt");assert(loaded.load()&&!memcmp(&loaded,&first,sizeof first));
    assert(next.save());missing=1;assert(f_mount(NULL,"",0)==FR_OK&&f_mount(&fs,"",1)==FR_NOT_READY);
    loaded=first;assert(!next.save()&&!loaded.load()&&!memcmp(&loaded,&first,sizeof first));free(disk);
    puts("PASS: currency pair actual FatFS restart, checksummed validation, backup recovery, failed replacement and missing SD");
}
