#pragma once
#include "ff.h"
#ifdef __cplusplus
extern "C" {
#endif
FRESULT pico_f_open(FIL *, const char *, BYTE);
FRESULT pico_f_read(FIL *, void *, UINT, UINT *);
FRESULT pico_f_write(FIL *, const void *, UINT, UINT *);
FRESULT pico_f_close(FIL *);
FRESULT pico_f_stat(const char *, FILINFO *);
FRESULT pico_f_unlink(const char *);
FRESULT pico_f_mkdir(const char *);
FRESULT pico_f_opendir(DIR *,const char *);
void pico_io_reset(void);
int pico_io_failed(void);
void pico_storage_verified(const char *);
void pico_storage_recovered(const char *,const char *);
int pico_commit_state(const char *,const char *);
#ifdef __cplusplus
}
#endif
