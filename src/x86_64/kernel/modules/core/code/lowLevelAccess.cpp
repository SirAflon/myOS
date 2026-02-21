#include "../lowLevelAccess.hpp"
namespace LowLevelAccess {
    
    // ============================================================
    // CPU CONTROL INSTRUCTIONS
    // ============================================================

    // Halts the CPU until the next interrupt arrives.
    // Used in idle loops or when the kernel has nothing to do.
    void Hlt() {
        __asm__ volatile("hlt");
    }

    // Disables maskable interrupts (IF = 0).
    // Use carefully — prevents all hardware interrupts.
    void Cli() {
        __asm__ volatile("cli");
    }

    // Enables maskable interrupts (IF = 1).
    // Typically used after setting up IDT and PIC/APIC.
    void Sti() {
        __asm__ volatile("sti");
    }


    // ============================================================
    // PORT‑MAPPED I/O (I/O BUS)
    // ============================================================

    // Writes an 8‑bit value to an I/O port.
    // Used for devices like PIC, PIT, PS/2 controller, VGA registers.
    void out8(uint16 port, uint8 val) {
        __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
    }

    // Writes a 16‑bit value to an I/O port.
    // Used for some hardware registers that accept 16‑bit writes.
    void out16(uint16 port, uint16 val) {
        __asm__ volatile("outw %0, %1" : : "a"(val), "Nd"(port));
    }

    // Writes a 32‑bit value to an I/O port.
    // Rarely used, but required for some PCI config operations.
    void out32(uint16 port, uint32 val) {
        __asm__ volatile("outl %0, %1" : : "a"(val), "Nd"(port));
    }

    // Reads an 8‑bit value from an I/O port.
    // Example: reading keyboard scancodes from port 0x60.
    uint8 in8(uint16 port) {
        uint8 ret;
        __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
        return ret;
    }

    // Reads a 16‑bit value from an I/O port.
    uint16 in16(uint16 port) {
        uint16 ret;
        __asm__ volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
        return ret;
    }

    // Reads a 32‑bit value from an I/O port.
    uint32 in32(uint16 port) {
        uint32 ret;
        __asm__ volatile("inl %1, %0" : "=a"(ret) : "Nd"(port));
        return ret;
    }

    // Small delay used after certain port writes.
    // Historically required for old hardware; still useful for safety.
    void ioWait() {
        __asm__ volatile("outb %%al, $0x80" : : "a"(0));
    }


    // ============================================================
    // CONTROL REGISTER ACCESS (CR0, CR2, CR3, CR4)
    // ============================================================

    // Reads CR0 — contains paging enable, write‑protect, etc.
    uint64 readCr0() {
        uint64 val;
        __asm__ volatile("mov %%cr0, %0" : "=r"(val));
        return val;
    }

    // Reads CR2 — contains the faulting address on a page fault.
    uint64 readCr2() {
        uint64 val;
        __asm__ volatile("mov %%cr2, %0" : "=r"(val));
        return val;
    }

    // Reads CR3 — contains the physical address of the PML4.
    uint64 readCr3() {
        uint64 val;
        __asm__ volatile("mov %%cr3, %0" : "=r"(val));
        return val;
    }

    // Reads CR4 — contains CPU feature flags (PAE, SSE, etc.).
    uint64 readCr4() {
        uint64 val;
        __asm__ volatile("mov %%cr4, %0" : "=r"(val));
        return val;
    }

    // Writes CR0 — used to enable paging, write‑protect, etc.
    void writeCr0(uint64 val) {
        __asm__ volatile("mov %0, %%cr0" : : "r"(val));
    }

    // Writes CR3 — switches page tables.
    void writeCr3(uint64 val) {
        __asm__ volatile("mov %0, %%cr3" : : "r"(val));
    }

    // Writes CR4 — enables CPU features like PAE, SSE, etc.
    void writeCr4(uint64 val) {
        __asm__ volatile("mov %0, %%cr4" : : "r"(val));
    }


    // ============================================================
    // MODEL‑SPECIFIC REGISTERS (MSRs)
    // ============================================================

    // Reads a 64‑bit MSR (Model Specific Register).
    // Example: EFER (0xC0000080) to enable long mode.
    uint64 readMsr(uint32 msr) {
        uint32 low, high;
        __asm__ volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
        return ((uint64)high << 32) | low;
    }

    // Writes a 64‑bit MSR.
    void writeMsr(uint32 msr, uint64 val) {
        uint32 low  = (uint32)(val & 0xFFFFFFFF);
        uint32 high = (uint32)(val >> 32);
        __asm__ volatile("wrmsr" : : "c"(msr), "a"(low), "d"(high));
    }


    // ============================================================
    // MEMORY‑MAPPED I/O (MMIO)
    // ============================================================

    // Reads an 8‑bit value from a memory‑mapped I/O address.
    // Used for APIC, HPET, PCIe ECAM, etc.
    uint8 mmioRead8(uintptr addr) {
        return *(volatile uint8*)addr;
    }

    // Reads a 16‑bit MMIO value.
    uint16 mmioRead16(uintptr addr) {
        return *(volatile uint16*)addr;
    }

    // Reads a 32‑bit MMIO value.
    uint32 mmioRead32(uintptr addr) {
        return *(volatile uint32*)addr;
    }

    // Reads a 64‑bit MMIO value.
    uint64 mmioRead64(uintptr addr) {
        return *(volatile uint64*)addr;
    }

    // Writes an 8‑bit MMIO value.
    void mmioWrite8(uintptr addr, uint8 val) {
        *(volatile uint8*)addr = val;
    }

    // Writes a 16‑bit MMIO value.
    void mmioWrite16(uintptr addr, uint16 val) {
        *(volatile uint16*)addr = val;
    }

    // Writes a 32‑bit MMIO value.
    void mmioWrite32(uintptr addr, uint32 val) {
        *(volatile uint32*)addr = val;
    }

    // Writes a 64‑bit MMIO value.
    void mmioWrite64(uintptr addr, uint64 val) {
        *(volatile uint64*)addr = val;
    }
}