; longmode.asm
; assembled as: nasm -f elf64
; entry: long_mode_start (32-bit protected mode)
section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

section .text
bits 32
global long_mode_start
extern KernelStart

long_mode_start:
    ; we are in 32-bit protected mode
    call setup_hw32

    ; Enable PAE
    mov eax, cr4
    or eax, 1 << 5          ; PAE
    mov cr4, eax

    ; Load PML4 physical address
    mov eax, pml4
    mov cr3, eax

    ; Enable long mode via EFER
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8          ; LME
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, 1 << 31         ; PG
    mov cr0, eax
    
    ; load 64-bit GDT and jump to long mode
    lgdt [gdt64_descriptor]
    jmp 0x08:long64


; ----------------------------------------
; 32-bit hardware + CPU setup before long mode
; ----------------------------------------
setup_hw32:
    ; -------------------------
    ; Remap PIC (8259) to 0x20–0x2F
    ; -------------------------
    mov al, 0x11
    out 0x20, al
    out 0xA0, al

    mov al, 0x20        ; master PIC vector offset
    out 0x21, al
    mov al, 0x28        ; slave PIC vector offset
    out 0xA1, al

    mov al, 0x04        ; tell master about slave at IRQ2
    out 0x21, al
    mov al, 0x02        ; tell slave its cascade identity
    out 0xA1, al

    mov al, 0x01        ; 8086/88 mode
    out 0x21, al
    out 0xA1, al

    mov al, 0x00        ; unmask all IRQs for now
    out 0x21, al
    out 0xA1, al

    ; -------------------------
    ; PIT: set channel 0 to mode 3, ~1000 Hz
    ; -------------------------
    mov al, 0x36        ; channel 0, lobyte/hibyte, mode 3, binary
    out 0x43, al

    mov ax, 1193        ; 1193182 / 1193 ≈ 1000 Hz
    out 0x40, al        ; low byte
    mov al, ah
    out 0x40, al        ; high byte

    ; -------------------------
    ; Enable SSE (optional but recommended)
    ; -------------------------
    mov eax, cr0
    and eax, ~(1 << 2)  ; clear EM (no FPU emulation)
    or  eax,  (1 << 1)  ; set MP (monitor coprocessor)
    mov cr0, eax

    mov eax, cr4
    or  eax, (1 << 9)   ; OSFXSR
    or  eax, (1 << 10)  ; OSXMMEXCPT
    mov cr4, eax

    ret

bits 64
long64:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov rsp, stack_top
    and rsp, -16

    mov rax, KernelStart
    jmp rax

.hang:
    hlt
    jmp .hang


align 8
gdt64:
    dq 0
    dq 0x00AF9A000000FFFF   ; 0x08: 64-bit code
    dq 0x00AF92000000FFFF   ; 0x10: 64-bit data

gdt64_descriptor:
    dw gdt64_descriptor_end - gdt64 - 1
    dq gdt64
gdt64_descriptor_end:


align 4096
pml4:
    dq pdpt + 3

align 4096
pdpt:
    dq pd + 3

align 4096
pd:
    %assign i 0
    %rep 512
        dq (i * 0x200000) + 0x83  ; (i * 2MiB) + Present + Writable + HugePage
        %assign i i+1
    %endrep

section .note.GNU-stack noalloc noexec nowrite progbits