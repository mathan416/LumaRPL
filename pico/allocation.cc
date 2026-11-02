// Match the SDK's no-exception allocator without pulling exception-enabled
// libstdc++ nothrow wrappers, terminate handlers or the demangler into flash.
#include <new>
#include <cstdlib>
void *operator new(std::size_t n,const std::nothrow_t &) noexcept {return std::malloc(n?n:1);}
void *operator new[](std::size_t n,const std::nothrow_t &) noexcept {return std::malloc(n?n:1);}
void operator delete(void *p,const std::nothrow_t &) noexcept {std::free(p);}
void operator delete[](void *p,const std::nothrow_t &) noexcept {std::free(p);}
