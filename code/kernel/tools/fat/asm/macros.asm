%macro LINEAR_TO_SEG_OFFSET 4
    mov %3, %1
    shr %3, 4
    push %3
    pop %4
    mov %2, %4

    mov %3, %1
    and %3, 0xF
    push %3
    pop %4
%endmacro

%macro ENTER_LONG_MODE 0
    cli
    mov rax, cr0
    or al, 0x01
    mov cr0, rax
    jmp 0x08:%%lmode_start
%%lmode_start:
    [bits 32]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
%endmacro
