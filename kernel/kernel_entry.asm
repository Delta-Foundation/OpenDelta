[bits 32]

; asm functions
global _start
global readp  ; read port
global writep ; write port
[extern __bss_start]
[extern __bss_end]

; C functions
[extern kmain]

VIDEO_MEMORY    equ 0xB8000
KERNEL_CS       equ 0x08
KERNEL_DS       equ 0x10

section .text._start

_start:
    cli

    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, stack_top

    ; Marker 1: segments + stack ready. Kernel started
    mov dword [VIDEO_MEMORY], 0x2F312F31

    mov edi, __bss_start
    mov ecx, __bss_end
    sub ecx, edi
    xor al, al
    cld
    rep stosb

    ; Marker 2: BSS cleared
    mov dword [VIDEO_MEMORY + 4], 0x2F322F32

    ; Marker 3: call kmain
    mov dword [VIDEO_MEMORY + 8], 0x2F332F33

    call kmain

    cli
.halt:
    hlt
    jmp .halt

readp:
    push ebp

    mov ebp, esp
    xor eax, eax
    mov dx, word [ebp + 8]
    mov al, byte [ebp + 12]
    in al, dx

    pop ebp
    ret

writep:
    push ebp

    mov ebp, esp
    mov dx, word [ebp + 8]
    mov al, byte [ebp + 12]
    out dx, al

    pop ebp
    ret

section .bss
    align 16
    stack_bot:
        resb 16384
    stack_top:
