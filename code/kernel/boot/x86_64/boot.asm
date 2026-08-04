[bits 16]
[org 0x7C00]

PAGE_BASE equ 0x1000       ; page tables at 0x1000 (safe area)

_start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; --- clear screen ---
    mov ax, 0x0003
    int 0x10

    mov si, str_16_bit_start
    call print16

    mov si, str_a20
    call print16

    in  al, 0x92
    or  al, 2
    and al, 0xFE
    out 0x92, al

    mov si, str_jump_to_32
    call print16

    lgdt [gdt32_ptr]
    mov eax, cr0
    or  al, 1
    mov cr0, eax
    jmp 0x08:protected_mode_entry

print16:
    pusha
.lp:
    lodsb
    test al, al
    jz .dn
    mov ah, 0x0E
    int 0x10
    jmp .lp
.dn:
    popa
    ret

; =============  32-bit Protected Mode  =============
[bits 32]
protected_mode_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x90000

    mov edi, 0xB8000 + 3*160
    mov esi, str_32_bit_start
    mov ah, 0x0B            ; light cyan
    call print32

    mov edi, 0xB8000 + 4*160
    mov esi, str_cpuid
    mov ah, 0x0B
    call print32

    ; --- check CPUID ---
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
    je halt32

    ; --- check long mode ---
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz halt32

    mov edi, 0xB8000 + 5*160
    mov esi, str_pages_ready
    mov ah, 0x0B
    call print32

    ; --- setup page tables at PAGE_BASE ---
    mov edi, PAGE_BASE
    xor eax, eax
    mov ecx, 4096
    rep stosd

    ; PML4[0] -> PDPT
    mov dword [PAGE_BASE],       PAGE_BASE + 0x1003
    ; PDPT[0] -> PD
    mov dword [PAGE_BASE+0x1000], PAGE_BASE + 0x2003
    ; PD[0..3] -> 2MB pages identity mapped
    mov dword [PAGE_BASE+0x2000], 0x000083
    mov dword [PAGE_BASE+0x2008], 0x200083
    mov dword [PAGE_BASE+0x2010], 0x400083
    mov dword [PAGE_BASE+0x2018], 0x600083

    mov edi, 0xB8000 + 6*160
    mov esi, str_jump_to_64
    mov ah, 0x0B
    call print32

    mov eax, cr4
    or  eax, 1 << 5
    mov cr4, eax

    mov eax, PAGE_BASE
    mov cr3, eax

    mov ecx, 0xC0000080
    rdmsr
    or  eax, 1 << 8
    wrmsr

    mov eax, cr0
    or  eax, 1 << 31
    mov cr0, eax

    lgdt [gdt64_ptr]
    jmp 0x08:long_mode_entry 

halt32:
    mov edi, 0xB8000 + 5 * 160
    mov esi, str_err 
    mov ah, 0x0C
    call print32
    jmp $

print32:
    push edi
    push esi
.lp:
    lodsb
    test al, al
    jz .dn
    stosw
    jmp .lp
.dn:
    pop esi
    pop edi
    ret

; =============  64-bit Long Mode  =============
[bits 64]
long_mode_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov rsp, 0x90000

    mov edi, 0xB8000 + 7 * 160
    mov rsi, str_64_bit_start
    mov ah, 0x0D            ; light magenta
    call print64

    mov edi, 0xB8000 + 8 * 160
    mov rsi, str_64_bit_done
    mov ah, 0x0A            ; light green
    call print64

    cli
    hlt
    jmp $-2

print64:
    push rdi
    push rsi
.lp:
    lodsb
    test al, al
    jz .dn
    stosw
    jmp .lp
.dn:
    pop rsi
    pop rdi
    ret

; ===============  DATA  ===============
str_16_bit_start: db "[DBL 16]: Real mode OK", 13, 10, 0
str_a20:          db "[DBL 16]: A20 enabled", 13, 10, 0
str_jump_to_32:   db "[DBL 16]: -> Protected mode...", 13, 10, 0

str_32_bit_start: db "[DBL 32]: Protected mode OK", 0
str_cpuid:        db "[DBL 32]: CPUID: Long Mode OK", 0
str_pages_ready:  db "[DBL 32]: Page tables ready", 0
str_jump_to_64:   db "[DBL 32]: -> Long mode...", 0
str_err:          db "[ERR!!]: No Long Mode!", 0

str_64_bit_start: db "[DBL 64]: Long mode OK", 0
str_64_bit_done:  db "[DBL 64]: All transitions done!", 0

; ============  GDT 32-bit  ============
align 8
gdt32:
    dq 0                        ; null
    dq 0x00CF9A000000FFFF       ; 32-bit code
    dq 0x00CF92000000FFFF       ; 32-bit data
gdt32_ptr:
    dw $ - gdt32 - 1
    dd gdt32

; ============  GDT 64-bit  ============
align 8
gdt64:
    dq 0                        ; null
    dq 0x00AF9A000000FFFF       ; 64-bit code
    dq 0x00CF92000000FFFF       ; 64-bit data
gdt64_ptr:
    dw $ - gdt64 - 1
    dd gdt64

; ============  Boot signature  ============
times 510 - ($ - $$) db 0
dw 0xAA55
