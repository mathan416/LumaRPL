#include <cstdlib>
#include <new>
#include <stdint.h>
#include <malloc.h>
#include "scoped_array.h"
#include "lua_runner.h"
#include "plot_engine.h"
#include <cstring>
void print(const char *s){register unsigned r0 asm("r0")=4;register const char *r1 asm("r1")=s;asm volatile("bkpt 0xab" : "+r"(r0):"r"(r1):"memory");}
extern "C" void exit(int result){unsigned p[]={0x20026,unsigned(result)};register unsigned r0 asm("r0")=0x20;register void *r1 asm("r1")=p;asm volatile("bkpt 0xab" : "+r"(r0):"r"(r1):"memory");for(;;);}
void hex(unsigned n){char b[10];const char *h="0123456789ABCDEF";for(int i=0;i<8;++i)b[i]=h[(n>>((7-i)*4))&15];b[8]='\n';b[9]=0;print(b);}
void check(bool ok){static unsigned stage;++stage;if(!ok){char text[]="FAIL: Arm allocation check 0000\n";const char *hex="0123456789ABCDEF";for(int i=0;i<4;++i)text[27+i]=hex[(stage>>((3-i)*4))&15];print(text);exit(1);}}
// Same ordinary delete contract supplied by the SDK in the firmware.
void operator delete(void *p) noexcept {std::free(p);}
void operator delete[](void *p) noexcept {std::free(p);}
void operator delete(void *p,std::size_t) noexcept {std::free(p);}
void operator delete[](void *p,std::size_t) noexcept {std::free(p);}
struct Root {static int count;Root(){++count;}~Root(){--count;}};int Root::count;
extern "C" int pico_lua_cancel(void){return 0;}
extern "C" int pico_lua_eval(const char *,char *out,size_t size){if(size)out[0]=0;return 1;}
extern "C" int pico_native_plot(pico_plot_request *request,pico_plot_sample sample,void *context)
{
    double *values=static_cast<double *>(malloc(7200));void *export_file=malloc(1224);
    if(!values||!export_file){free(values);free(export_file);return 0;}int result=pico_plot_engine::sample_curves(*request,sample,context,values);
    free(values);free(export_file);return result;
}
struct source_reader {const char *text;size_t position;};
const char *read_script(void *opaque,size_t *size)
{
    auto &r=*static_cast<source_reader *>(opaque);size_t left=strlen(r.text)-r.position;*size=left>32?32:left;const char *part=r.text+r.position;r.position+=*size;return *size?part:nullptr;
}
int main()
{
    auto *zero=::operator new(0,std::nothrow);check(zero);::operator delete(zero);
    {scoped_array<Root> roots(10240);check(!!roots&&Root::count==10240);}check(!Root::count);
    {scoped_array<uint32_t> large(10240);check(!!large);large[10239]=42;check(large[10239]==42);}
    {scoped_array<uint32_t> overflow(SIZE_MAX);check(!overflow);}
    // Actual newlib malloc over the same production bounded _sbrk.
    void *documents[5];size_t sizes[]={4676,2064,900,536,216};for(int i=0;i<5;++i){documents[i]=malloc(sizes[i]);check(documents[i]);}
    for(int run=0;run<100;++run)
    {
        void *source=malloc(16384),*undo=malloc(4280);check(source&&undo);free(source);
        void *context=malloc(2464),*lua[48];check(context);for(auto &block:lua){block=malloc(1024);check(block);}
        // Streaming source uses a 512-byte stack buffer, not the editor heap.
        void *plot=malloc(7200),*export_file=malloc(1224);check(plot&&export_file);
        check(!::operator new(98304,std::nothrow));free(export_file);free(plot);for(auto block:lua)free(block);free(context);free(undo);
    }
    size_t lua_peak=0;
    for(int i=0;i<20;++i)
    {
        pico_lua_result result;source_reader reader={"local t={} for i=1,150 do t[i]=i end;plot.function2d(function(x) return math.sin(x) end);plot.show()",0};
        check(pico_lua_run_stream(read_script,&reader,&result));check(result.peak<=49152);if(result.peak>lua_peak)lua_peak=result.peak;
    }
    {pico_lua_result result;source_reader reader={"local t={} for i=1,1200 do t[i]=i end print(#t)",0};check(pico_lua_run_stream(read_script,&reader,&result));check(result.peak>40000&&result.peak<=49152&&!strcmp(result.output,"1200\n"));print("Expanded Arm Lua peak (hex): ");hex(result.peak);}
    void *pressure[128];int pressure_count=0;while(pressure_count<128&&(pressure[pressure_count]=malloc(1024)))++pressure_count;check(pressure_count>0&&pressure_count<128);
    {pico_lua_result result;source_reader reader={"local t={} for i=1,150 do t[i]=i end;plot.function2d(function(x) return x end);plot.show()",0};check(!pico_lua_run_stream(read_script,&reader,&result));check(result.length>0);}
    while(pressure_count)free(pressure[--pressure_count]);
    {pico_lua_result result;source_reader reader={"plot.function2d(function(x) return x end);plot.show()",0};check(pico_lua_run_stream(read_script,&reader,&result));}
    print("Actual Arm Lua peak (hex): ");hex(lua_peak);
    for(auto p:documents)free(p);
    void *blocks[128];int n=0;while(n<128&&(blocks[n]=malloc(1024)))++n;check(n<128);
    auto *fail=new(std::nothrow) char[1024];check(!fail);{scoped_array<Root> denied(1024);check(!denied&&!Root::count);}
    while(n)free(blocks[--n]);
    auto *array=new(std::nothrow) Root[20];check(array&&Root::count==20);delete[] array;check(!Root::count);
    print("PASS: Arm newlib 96KiB heap, zero-size/nothrow/array failure, GC-root lifetime and 100 combined budget cycles and 20 actual streamed Lua/native plots\n");return 0;
}
