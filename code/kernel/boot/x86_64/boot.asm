; === SECTOR 1: Boot sector (512 bytes) === ;
[bits 16]
[org 0x7C00]

SECOND_STAGE_ADDR equ 0x7E00
SECOND_STAGE_SECTORS equ 4

PAGE_BASE equ 0x1000       ; page tables at 0x1000 (safe area)

_start:
    cli
    xor ax, ax 
    mov ds, ax 
    mov es, ax 
    mov ss, ax 
    mov sp, 0x7C00

    mov [boot_drive], dl 

    mov ax, 0x0003
    int 0x10

    mov si, str_16_loading
    call print16

    mov ah, 0x02
    mov al, SECOND_STAGE_SECTORS
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [boot_drive]
    mov bx, SECOND_STAGE_ADDR

    int 0x13
    jc disk_error

    jmp second_stage_entry

disk_error:
    mov si, str_disk_error
    call print16 
    jmp $

print16:
    pusha
.loop:
    lodsb
    test al, al 
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp .loop
.done:
    popa 
    ret 

; --- data of boot sector --- ;
boot_drive:     db 0
str_16_loading: db "[DBL 16]: Loading second stage of boot...", 13, 10, 0
str_disk_error: db "[DBL ERROR]: Disk read failed!!!", 13, 10, 0

; --- boot signature --- ;
times 510 - ($ - $$) db 0
dw 0xAA55

; === SECTOR 2+: second stage of boot === ;
second_stage_entry:
    mov si, str_16_bit_start
    call print16 

    ; --- Enable A20 --- ;
    mov si, str_a20
    call print16 

    in al, 0x92
    or al, 2 
    and al, 0xFE
    out 0x92, al 

    ; --- Check CPUID --- ;
    pushfd
    pop eax
    mov ecx, eax 
    xor eax, 1 << 21
    push eax 
    popfd 
    
    pushfd 
    pop eax 
    push ecx 
    popfd 
    cmp eax, ecx 
    je .no_long_mode_16

    ; --- Check long mode support --- ;
    mov eax, 0x80000001
    cpuid 
    test edx, 1 << 29 
    jz .no_long_mode_16

    mov si, str_cpuid_ok
    call print16

    ; --- Setup page tables at PAGE_BASE --- ;
    mov si, str_setup_pages
    call print16

    ; Clear 16KB for page tables
    mov ax, PAGE_BASE >> 4
    mov es, ax 
    xor di, di 
    xor ax, ax 
    mov cx, 0x2000
    rep stosw
    xor ax, ax 
    mov es, ax 

    mov dword [PAGE_BASE], PAGE_BASE + 0x1003
    mov dword [PAGE_BASE + 4], 0

    mov dword [PAGE_BASE + 0x1000], PAGE_BASE + 0x2003
    mov dword [PAGE_BASE + 0x1004], 0

    mov dword [PAGE_BASE + 0x2000], 0x000083
    mov dword [PAGE_BASE + 0x2008], 0x200083
    mov dword [PAGE_BASE + 0x2010], 0x400083
    mov dword [PAGE_BASE + 0x2018], 0x600083

    mov si, str_jump_to_64
    call print16

    ; --- enter long mode directly from real mode --- ;
    cli 

    ; Load GDT
    lgdt [gdt_ptr]

    ; Enable PAE in CR4
    mov eax, cr4 
    or eax, 1 << 5
    mov cr4, eax 

    ; Set CR3 to page table base
    mov eax, PAGE_BASE
    mov cr3, eax 

    ; Enable long mode in EFER MSR
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Enable paging + protected mode
    mov eax, cr0
    or eax, (1 << 31) | (1 << 0)
    mov cr0, eax 

    ; Far jump to 64-bit mode
    jmp 0x08:long_mode_entry 

.no_long_mode_16:
    mov si, str_error_long_mode
    call print16 
    jmp $

; === 64-bit long mode === ;
[bits 64]
long_mode_entry:
    mov ax, 0x10
    mov ds, ax 
    mov es, ax 
    mov ss, ax 
    mov rsp, 0x90000

    mov edi, 0xB8000 + 7 * 160
    mov rsi, str_64_bit_start
    mov ah, 0x0D
    call print64

    mov edi, 0xB8000 + 8 * 160
    mov rsi, str_64_bit_done
    mov ah, 0x0A
    call print64

    jmp $

print64:
    push rdi
    push rsi
.loop:
    lodsb
    test al, al
    jz .done
    stosw
    jmp .loop
.done:
    pop rsi
    pop rdi
    ret

; ===============  DATA  ===============
str_16_bit_start: db "[DBL 16]: Real mode OK", 13, 10, 0
str_a20:          db "[DBL 16]: A20 enabled", 13, 10, 0
str_cpuid_ok:     db "[DBL 16]: CPUID: Long Mode OK", 13, 10, 0
str_setup_pages:     db "[DBL 16]: Page tables ready", 13, 10, 0
str_jump_to_64:      db "[DBL 16]: -> Long mode...", 13, 10, 0
str_error_long_mode: db "[DBL ERROR]: No Long Mode!", 13, 10, 0

str_64_bit_start: db "[DBL 64]: Long mode OK", 0
str_64_bit_done:  db "[DBL 64]: All transitions done!", 0

; ============  GDT 64-bit  ============
align 8
gdt64:
    dq 0                        ; null
    dq 0x00AF9A000000FFFF       ; 64-bit code
    dq 0x00CF92000000FFFF       ; 64-bit data
gdt_ptr:
    dw $ - gdt64 - 1
    dd gdt64

; pad stage2 to full sectors
times ((1 + SECOND_STAGE_SECTORS) * 512) - ($ - $$) db 0
