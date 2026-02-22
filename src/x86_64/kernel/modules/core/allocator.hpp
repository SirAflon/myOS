#pragma once
#include <stdint.h>
#include <cstddef>
#include "dataTypes.hpp"

namespace Allocator {
    namespace Bitmap {
        void init(uint8* buffer, uint64 bits);
        uint64  alloc();
        void free(uint64 index);
        bool isFree(uint64 index);
        void setUsed(uint64 index);
        void setFree(uint64 index);
    }
    namespace Buddy {
        constexpr uint64 MIN_ORDER = 4;   // 16 bytes
        constexpr uint64 MAX_ORDER = 20;  // 1 MB
        void init(uint8* base, uint64 size);
        void* alloc(uint64 size);
        void free(void* ptr);
    }
    namespace Slab {
        struct Cache {
            uint64 objectSize;
            void* freeList;
        };
        void initCache(Cache& c, uint64 objSize);
        void* alloc(Cache& c);
        void free(Cache& c, void* ptr);
    }
}
void* operator new(size_t size);
void* operator new(size_t, void* ptr) noexcept;
void* operator new[](size_t size);
void  operator delete(void* ptr) noexcept;
void  operator delete[](void* ptr) noexcept;
void  operator delete(void*, void*) noexcept;
void operator delete(void* ptr,size_t)noexcept;
void operator delete[](void* ptr,size_t)noexcept;