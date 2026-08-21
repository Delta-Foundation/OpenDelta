; === SECTOR 1: Boot sector (512 bytes) === ;
[bits 16]
[org 0x7C00]

global _start
global second_stage_entry

_start:
    cli
    cld 
    jmp 0x0000:flush_segments

flush_segments:
    mov sp, _start 
    xor ax, ax 
    mov ds, ax 
    mov es, ax 
    mov ss, ax 
    mov sp, 0x7C00
    
    mov byte [boot_drive], dl 

    mov ax, 0x0003
    int 0x10

    mov si, str_16_loading
    call print_rmode 
    
    mov ax, 0x0241
    mov bx, 0x7E00 
    mov cx, 0x0002
    xor dh, dh 
    
    int 0x13 
    jc disk_error 

    jmp second_stage_entry

disk_error:
    mov si, str_disk_error
    call print_rmode 
    jmp $

print_rmode:
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

; === data of 16-bit real mode. Boot sector === ;
boot_drive:       db 0
str_16_loading:   db "[DBL 16]: Loading second stage of boot...", 13, 10, 0
str_disk_error:   db "[DBL ERROR]: Disk read error!!!", 13, 10, 0

; --- boot signature --- ;
times 510 - ($ - $$) db 0
dw 0xAA55

; === SECTOR 2+: second stage of boot === ;
[bits 16]
second_stage_entry:
    mov si, str_16_bit_start
    call print_rmode 

    ; --- Enable A20 --- ;
    mov si, str_a20
    call print_rmode

    in al, 0x92
    or al, 2 
    and al, 0xFE
    out 0x92, al 

    mov si, str_jump_to_32
    call print_rmode

    ; --- siwtch to 32-bit Protected Mode --- ;
    cli 
    lgdt [gdt32_ptr]
    mov eax, cr0
    or al, 1
    mov cr0, eax 
    jmp 0x08:pmode_entry

; === 32-bit protected mode === ;
[bits 32]
pmode_entry:
    mov ax, 0x10
    mov ds, ax 
    mov es, ax 
    mov fs, ax 
    mov gs, ax 
    mov ss, ax 
    mov esp, 0x90000

    mov edi, 0xB8000 + 3 * 160
    mov esi, str_32_bit_start
    mov ah, 0x0B
    call print_pmode 

    mov edi, 0xB8000 + 4 * 160
    mov esi, str_cpuid
    mov ah, 0x0B 
    call print_pmode 

    call check_cpuid
    call setup_pages
    call swtich_lmode

check_cpuid:
    pushfd
    pop eax
    mov ecx, eax 
    xor eax, (1 << 21)
    push eax 
    popfd 
    
    pushfd 
    pop eax 
    push ecx 
    popfd 
    cmp eax, ecx 
    jz no_cpuid

    mov eax, 0x80000001
    cpuid 
    test edx, (1 << 29) 
    jz halt32 

no_cpuid:
    mov si, str_no_cpuid
    call print_pmode
    cli 
    hlt 

setup_pages:
    ; --- Setup page tables at PAGE_BASE --- ;
    mov edi, 0xB8000 + 5 * 160
    mov esi, str_pages_ready
    mov ah, 0x0B
    call print_pmode 

    ; Clear 16KB for page tables
    mov edi, PAGE_BASE
    xor eax, eax 
    mov ecx, 4096
    rep stosd

    mov dword [PAGE_BASE], PAGE_BASE + 0x1003
    mov dword [PAGE_BASE + 0x1000], PAGE_BASE + 0x2003

    mov dword [PAGE_BASE + 0x2000], 0x000083
    mov dword [PAGE_BASE + 0x2008], 0x200083
    mov dword [PAGE_BASE + 0x2010], 0x400083
    mov dword [PAGE_BASE + 0x2018], 0x600083

    mov edi, 0xB8000 + 6 * 160
    mov esi, str_jump_to_64
    mov ah, 0x0B
    call print_pmode 

swtich_lmode
    ; --- switch to 64-bit Long Mode --- ;
    mov eax, cr4 
    or eax, (1 << 5)
    mov cr4, eax 

    mov eax, PAGE_BASE
    mov cr3, eax 

    mov ecx, 0xC000008
    rdmsr
    or eax, (1 << 8)
    wrmsr

    mov eax, cr0 
    or eax, (1 << 8)
    mov cr0, eax 

    lgdt [gdt64_ptr]
    jmp 0x08:lmode_entry 

halt32:
    mov edi, 0xB8000 + 5 * 160
    mov esi, str_error 
    mov ah, 0x0C
    call print_pmode 
    jmp $

print_pmode:
    push edi 
    push esi 
.loop:
    lodsb 
    test al, al 
    jz .done 
    stosw 
    jmp .loop 
.done:
    pop esi 
    pop edi 
    ret

; === data of 32-bit protected mode and 16-bit real mode. Stage 2 === ;
PAGE_BASE equ 0x1000

str_16_bit_start: db "[DBL 16]: Real mode   [OK]", 13, 10, 0
str_a20:          db "[DBL 16]: A20 enabled", 13, 10, 0
str_jump_to_32:   db "[DBL 16]: -> Protecterd Mode... ", 13, 10, 0

str_32_bit_start: db "[DBL 32]: Protected mode   [OK]", 0
str_cpuid:        db "[DBL 32]: CPUID: Long Mode [OK]", 0
str_pages_ready:  db "[DBL 32]: Page tables ready", 0
str_jump_to_64:   db "[DBL 32]: -> Long Mode... ", 0

; --- Errors --- ;
str_error:        db "[DBL ERROR]: No Long Mode!", 0
str_no_cpuid:     db "[DBL ERROR]: No CPUID!", 0

; ============  GDT 32-bit  ============
align 8
gdt32:
    dq 0                        ; null
    dq 0x00CF9A000000FFFF       ; 32-bit code
    dq 0x00CF92000000FFFF       ; 32-bit data
gdt32_end:
gdt32_ptr:
    dw gdt32_end - gdt32 - 1
    dd gdt32

; === 64-bit long mode === ;
[bits 64]
lmode_entry:
    mov ax, 0x10
    mov ds, ax 
    mov es, ax 
    mov fs, ax 
    mov gs, ax 
    mov ss, ax 
    mov rsp, 0x90000

    mov edi, 0xB8000 + 7 * 160
    mov rsi, str_64_bit_start
    mov ah, 0x0D
    call print_lmode 

    mov edi, 0xB8000 + 8 * 160
    mov rsi, str_64_bit_done
    mov ah, 0x0A
    call print_lmode 

    cli 

    xor rax, rax 
    xor rbx, rbx
    xor rcx, rcx 
    xor rdx, rdx 
    xor rdi, rdi 

    mov rsp, 0x10000
    mov rbp, rsp 

    call _start 

    cli 
    hlt 

print_lmode:
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

; === data of 64-bit long mode. Stage 2 === ;
str_64_bit_start: db "[DBL 64]: Long mode OK", 0
str_64_bit_done:  db "[DBL 64]: All transitions done!", 0

; ============  GDT 64-bit  ============
align 8
gdt64:
    dq 0                        ; null
    dq 0x00AF9A000000FFFF       ; 64-bit code
    dq 0x00CF92000000FFFF       ; 64-bit data
gdt64_end:
gdt64_ptr:
    dw gdt64_end - gdt64 - 1
    dd gdt64
