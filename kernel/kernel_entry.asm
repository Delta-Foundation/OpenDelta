[bits 32]

section .entry
    ; C functions
    extern entry_point
    extern kmain
    
    global _start

_start:
    mov dword [0xB8000], 0x0F4B0F4B
    mov dword [0xB8000 + 4], 0x0F4E0F4F

    mov esp, stack_space
    mov ebp, esp

    call entry_point
    call kmain 

    cli
    hlt

    jmp $ 

section .data
    nl db 'W', 0x0A

section .text
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

_exit:
    mov ebx, 0
    mov eax, 1
    int 0x80

section .bss
    align 4096
    stack_space:
        resb 0x4000
    stack_top:
