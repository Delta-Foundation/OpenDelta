[bits 16]
[org 0x7C00]

global _start
    
_start:
    cli 

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax 
    mov sp, 0x7c00

    call clear_screen

    mov [boot_drive], dl
    
    mov  si, str_real 
    call print
    call print_nl

    call load_kernel

    call setup_gdt
    call switch

    jmp $

print:
    pusha

print_loop:
    lodsb
    test al, al
    jz done

    mov ah, 0x0E
    mov bh, 0
    int 0x10

    jmp print_loop

done:
    popa
    ret

print_nl:
    pusha

    mov ah, 0x0E
    mov al, 0x0D
    int 0x10
    mov al, 0x0A
    int 0x10

    popa
    ret

clear_screen:
    pusha

    mov ah, 0x00
    mov al, 0x03 
    int 0x10 
    
    popa 
    ret 

[bits 16]
load_kernel:
    pusha 

    mov	si, str_load
    call print
    call print_nl
    
    push dx 

    mov ah, 0x02
    mov al, 32
    mov ch, 0x00 
    mov dh, 0x00  
    mov dl, [boot_drive]
    mov cl, 0x02 
    
    int 0x13
    jc read_error

    pop dx
    cmp al, 32
    jne read_error


    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    popa
    ret

read_error:
    mov bx, str_read_fail
    call print
    jmp $

setup_gdt:
    lgdt [gdt_descriptor]
    ret

switch:
    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp CODE_SEG:init

[bits 32]
init:
    jmp CODE_SEG:protected_mode

protected_mode:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x90000

    lea ebx, str_pmode
    call print_pmode

    jmp $

print_pmode:
	pusha
    mov edx, VIDEO_MEMORY

print_pmode_loop:
	mov al, [ebx]
	mov ah, WHITE_ON_BLACK
	test al, al
	jz  print_pmode_end

	mov [edx], ax

	add ebx, 1
	add edx, 2

	jmp print_pmode_loop

print_pmode_end:
	popa
	ret

gdt_start:
    gdt_null:
        dq 0

    gdt_code:
        dw 0xFFFF       ; Лимит 0-15
        dw 0x0          ; База 0-15
        db 0x0
        db 10011010b 	; P, DPL, S, Type flags
	    db 11001111b         ; Granularity, 32-bit, Лимит 16-19
        db 0x0          ; База 24-31

    gdt_data:
        dw 0xFFFF
        dw 0x0
        db 0x0
        db 10010010b         ; Present, Ring 0, Data, Read/Write
        db 11001111b
        db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start + 1  ; Лимит GDT
    dd gdt_start                ; Базовый адрес GDT


CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
VIDEO_MEMORY    equ 0xB8000
WHITE_ON_BLACK  equ 0x0F

    ; data, strings, messages...
str_real:  db "Started in 16-bit real mode", 0x0D, 0x0A, 0
str_pmode: db "Landed in 32-bit protected mode", 0x0D, 0x0A, 0
str_load:  db "Loading dltkernel from the disk", 0x0D, 0x0A, 0

boot_drive:    db 0
boot_part_seg: dw 0
boot_part_off: dw 0

    ; errors
str_returned_kernel:  db "Returned from kernel. Error?", 0x0D, 0x0A, 0
str_read_fail:        db "[err]: [read failed!]", 0x0D, 0x0A, 0

times 510 - ($-$$) db 0
db 0x55, 0xAA
