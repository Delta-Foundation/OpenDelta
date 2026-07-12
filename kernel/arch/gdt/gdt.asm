[bits 32]

global i386_GDT_load

i386_GDT_load:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    lgdt [eax]

    mov eax, [ebp + 12]
    push eax
    mov eax, reload_cs
    push eax 
    retf

reload_cs:
    mov ax, [ebp + 16]
    mov ds, ax 
    mov es, ax
    mov fs, ax
    mov gs, ax 
    mov ss, ax

    mov esp, ebp
    pop ebp
    ret
