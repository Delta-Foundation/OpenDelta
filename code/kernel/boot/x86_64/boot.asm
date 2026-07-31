[bits 16]
[org 0x7C00]

global _start

_start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov ax, 0x0003
    int 0x10

    mov [boot_drive], dl

    mov si, str_real
    call print

    call load_kernel
    call setup_pages
    
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
    je error_loop

    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz error_loop
    
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    
    mov eax, 0x1000
    mov cr3, eax
    
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
    
    mov eax, cr0
    or eax, 1 << 31 | 1
    mov cr0, eax
    
    lgdt [gdt_descriptor]
    jmp 0x08:long_mode_start

print:
    pusha
.print_loop:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp .print_loop
.done:
    popa
    ret

load_kernel:
    mov ax, 0x1000
    mov es, ax
    xor bx, bx
    mov dl, [boot_drive]
    
    mov ah, 0x02
    mov al, 17
    mov ch, 0
    mov dh, 0
    mov cl, 2
    int 0x13
    jc error_loop
    
    mov ah, 0x02
    mov al, 15
    mov ch, 0
    mov dh, 1
    mov cl, 1
    int 0x13
    jc error_loop
    ret

setup_pages:
    ; PML4[0] -> PDPT
    mov dword [0x1000], 0x2003
    mov dword [0x1004], 0
    ; PDPT[0] -> PD
    mov dword [0x2000], 0x3003
    mov dword [0x2004], 0
    
    mov edi, 0x3000
    mov dword [edi], 0x83           ; 0x00000000
    mov dword [edi+4], 0
    mov dword [edi+8], 0x00200083   ; 0x00200000
    mov dword [edi+12], 0
    mov dword [edi+16], 0x00400083  ; 0x00400000
    mov dword [edi+20], 0
    mov dword [edi+24], 0x00600083  ; 0x00600000
    mov dword [edi+28], 0
    ret

error_loop:
    mov si, str_error
    call print
    jmp $

[bits 64]
long_mode_start:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov rsp, 0x90000

    mov rdi, str_long_mode
    call print_long_mode

    mov rax, 0x10000
    jmp rax

print_long_mode:
    push rdi
    push rax
    mov rdx, 0xB8000
.loop:
    mov al, [rdi]
    test al, al
    jz .end
    mov ah, 0x0F
    mov [rdx], ax
    inc rdi
    add rdx, 2
    jmp .loop
.end:
    pop rax
    pop rdi
    ret

gdt_start:
    dq 0x0000000000000000 ; Null Segment
    dq 0x00AF9A000000FFFF ; 64-bit Code Segment (L=1)
    dq 0x00CF92000000FFFF ; 64-bit Data Segment
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

str_real:  db "[DBL]: 16-bit real mode started", 13, 10, 0
str_long_mode: db "[DBL]: 64-bit long mode. Jumping...", 13, 10, 0
str_error: db "[err]: Fail!", 13, 10, 0

boot_drive: db 0

times 510 - ($ - $$) db 0
dw 0xAA55
