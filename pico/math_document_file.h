#pragma once
#include "math_documents.h"
#include "resource_file.h"
template<class T>bool pico_math_save(const char *path,T &doc){uint8_t data[T::bytes];size_t n=doc.encode(data);bool ok=n&&pico_resource_save(path,data,n);if(ok)doc.dirty=false;return ok;}
template<class T>bool pico_math_load(const char *path,T &doc,bool &backup){uint8_t data[T::bytes];return pico_resource_load(path,data,sizeof data,backup,[&](size_t n){return doc.decode(data,n);});}
