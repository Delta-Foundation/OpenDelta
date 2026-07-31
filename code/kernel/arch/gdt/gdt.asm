[bits 64]

global gdt_load

gdt_load:
    push rbp
    mov rbp, rsp

    mov rax, [rbp + 8]
    lgdt [rax]

    mov rax, [rbp + 12]
    push rax
    push reload_cs
    retf

reload_cs:
    mov ax, [rbp + 16]
    mov ds, ax 
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rsp, rbp
    pop rbp
    ret
