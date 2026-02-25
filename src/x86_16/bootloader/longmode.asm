; longmode.asm
; assembled as: nasm -f elf64
; entry: long_mode_start (32-bit protected mode)

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

    mov al, 0x00        ; unmask all IRQs for now (you can change this)
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

    mov rsp, 0x90000
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
    dq 0x00000000 + (1 << 7) + 3      ; 0–2 MiB
    dq 0x00200000 + (1 << 7) + 3      ; 2–4 MiB
    dq 0x00400000 + (1 << 7) + 3      ; 4–6 MiB
    dq 0x00600000 + (1 << 7) + 3      ; 6–8 MiB
    dq 0x00800000 + (1 << 7) + 3      ; 8–10 MiB
    dq 0x00A00000 + (1 << 7) + 3      ; 10–12 MiB
    dq 0x00C00000 + (1 << 7) + 3      ; 12–14 MiB
    dq 0x00E00000 + (1 << 7) + 3      ; 14–16 MiB
    dq 0x01000000 + (1 << 7) + 3      ; 16–18 MiB
    dq 0x01200000 + (1 << 7) + 3      ; 18–20 MiB
    dq 0x01400000 + (1 << 7) + 3      ; 20–22 MiB
    dq 0x01600000 + (1 << 7) + 3      ; 22–24 MiB
    dq 0x01800000 + (1 << 7) + 3      ; 24–26 MiB
    dq 0x01A00000 + (1 << 7) + 3      ; 26–28 MiB
    dq 0x01C00000 + (1 << 7) + 3      ; 28–30 MiB
    dq 0x01E00000 + (1 << 7) + 3      ; 30–32 MiB
    dq 0x02000000 + (1 << 7) + 3      ; 32–34 MiB
    dq 0x02200000 + (1 << 7) + 3      ; 34–36 MiB
    dq 0x02400000 + (1 << 7) + 3      ; 36–38 MiB
    dq 0x02600000 + (1 << 7) + 3      ; 38–40 MiB
    dq 0x02800000 + (1 << 7) + 3      ; 40–42 MiB
    dq 0x02A00000 + (1 << 7) + 3      ; 42–44 MiB
    dq 0x02C00000 + (1 << 7) + 3      ; 44–46 MiB
    dq 0x02E00000 + (1 << 7) + 3      ; 46–48 MiB
    dq 0x03000000 + (1 << 7) + 3      ; 48–50 MiB
    dq 0x03200000 + (1 << 7) + 3      ; 50–52 MiB
    dq 0x03400000 + (1 << 7) + 3      ; 52–54 MiB
    dq 0x03600000 + (1 << 7) + 3      ; 54–56 MiB
    dq 0x03800000 + (1 << 7) + 3      ; 56–58 MiB
    dq 0x03A00000 + (1 << 7) + 3      ; 58–60 MiB
    dq 0x03C00000 + (1 << 7) + 3      ; 60–62 MiB
    dq 0x03E00000 + (1 << 7) + 3      ; 62–64 MiB

section .note.GNU-stack noalloc noexec nowrite progbits
