#pragma once
#include "psram.h"
#include <stdint.h>
#include <stddef.h>
#include <cstdlib>
#include <cstring>
#include <new>
#include <type_traits>
// Fixed slots avoid external allocator fragmentation. Slot ownership is explicit;
// address zero is never returned as a handle. No access aliases the GC arena.
uint32_t pico_bulk_allocate(size_t size);
void pico_bulk_free(uint32_t handle);
size_t pico_bulk_used();
inline uint32_t pico_bulk_checksum(const void *data,size_t n)
{uint32_t h=2166136261u;auto *p=static_cast<const uint8_t *>(data);while(n--)h=(h^*p++)*16777619u;return h;}
template<class T>struct pico_document_cache
{
    static_assert(std::is_trivially_copyable<T>::value,"Serial backing requires pointer-free value documents");
    pico_document_cache()=default;
    pico_document_cache(const pico_document_cache &)=delete;
    T *resident=nullptr;uint32_t handle=0,checksum=0;
    bool get()
    {
        if(resident)return true;resident=new(std::nothrow) T;if(!resident)return false;
        if(handle){if(!pico_psram_read(handle,resident,sizeof(T))||pico_bulk_checksum(resident,sizeof(T))!=checksum){delete resident;resident=nullptr;return false;}}
        return true;
    }
    bool park()
    {
        if(!resident)return true;if(!handle)handle=pico_bulk_allocate(sizeof(T));if(!handle)return false;
        if(!pico_psram_write(handle,resident,sizeof(T)))return false;
        uint8_t check[256];
        for(size_t i=0;i<sizeof(T);i+=sizeof check){size_t n=sizeof(T)-i;if(n>sizeof check)n=sizeof check;if(!pico_psram_read(handle+i,check,n)||memcmp(check,reinterpret_cast<const uint8_t *>(resident)+i,n))return false;}
        checksum=pico_bulk_checksum(resident,sizeof(T));delete resident;resident=nullptr;return true;
    }
    ~pico_document_cache(){delete resident;if(handle)pico_bulk_free(handle);}
};
template<class T>struct pico_document_access
{
    pico_document_cache<T> &cache;T *&pointer;
    pico_document_access(pico_document_cache<T> &c,T *&p):cache(c),pointer(p){if(cache.get())pointer=cache.resident;else pointer=nullptr;}
    pico_document_access(const pico_document_access &)=delete;
    ~pico_document_access(){cache.park();pointer=cache.resident;}
};

// One hot record stays in SRAM. Checked records live in serial PSRAM; machines
// without a working chip use a bounded ordinary heap allocation instead.
template<class T,size_t N>class pico_bulk_records
{
    struct record{T value;uint32_t checksum;};
    uint32_t handle=0;T *fallback=nullptr;T hot{};size_t index=N;bool failed=false;uint16_t initialized=N;
    bool flush()
    {
        if(!handle||index==N)return true;
        record r{hot,pico_bulk_checksum(&hot,sizeof hot)};
        if(!pico_psram_write(handle+index*sizeof(record),&r,sizeof r)){failed=true;return false;}
        return true;
    }
public:
    static_assert(std::is_trivially_copyable<T>::value,"Serial records must be plain values");
    static_assert(N>0&&N<=65535,"Bounded record count must fit initialization metadata");
    explicit pico_bulk_records(bool initialize=true)
    {
        initialized=initialize?N:0;
        handle=pico_bulk_allocate(N*sizeof(record));
        if(handle&&initialize){record r{};r.checksum=pico_bulk_checksum(&r.value,sizeof r.value);for(size_t i=0;i<N;++i)if(!pico_psram_write(handle+i*sizeof r,&r,sizeof r)){pico_bulk_free(handle);handle=0;break;}}
        if(!handle)fallback=new(std::nothrow) T[N]{};
        failed=!handle&&!fallback;
    }
    ~pico_bulk_records(){if(handle)pico_bulk_free(handle);delete[] fallback;}
    bool good()const{return !failed;}
    T &operator[](size_t next)
    {
        if(next>=initialized){failed=true;hot=T{};return hot;}
        if(fallback)return fallback[next];
        if(next!=index)
        {
            if(!flush()){hot=T{};index=N;return hot;}
            record r{};
            if(!handle||!pico_psram_read(handle+next*sizeof r,&r,sizeof r)||pico_bulk_checksum(&r.value,sizeof r.value)!=r.checksum){failed=true;hot=T{};index=N;return hot;}
            hot=r.value;index=next;
        }
        return hot;
    }
    // Append-only clients avoid clearing unused records and read-before-write.
    // Unwritten records are inaccessible, including on the SRAM fallback.
    bool reset_append(){if(failed)return false;index=N;initialized=0;return true;}
    bool put(size_t next,const T &value){
        if(failed||next>=N||next>initialized){failed=true;return false;}
        if(!flush())return false;index=N;
        if(fallback)fallback[next]=value;
        else{record r{value,pico_bulk_checksum(&value,sizeof value)};if(!handle||!pico_psram_write(handle+next*sizeof r,&r,sizeof r)){failed=true;return false;}}
        if(next==initialized)++initialized;return true;
    }
    bool read(size_t next,T &value){
        if(failed||next>=initialized){failed=true;return false;}
        if(fallback){value=fallback[next];return true;}
        if(next==index){value=hot;return true;}
        if(!flush())return false;index=N;record r{};
        if(!handle||!pico_psram_read(handle+next*sizeof r,&r,sizeof r)||pico_bulk_checksum(&r.value,sizeof r.value)!=r.checksum){failed=true;return false;}
        value=r.value;return true;
    }
    void drop_first(){for(size_t i=1;i<N;++i){T value=(*this)[i];(*this)[i-1]=value;}}
    pico_bulk_records(const pico_bulk_records &)=delete;
};
