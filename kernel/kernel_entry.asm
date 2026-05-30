[bits 32]

    ; asm functions
global _start
global readp  ; read port
global writep ; write port
global load_idt

    ; C functions
extern entry_point
extern kmain

section .text.start

_start:
    cli

    mov dword [0xB8000], 0x0F4B0F4B

    mov esp, stack_top

    call entry_point
    call kmain 

    cli

.hang:
    hlt
    jmp .hang

_exit:
    mov ebx, 0
    mov eax, 1
    int 0x80

readp:
    push ebp

    mov ebp, esp
    mov dx, [ebp + 8]
    in al, dx

    pop ebp
    ret

writep:
    push ebp

    mov ebp, esp
    mov dx, [ebp + 8]
    mov al, [ebp + 12]
    out dx, al

    pop ebp
    ret

load_idt:
    push ebp

    mov ebp, esp
    lidt [ebp + 8]

    sti
    pop ebp
    ret

section .data
    nl db 'W', 0x0A

section .bss
    align 16
    stack_bot:
        resb 0x4000
    stack_top:
