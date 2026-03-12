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
    uint8* rawHeapBase = _kernel_end;
    uint64 heapEnd = 0x04000000; //64MiB if Heap is made bigger in longmode.asm pd must also be set bigger
    uint64 maxOrder = Allocator::Buddy::MAX_ORDER;
    uint64 align = 1ULL << maxOrder;
    uint64 alignedBase = ((uint64)rawHeapBase + align -1)& ~(align -1);
    uint64 alignedEnd = heapEnd & ~(align -1);
    if(alignedBase >= alignedEnd)
        Display::panic("Allocator error. Base bigger as END");
    uint64 heapSize = alignedEnd - alignedBase;
    Allocator::Buddy::init((uint8*)alignedBase, heapSize);
}

extern "C" void KernelStart(){
    Console::ClearScreen();
    initAllocator();
    FileSystem::init();
    Console::init();
    Console::println("Starting Kernel ...");
    Console::println("Allocator Initalised");
    Console::println("FileSystem Initalised");
    Console::println("Console Initalised");
    KernelRun();
}