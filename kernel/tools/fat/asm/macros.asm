%macro LINEAR_TO_SEG_OFFSET 4
    mov %3, %1
    push %3
    pop %4
    shr %3, 4
    push %3
    pop %2
%endmacro

%macro ENTER_PROTECTED_MODE 0
    [bits 16]
    cli
    mov eax, cr0 
    or al, 1
    mov cr0, eax
    jmp 0x08:%%pmode_start
%%pmode_start:
    [bits 32]
    mov ax, 10
    mov ds, ax 
    mov es, ax
    mov fs, ax 
    mov gs, ax  
    mov ss, ax
%endmacro
