#include "storage.h"
#include <stdio.h>
#include <string.h>
static int failed;

// Both calculator absolute paths and relative paths are rooted in /db48x.
static const char *rooted(const char *path,char *out,size_t n)
{
    if (!path || !*path || strchr(path,':') || strchr(path,'\\')) return NULL;
    const char *p=path;
    while(*p=='/') ++p;
    if(!strncmp(p,"db48x/",6)) p+=6;
    for(const char *q=p; *q;)
    {
        const char *end=strchr(q,'/');
        size_t len=end?(size_t)(end-q):strlen(q);
        if(len==2 && q[0]=='.' && q[1]=='.') return NULL;
        if(!end) break;
        q=end+1;
    }
    int count=snprintf(out,n,"/db48x/%s",p);
    return count>=0 && (size_t)count<n ? out:NULL;
}
void pico_io_reset(void) { failed=0; }
int pico_io_failed(void) { return failed; }
FRESULT pico_f_open(FIL *f,const char *p,BYTE mode)
{
    char path[512]; const char *s=rooted(p,path,sizeof path);
    FRESULT r=s?f_open(f,s,mode):FR_INVALID_NAME;
    if(r!=FR_OK) { memset(f,0,sizeof *f); if(mode&FA_WRITE) failed=1; }
    return r;
}
FRESULT pico_f_read(FIL *f,void *p,UINT n,UINT *read)
{
    FRESULT r=f_read(f,p,n,read);
    if(r!=FR_OK) failed=1;
    return r;
}
FRESULT pico_f_write(FIL *f,const void *p,UINT n,UINT *written)
{
    FRESULT r=f_write(f,p,n,written);
    if(r!=FR_OK || *written!=n) failed=1;
    return r;
}
FRESULT pico_f_close(FIL *f)
{
    FRESULT r=f_close(f);
    if(r!=FR_OK) failed=1;
    return r;
}
FRESULT pico_f_stat(const char *p,FILINFO *info)
{ char path[512];const char *s=rooted(p,path,sizeof path);return s?f_stat(s,info):FR_INVALID_NAME; }
FRESULT pico_f_unlink(const char *p)
{ char path[512];const char *s=rooted(p,path,sizeof path);return s?f_unlink(s):FR_INVALID_NAME; }
FRESULT pico_f_mkdir(const char *p)
{ char path[512];const char *s=rooted(p,path,sizeof path);return s?f_mkdir(s):FR_INVALID_NAME; }
FRESULT pico_f_opendir(DIR *d,const char *p)
{ char path[512];const char *s=rooted(p,path,sizeof path);return s?f_opendir(d,s):FR_INVALID_NAME; }
// Bounded validation cache. A miss conservatively preserves an existing .bak.
// Hashing names keeps mixed state/document workflows from consuming path RAM.
typedef struct {uint64_t path,contents;} verified_file;
static verified_file verified[16];
static unsigned verified_count,verified_next;
static uint64_t path_key(const char *p)
{
    uint64_t hash=UINT64_C(14695981039346656037);
    while(*p)hash=(hash^(uint8_t)*p++)*UINT64_C(1099511628211);
    return hash;
}
static int verified_index(uint64_t key)
{for(unsigned i=0;i<verified_count;++i)if(verified[i].path==key)return (int)i;return -1;}
static void forget_verified(uint64_t key)
{int i=verified_index(key);if(i>=0)verified[i]=verified[--verified_count];}
static int fingerprint(const char *path,uint64_t *hash)
{
    FIL file={0};uint8_t bytes[256];UINT n;uint64_t value=UINT64_C(14695981039346656037);
    if(f_open(&file,path,FA_READ)!=FR_OK)return 0;
    int ok=1;
    do {
        if(f_read(&file,bytes,sizeof bytes,&n)!=FR_OK){ok=0;break;}
        for(UINT i=0;i<n;++i)value=(value^bytes[i])*UINT64_C(1099511628211);
    } while(n);
    if(f_close(&file)!=FR_OK)ok=0;
    if(ok)*hash=value;return ok;
}
void pico_storage_verified(const char *path)
{
    char p[512];uint64_t hash;
    if(!rooted(path,p,sizeof p))return;uint64_t key=path_key(p);
    if(!fingerprint(p,&hash)){forget_verified(key);return;}
    int index=verified_index(key);
    if(index<0)index=verified_count<16?(int)verified_count++:(int)(verified_next++%16);
    verified[index]=(verified_file){key,hash};
}
void pico_storage_recovered(const char *path,const char *recovery)
{
    char p[512],r[512],rollback[522];
    if(!rooted(path,p,sizeof p)||!rooted(recovery,r,sizeof r))return;
    forget_verified(path_key(p));
    // Called only after the complete recovery file has passed validation.
    snprintf(rollback,sizeof rollback,"%s.rollback",p);
    if(!strcmp(r,rollback))
    {
        FRESULT removed=f_unlink(p);
        if(removed!=FR_OK&&removed!=FR_NO_FILE)return;
        if(f_rename(r,p)==FR_OK)pico_storage_verified(path);
    }
    else f_unlink(rollback); // Rejected rollback; keep the valid .bak untouched.
}
int pico_commit_state(const char *temp,const char *dest)
{
    char t[512],d[512],backup[522],rollback[522];uint64_t hash;
    if(failed||!rooted(temp,t,sizeof t)||!rooted(dest,d,sizeof d))return 0;
    snprintf(backup,sizeof backup,"%s.bak",d);snprintf(rollback,sizeof rollback,"%s.rollback",d);
    int index=verified_index(path_key(d));
    int trusted=index>=0&&fingerprint(d,&hash)&&hash==verified[index].contents;
    FILINFO info;
    FRESULT pending=f_stat(rollback,&info);
    if(pending==FR_OK)
    {
        // Never discard an unresolved rollback unless the current primary is
        // still the validated generation. Loading repairs interrupted commits.
        if(!trusted||f_unlink(rollback)!=FR_OK)return 0;
    }
    else if(pending!=FR_NO_FILE)return 0;
    FRESULT exists=f_stat(d,&info);int moved=exists==FR_OK;
    if(exists!=FR_OK&&exists!=FR_NO_FILE)return 0;
    if(moved&&f_rename(d,rollback)!=FR_OK)return 0;
    if(f_rename(t,d)!=FR_OK)
    {if(moved)f_rename(rollback,d);return 0;}
    // New primary is installed. Cleanup cannot invalidate that successful save.
    if(moved)
    {
        FRESULT old=f_stat(backup,&info);
        if(trusted||old==FR_NO_FILE)
        {
            FRESULT removed=f_unlink(backup);
            if(removed==FR_OK||removed==FR_NO_FILE)f_rename(rollback,backup);
        }
        else if(old==FR_OK)f_unlink(rollback);
    }
    pico_storage_verified(dest);return 1;
}
