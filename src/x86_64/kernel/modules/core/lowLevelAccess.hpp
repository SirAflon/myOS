#pragma once
#include "dataTypes.hpp"
#include "acpi.hpp"

//
// LowLevelAccess
// A header‑only, inline hardware access layer for x86_64 kernels.
// Provides CPU control, port‑mapped I/O, MSR access, and MMIO helpers.
// All functions are static + inline to avoid needing a .cpp file.
//
namespace LowLevelAccess {

    void Hlt();
    void Cli();
    void Sti();
    void out8(uint16 port, uint8 val);
    void out16(uint16 port, uint16 val);
    void out32(uint16 port, uint32 val);
    uint8 in8(uint16 port);
    uint16 in16(uint16 port);
    uint32 in32(uint16 port);
    uint64 readMsr(uint32 msr);
    void writeMsr(uint32 msr, uint64 val);
    uint8 mmioRead8(uintptr addr);
    uint16 mmioRead16(uintptr addr);
    uint32 mmioRead32(uintptr addr);
    uint64 mmioRead64(uintptr addr);
    void mmioWrite8(uintptr addr, uint8 val);
    void mmioWrite16(uintptr addr, uint16 val);
    void mmioWrite32(uintptr addr, uint32 val);
    void mmioWrite64(uintptr addr, uint64 val);
    void shutdown();
    void tripleFault();
    void kbdReboot();
    void reboot();
}