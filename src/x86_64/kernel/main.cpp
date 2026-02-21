#include "modules/core.hpp"
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
    uint8* heapBase = bitmapBase + bitmapBytes;
    uint64 heapEnd = 0x01000000;
    uint64 heapSize = heapEnd - (uint64)heapBase;
    Allocator::Buddy::init(heapBase, heapSize);
}

extern "C" void KernelStart(){
    //Sector in boot at line 57 as the kernel gets bigger (longmode.bin(size in bytes)+511)/512
    Console::ClearScreen();
    initAllocator();
    Console::init();
    Console::println("Starting Kernel ...");
    Console::println("Allocator Initalised");
    KernelRun();
}
