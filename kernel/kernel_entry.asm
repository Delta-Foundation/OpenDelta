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

    ; ======== встроенный GDT, будем использовать вместо сишного GDT ==========
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
    stack_bot: resb 0x90000
    stack_top:

section .text

_start:
    cli
    cld

    mov al, 0xFF
    out 0x21, al 
    out 0xA1, al 

    ; Start entry point. Marker 1 
    mov dword [VIDEO_MEMORY], 0x2F312F31

    mov edi, __bss_start
    mov ecx, __bss_end 
    sub ecx, edi 
    shr ecx, 2
    xor eax, eax 
    rep stosd

    ; BSS clear; Marker 2 
    mov dword [VIDEO_MEMORY + 4], 0x2F322F32

    ; loading kernel GDT 
    lgdt [gdt_descriptor]
    jmp 0x08:.reload_cs 

.reload_cs:
    mov ax, 0x10
    mov ds, ax 
    mov es, ax 
    mov fs, ax 
    mov gs, ax 
    mov ss, ax 
    mov esp, stack_top
   
    ; Stack ready; Marker 3 
    mov dword [VIDEO_MEMORY + 8], 0x2F332F33

    ; Call kmain; Marker 4
    mov dword [VIDEO_MEMORY + 12], 0x2F342F34 

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

load_idt:
    mov edx, [esp + 4]
    lidt [edx]
    ret
