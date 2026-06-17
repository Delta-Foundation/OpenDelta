[bits 16]
[org 0x7C00]

global _start

_start:
    cli

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov ax, 0x0003
    int 0x10

    mov [boot_drive], dl

    mov  si, str_real
    call print
    call print_nl

    call load_kernel

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

[bits 16]
load_kernel:
    pusha
    mov si, str_load
    call print

    mov ax, 0x1000
    mov es, ax

    mov bp, 89
    mov si, 1
    xor bx, bx

load_loop:
    mov ax, si
    xor dx, dx
    mov cx, 18
    div cx
    inc dx
    mov cl, dl

    xor dx, dx
    mov cx, 2
    div cx
    mov ch, al
    mov dh, dl

    mov ax, 0x0201      ; AH = 2 (читать), AL = 1 (сектор)
    mov dl, [boot_drive]

    push bp
    push si
    int 0x13
    pop si
    pop bp
    jc read_error

    add bx, 0x200
    inc si
    dec bp
    jnz load_loop

    mov si, str_kernel_loaded
    call print
    popa
    ret

read_error:
    mov si, str_read_fail
    call print
    jmp $

setup_gdt:
    cli
    lgdt [gdt_descriptor]
    ret

switch:
    call setup_gdt
    mov eax, cr0
    or al, 0x1
    mov cr0, eax

    jmp 0x08:protected_mode

[bits 32]
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x90000

    mov dword ebx, str_pmode
    call print_pmode

    mov eax, 0x10000
    jmp eax

print_pmode:
	pushad
    mov edx, VIDEO_MEMORY

print_pmode_loop:
	mov al, [ebx]
	test al, al
	jz  print_pmode_end

	mov ah, WHITE_ON_BLACK
	mov [edx], ax

	inc ebx
	add edx, 2

	jmp print_pmode_loop

print_pmode_end:
	popad
	ret

gdt_start:
    gdt_null:
        dd 0x0
        dd 0x0

    gdt_code:
        dw 0xFFFF       ; Лимит 0-15
        dw 0x0000         ; База 0-15
        db 0x00
        db 10011010b 	; P, DPL, S, Type flags
	    db 11001111b         ; Granularity, 32-bit, Лимит 16-19
        db 0x00          ; База 24-31

    gdt_data:
        dw 0xFFFF
        dw 0x0000
        db 0x00
        db 10010010b         ; Present, Ring 0, Data, Read/Write
        db 11001111b
        db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Лимит GDT
    dd gdt_start                ; Базовый адрес GDT

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
VIDEO_MEMORY    equ 0xB8000
WHITE_ON_BLACK  equ 0x0F

    ; data, strings, messages...
str_real:  db "[DBL]: Started in 16-bit real mode", 0x0D, 0x0A, 0
str_pmode: db "[DBL]: Landed in 32-bit protected mode. Jumping to kernel...", 0x0D, 0x0A, 0
str_load:  db "[DBL]: Loading dltkernel from the disk...", 0x0D, 0x0A, 0
str_kernel_loaded: db "[DBL]: Kernel loaded at 0x10000", 0x0D, 0x0A, 0

boot_drive:    db 0
boot_part_seg: dw 0
boot_part_off: dw 0

    ; errors
str_returned_kernel:  db "Returned from kernel. Error?", 0x0D, 0x0A, 0
str_read_fail:        db "[err]: [read failed!]", 0x0D, 0x0A, 0

times 510 - ($ - $$) db 0
dw 0xAA55
