section .text



bits 16

BOOT_DRIVE db 0
global _start
_start:
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

    ; DL already has boot drive from BIOS
    call load_longmode

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



; DAP for longMode.bin (LBA 1+)
dap_longmode:
    db 0x10
    db 0
    dw 0            ; sector count (big enough for longMode.bin)
    dw 0            ; offset
    dw 0x8000       ; segment (0x8000:0 = 0x80000)
    dq 1            ; LBA 1 (boot.bin is LBA 0, longMode.bin starts at LBA 1)

%ifndef LONGMODE_SECTORS
    %error "LONGMODE_SECTORS not defined (Makefile must pass -DLONGMODE_SECTORS=...)"
%endif

; ----------------------------------------
; load_longmode:
;   Uses LONGMODE_SECTORS (constant from Makefile)
;   Loads longMode.bin starting at 0x8000:0
;   DL = boot drive
; ----------------------------------------
load_longmode:
    mov bx, LONGMODE_SECTORS   ; remaining sectors
    xor si, si                 ; offset from 0x8000:0 in bytes (low 16 bits)
    mov dword [dap_longmode + 8], 1  ; starting LBA = 1 (right after boot sector)

.load_loop:
    cmp bx, 127
    jbe .last_chunk

    ; read 127 sectors
    mov word [dap_longmode + 2], 127      ; count
    mov word [dap_longmode + 4], si       ; offset within 0x8000
    call disk_read

    ; advance LBA
    add dword [dap_longmode + 8], 127

    ; advance offset: si += 127 * 512
    mov ax, 127*512
    add si, ax

    ; remaining -= 127
    sub bx, 127
    jmp .load_loop

.last_chunk:
    ; bx <= 127 here
    mov word [dap_longmode + 2], bx       ; count = remaining
    mov word [dap_longmode + 4], si       ; offset = current offset
    call disk_read
    ret

; ----------------------------------------
; disk_read:
;   Uses INT 13h AH=42h with dap_longmode
;   DL = boot drive
; ----------------------------------------
disk_read:
    pusha
    mov ah, 0x42
    mov si, dap_longmode
    int 0x13
    jc disk_error
    popa
    ret

disk_error:
    ; super simple: hang if disk read fails
    cli
    hlt


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
