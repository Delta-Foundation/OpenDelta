[bits 32]

section .data
    nl db 'W', 0x0A

    ; C functions
extern entry_point
extern kmain

; asm functions
global _start
global readp  ; read port
global writep ; write port
global load_idt

_start:
    cli

    mov dword [0xB8000], 0x0F4B0F4B
    mov dword [0xB8000 + 4], 0x0F4E0F4F

    mov esp, stack_space

    call entry_point
    call kmain 

    mov ebp, esp

    hlt

    jmp $

_exit:
    mov ebx, 0
    mov eax, 1
    int 0x80

readp:
    mov edx, [esp + 4]
    in al, dx
    ret

writep:
    mov edx, [esp + 4]
    mov al, [esp + 4 + 4]
    out dx, al
    ret

load_idt:
    mov edx, [esp + 4]
    lidt [edx]
    sti
    ret

section .bss
    align 4096
    stack_space:
        resb 0x4000
    stack_top:
