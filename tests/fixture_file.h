#pragma once
// Include after test_abort() and the host FatFs shim. Every later fopen() in a
// controller test opens a fixture or report, so a missing file or folder fails
// the check at the caller's line instead of crashing inside stdio.
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string>
#include <sys/stat.h>
inline FILE *fixture_open(const char *path,const char *mode,int line)
{
    if(strchr(mode,'w')||strchr(mode,'a'))
    {
        // Writers create their parent folders; the prepared tree is not assumed.
        std::string parent(path);
        for(size_t at=parent.find('/',1);at!=std::string::npos;at=parent.find('/',at+1))
            mkdir(parent.substr(0,at).c_str(),0700);
    }
    FILE *file=std::fopen(path,mode);
    if(!file)
    {
        fprintf(stderr,"FAIL: cannot open test fixture %s (%s)\n",path,strerror(errno));
        test_abort(line);
    }
    return file;
}
#define fopen(path,mode) fixture_open(path,mode,__LINE__)
