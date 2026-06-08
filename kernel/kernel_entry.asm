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

VIDEO_MEMORY    equ 0xB8000
KERNEL_CS       equ 0x08 
KERNEL_DS       equ 0x10 

section .text

_start:
    cli

    mov ax, KERNEL_DS
    mov ds, ax 
    mov es, ax 
    mov fs, ax 
    mov gs, ax 
    mov ss, ax 

    mov esp, stack_top

    ; Marker 1: segments + stack ready 
    mov dword [VIDEO_MEMORY], 0x2F312F31 
    
    mov edi, __bss_start 
    mov ecx, __bss_end 
    sub ecx, edi 
    xor al, al 
    cld 
    rep stosb

    ; Marker 2: BSS cleared 
    mov dword [VIDEO_MEMORY + 4], 0x2F322F32 

    lgdt [gdt_descriptor]
    jmp KERNEL_CS:.flush_cs

.flush_cs:
    mov ax, KERNEL_DS 
    mov ds, ax 
    mov es, ax 
    mov fs, ax 
    mov gs, ax 
    mov ss, ax 
    mov esp, stack_top
    
    ; Marker 3: GDT loaded and segments reloaded
    mov dword [VIDEO_MEMORY + 8], 0x2F332F33

    call kmain 

    ; Marker 4: call kmain
    mov dword [VIDEO_MEMORY + 12], 0x2F342F34 

    cli
.halt:
    hlt
    jmp .halt

readp:
    push ebp

    mov ebp, esp
    xor eax, eax
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
    mov edx, [ebp + 8]
    test edx, edx 
    jz .skip_idt 
    lidt [edx]
.skip_idt:
    pop ebp 
    ret

section .rodata
    ; ======== встроенный GDT, будем использовать вместо сишного GDT ==========
    align 8 
    gdt_start:
        gdt_null:
            dq 0x0

        gdt_code:
            dw 0xFFFF, 0x0000
            db 0x00, 0x9A, 0xCF, 0x00

        gdt_data:
            dw 0xFFFF, 0x0000
            db 0x00, 0x92, 0xCF, 0x00

    gdt_end:

    gdt_descriptor:
        dw gdt_end - gdt_start - 1  ; Лимит GDT
        dd gdt_start                ; Базовый адрес GDT

section .bss
    align 16
    stack_bot: resb 0x10000
    stack_top:
