[bits 32]

section .multiboot
    align 4
    dd 0x1BADB002
    dd 0x00
    dd - (0x1BADB002 - 0x00)
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0

section .data
    nl db 'W', 0x0A

section .text 
    ; C functions
    extern entry_point
    extern kmain
    
    ; ASM functions
    global _start
    global readp  ; read port 
    global writep ; write port
    global load_idt

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

_start:
    mov dword [0xB8000], 0x0F4B0F4B

    mov ebp, 0x90000
    mov esp, 0x90000

    mov esp, stack_space
    call entry_point

    mov ecx, nl
    mov edx, 0x41
    cmp edx, ecx
    je _exit

    call kmain 
    cli
    hlt

    jmp $ 

_exit:
    mov ebx, 0
    mov eax, 1
    int 0x80

section .bss
stack_space:
    resb 8129
stack_top
