#include "../allocator.hpp"

namespace Allocator {
    namespace Bitmap {
        static uint8* bitmap = nullptr;
        static uint64 bitCount = 0;
        void init(uint8* buffer, uint64 bits) {
            bitmap = buffer;
            bitCount = bits;
            for (uint64 i = 0; i < (bits + 7) / 8; i++)
                bitmap[i] = 0;
        }

        bool isFree(uint64 index) {
            uint64 byteIndex = index /8;
            uint8 bit = static_cast<uint8>(index % 8);
            uint8 mask = static_cast<uint8>(1u << bit);
            return (bitmap[byteIndex] & mask) == 0;
        }
        void setUsed(uint64 index) {
            uint64 byteIndex = index /8;
            uint8 bit = static_cast<uint8>(index % 8);
            uint8 mask = static_cast<uint8>(1u << bit);
            bitmap[byteIndex] |= mask;
        }

        void setFree(uint64 index) {
            uint64 byteIndex = index /8;
            uint8 bit = static_cast<uint8>(index %8);
            uint8 mask = static_cast<uint8>(
                ~(static_cast<uint8>(1u) << bit)
            );
            bitmap[byteIndex] &= mask;
        }

        uint64 alloc() {
            for (uint64 i = 0; i < bitCount; i++) {
                if (isFree(i)) {
                    setUsed(i);
                    return i;
                }
            }
            return (uint64)-1;
        }

        void free(uint64 index) {
            setFree(index);
        }
    }
    namespace Buddy {
        constexpr uint64 NUM_ORDERS = MAX_ORDER - MIN_ORDER + 1;

        struct FreeBlock {
            FreeBlock* next;
        };

        static FreeBlock* freeLists[NUM_ORDERS];
        static uint8* heapBase = nullptr;
        static uint64 heapSize = 0;

        void init(uint8* base, uint64 size) {
            heapBase = base;
            heapSize = size;

            for (uint64 i = 0; i < NUM_ORDERS; i++)
                freeLists[i] = nullptr;

            uint64 offset = 0;
            while (offset + (1ULL << MIN_ORDER) <= heapSize) {
                // choose largest block that fits at this offset
                uint64 order = MAX_ORDER;
                while (order > MIN_ORDER && (offset + (1ULL << order) > heapSize))
                    order--;

                uint64 blockSize = 1ULL << order;
                FreeBlock* block = (FreeBlock*)(heapBase + offset);
                block->next = freeLists[order - MIN_ORDER];
                freeLists[order - MIN_ORDER] = block;

                offset += blockSize;
            }
        }


        void* alloc(uint64 size) {
            size += sizeof(uint64);

            uint64 order = MIN_ORDER;
            while ((1ULL << order) < size && order <= MAX_ORDER)
                order++;

            if(order > MAX_ORDER)
                return nullptr;

            uint64 index = order - MIN_ORDER;

            uint64 i = index;
            while (i < NUM_ORDERS && freeLists[i] == nullptr)
                i++;

            if (i == NUM_ORDERS)
                return nullptr;

            FreeBlock* block = freeLists[i];
            freeLists[i] = block->next;

            while (i > index) {
                i--;
                uint64 splitOrder = MIN_ORDER + i;
                uint64 blockSize = 1ULL << splitOrder;
                uint8* blockAddr = (uint8*)block;
                FreeBlock* buddy = (FreeBlock*)(blockAddr + blockSize);
                buddy->next = freeLists[i];
                freeLists[i] = buddy;
                block = (FreeBlock*)blockAddr;
            }

            *((uint64*)block) = order;

            return (uint8*)block + sizeof(uint64);
        }

        void free(void* ptr) {
            if (!ptr  || !heapBase)
                return;

            uint8* blockPtr = (uint8*)ptr - sizeof(uint64);
            uint64 order = *((uint64*)blockPtr);

            if(order < MIN_ORDER || order > MAX_ORDER)
                return;

            uint64 blockSize = 1ULL << order;
            uint64 offset = blockPtr - heapBase;

            if(offset >= heapSize)
                return;

            while(order < MAX_ORDER){
                uint64 buddyOffset = offset ^ blockSize;
                if(buddyOffset >= heapSize)
                    break;
                FreeBlock* buddy = (FreeBlock*)(heapBase + buddyOffset);
                FreeBlock** list = &freeLists[order - MIN_ORDER];
                FreeBlock* prev = nullptr;
                FreeBlock* curr = *list;
                bool found = false;
                while (curr) {
                    if (curr == buddy) {
                        if(prev)
                            prev->next = curr->next;
                        else
                            *list = curr->next;
                        found = true;
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
                if(!found)
                    break;
                if(buddyOffset < offset)
                    offset = buddyOffset;
                blockPtr = heapBase + offset;
                order++;
                blockSize <<=1;
            }
            FreeBlock* block = (FreeBlock*)blockPtr;
            block->next = freeLists[order - MIN_ORDER];
            freeLists[order - MIN_ORDER] = block;
        }
    }
    namespace Slab {
        void initCache(Cache& c, uint64 objSize) {
            c.objectSize = objSize;
            c.freeList = nullptr;
        }

        void* alloc(Cache& c) {
            if (c.freeList) {
                void* obj = c.freeList;
                c.freeList = *(void**)c.freeList;
                return obj;
            }
            return Buddy::alloc(c.objectSize);
        }

        void free(Cache& c, void* ptr) {
            *(void**)ptr = c.freeList;
            c.freeList = ptr;
        }
    }
}
void* operator new(size_t size) {
    return Allocator::Buddy::alloc(size);
}
void* operator new[](size_t size) {
    return Allocator::Buddy::alloc(size);
}
void operator delete(void* ptr) noexcept {
    Allocator::Buddy::free(ptr);
}
void operator delete[](void* ptr) noexcept {
    Allocator::Buddy::free(ptr);
}
void* operator new(size_t, void* ptr) noexcept {
    return ptr;
}
void operator delete(void*, void*) noexcept {

}
void operator delete(void* ptr, size_t) noexcept {
    Allocator::Buddy::free(ptr);
}
void operator delete[](void* ptr, size_t) noexcept {
    Allocator::Buddy::free(ptr);
}
