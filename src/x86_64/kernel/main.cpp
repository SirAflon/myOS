#include "modules/core.hpp"
#include "modules/core/lowLevelAccess.hpp"
extern "C" uint8 _kernel_end[];


void KernelRun(){
    Console::println("Kernel Started");
    Console::println("Entering main loop...");
    while (true) {
        Console::run();
    }
}
void initAllocator(){
    uint8* bitmapBase = _kernel_end;
    uint64 frameCount = 32768;
    Allocator::Bitmap::init(bitmapBase, frameCount);
    uint64 bitmapBytes = (frameCount + 7)/8;
    uint8* rawHeapBase = bitmapBase + bitmapBytes;
    uint64 align = 1ULL << Allocator::Buddy::MAX_ORDER;
    uint64 aligned = ((uint64)rawHeapBase + align -1) & ~(align -1);
    uint8* heapBase = (uint8*)aligned;
    uint64 heapEnd = 0x04000000; //64MiB if Heap is made bigger in longmode.asm pd must also be set bigger
    uint64 heapSize = heapEnd - (uint64)heapBase;
    Allocator::Buddy::init(heapBase, heapSize);
}

extern "C" void KernelStart(){
    //Sector in boot at line 57 as the kernel gets bigger (longmode.bin(size in bytes)+511)/512
    Console::ClearScreen();
    initAllocator();
    fileSystem::init();
    Console::init();
    Console::println("Starting Kernel ...");
    Console::println("Allocator Initalised");
    Console::println("FileSystem Initalised");
    Console::println("Console Initalised");
    KernelRun();
}
