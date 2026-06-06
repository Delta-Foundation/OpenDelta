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
    VIDEO_MEMORY equ 0xB8000

section .bss
    align 16
    stack_bot: resb 0x90000
    stack_top:

section .text

_start:
    cli
    cld

    ; Start entry point. Marker 1 
    mov dword [0xB8000], 0x2F312F31

    mov edi, __bss_start
    mov ecx, __bss_end 
    sub ecx, edi 
    shr ecx, 2
    xor eax, eax 
    rep stosd

    ; BSS clear; Marker 2 
    mov dword [0xB8004], 0x2F322F32

    mov esp, stack_top
   
    ; Stack ready; Marker 3 
    mov dword [0xB8008], 0x2F332F33

    ; Call kmain; Marker 4
    mov dword [0xB800C], 0x2F342F34 

    call kmain 

    cli

.halt:
    hlt
    jmp .halt

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
