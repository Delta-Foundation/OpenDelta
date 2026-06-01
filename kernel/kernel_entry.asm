[bits 32]

    ; asm functions
global _start
global readp  ; read port
global writep ; write port
global load_idt

    ; C functions
extern kmain

section .data
    nl db 'W', 0x0A

section .bss
    align 32
    stack_bot: resb 0x4000
    stack_top:

section .text

_start:
    cli
    cld

    mov dword [0xB8000], 0x0F4B0F4B

    mov esp, stack_top
    push ebx 
    push eax 
    call kmain 

    cli

.hang:
    hlt
    jmp .hang

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
    mov eax, [ebp + 8]
    lidt [eax]

    sti
    pop ebp
    ret
