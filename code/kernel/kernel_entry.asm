[bits 64]

section .data
    ; asm functions
    global readp  ; read port
    global writep ; write port
    
    VIDEO_MEMORY    equ 0xB8000
    KERNEL_CS       equ 0x08
    KERNEL_DS       equ 0x10

section .text 
    global _start

    [extern __bss_start]
    [extern __bss_end]

    ; C functions
    [extern kmain]

VIDEO_MEMORY    equ 0xB8000
KERNEL_CS       equ 0x08
KERNEL_DS       equ 0x10

_start:
    cli

    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rsp, stack_top
    and rsp, 0xFFFFFFF0
    sub rsp, 4

    ; Marker 1: segments + stack ready. Kernel started
    mov dword [VIDEO_MEMORY], 0x2F312F31

    mov rdi, __bss_start
    mov rcx, __bss_end
    cmp rcx, rdi
    jbe .bss_clear_done

    sub rcx, rdi
    xor rax, rax
    cld
    rep stosb
.bss_clear_done:

    ; Marker 2: BSS cleared
    mov dword [VIDEO_MEMORY + 4], 0x2F322F32

    ; Marker 3: call kmain
    mov dword [VIDEO_MEMORY + 8], 0x2F332F33

    mov rax, kmain
    test rax, rax
    jz .halt

    call kmain

    cli
.halt:
    hlt
    jmp .halt

readp:
    push rbp

    mov rbp, rsp
    xor rax, rax
    mov dx, word [rbp + 8]
    mov al, byte [rbp + 12]
    in al, dx

    pop rbp
    ret

writep:
    push rbp

    mov rbp, rsp
    mov dx, word [rbp + 8]
    mov al, byte [rbp + 12]
    out dx, al

    pop rbp
    ret

section .bss
    align 16
    stack_bot:
        resb 16384
    stack_top:
