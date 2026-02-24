section .text
global _start
_start:

bits 16

BOOT_DRIVE db 0

start:
    cli
    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x7C00

    mov si, msg
    call print_string

    ; save BIOS drive number
    mov [BOOT_DRIVE], dl

    ; -----------------------------
    ; Load longMode.bin to 0x8000:0 (phys 0x80000)
    ; -----------------------------
    mov dl, [BOOT_DRIVE]
    mov si, dap_longmode
    call disk_read

    ; -----------------------------
    ; Enter protected mode
    ; -----------------------------
    cli
    lgdt [gdt_ptr]

    mov eax, cr0
    or eax, 1               ; PE = 1
    mov cr0, eax

    jmp 0x08:pm_entry


; --------------------------------
; BIOS DISK READER (LBA)
; --------------------------------
disk_read:
    pusha
    mov ah, 0x42
    int 0x13
    popa
    ret


; DAP for longMode.bin (LBA 1+)
dap_longmode:
    db 0x10
    db 0
    dw 110           ; sector count (big enough for longMode.bin)
    dw 0x0000       ; offset
    dw 0x8000       ; segment (0x8000:0 = 0x80000)
    dq 1            ; LBA 1 (boot.bin is LBA 0, longMode.bin starts at LBA 1)


msg: db "Booting Raw x86_64 OS", 0

print_string:
    mov ah, 0x0E
.next:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .next
.done:
    ret


; --------------------------------
; GDT and protected mode entry
; --------------------------------
gdt:
    dq 0
    dq 0x00CF9A000000FFFF   ; 0x08: 32-bit code
    dq 0x00CF92000000FFFF   ; 0x10: 32-bit data

gdt_ptr:
    dw gdt_end - gdt - 1
    dd gdt
gdt_end:


bits 32
pm_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    ; jump to long_mode_start at 0x80000
    mov eax, 0x80000
    jmp eax

hang:
    hlt
    jmp hang
