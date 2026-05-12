[bits 16]
switch:
    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp CODE_SEG:init

[bits 32]
init:
;    mov ax, DATA_SEG
;    mov ds, ax
;    mov es, ax
;    mov ss, ax
;    mov fs, ax
;    mov gs, ax
    
;    mov ebp, 0x80000
;    mov esp, ebp

    jmp protected_mode
