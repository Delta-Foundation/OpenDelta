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
    VIDEO_MEMORY equ 0xB8000

section .rodata
    ; ======== встроенный GDT, будем использовать вместо сишного GDT ==========
    
    align 8 
    gdt_start:
        gdt_null:
            dd 0x0
            dd 0x0

        gdt_code:
            dw 0xFFFF, 0x0000
            db 0x00, 10011010b, 11001111b, 0x00

        gdt_data:
            dw 0xFFFF, 0x0000
            db 0x00, 10010010b, 11001111b, 0x00

    gdt_end:

    gdt_descriptor:
        dw gdt_end - gdt_start - 1  ; Лимит GDT
        dd gdt_start                ; Базовый адрес GDT

section .bss
    align 16
    stack_bot: resb 0x10000
    stack_top:

section .text

_start:
    cli

    mov al, 0xFF
    out 0x21, al 
    out 0xA1, al 

    mov ax, 0x10
    mov ds, ax 
    mov es, ax 
    mov fs, ax 
    mov gs, ax 
    mov ss, ax 

    mov esp, stack_top

    ; Marker 1: segments + stack ready 
    mov dword [VIDEO_MEMORY], 0x2F312F31 

    lgdt [gdt_descriptor]
    jmp 0x08:.flush_cs

.flush_cs:
    mov ax, 0x10 
    mov ds, ax 
    mov es, ax 
    mov fs, ax 
    mov gs, ax 
    mov ss, ax 
    mov esp, stack_top
    
    ; Marker 2: GDT loaded and segments reloaded
    mov dword [VIDEO_MEMORY + 4], 0x2F322F32

    mov edi, __bss_start 
    mov ecx, __bss_end 
    sub ecx, edi 
    xor al, al 
    cld 
    rep stosb

    ; Marker 3: BSS cleared 
    mov dword [VIDEO_MEMORY + 8], 0x2F332F33 

    mov esp, stack_top
   
    ; Marker 4: stack ready (again, after BSS clear)
    mov dword [VIDEO_MEMORY + 12], 0x2F342F34 

    ; Marker 5: call kmain
    mov dword [VIDEO_MEMORY + 16], 0x2F352F35 

    call kmain 

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
