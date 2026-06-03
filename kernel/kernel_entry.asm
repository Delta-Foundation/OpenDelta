[bits 32]

    ; asm functions
global _start
global readp  ; read port
global writep ; write port
global load_idt
[extern __bss_start]
[extern __bss_end]

    ; C functions
[extern kmain]

section .data
    nl db 'W', 0x0A

section .bss
    align 16
    stack_bot: resb 0x4000
    stack_top:

section .text

_start:
    cli
    cld

    mov edi, __bss_start
    mov ecx, __bss_end 
    sub ecx, edi 
    shr ecx, 2
    xor eax, eax 
    rep stosd

    mov dword [0xB8000], 0x0F4B0F4B

    mov ax, 0x10 
    mov ds, ax 
    mov es, ax 
    mov fs, ax 
    mov gs, ax 
    mov ss, ax

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
