section .data
    error_msg: 
        db "[err]: disk read error", 0x0D, 0x0A, 0x0
        db "[err]: floppy read error", 0x0D, 0x0A, 0x0
        
    have_extensions:        db 0
    extensions_dap:
        .size:              db 10h
                            db 0
        .count:             dw 0
        .offset:            dw 0
        .segment:           dw 0
        .lba:               dq 0

    STAGE2_LOAD_SEGMENT     equ 0x0
    STAGE2_LOAD_OFFSET      equ 0x500

    PARTITION_ENTRY_SEGMENT equ 0x2000
    PARTITION_ENTRY_OFFSET  equ 0x0

disk_load:
    pusha
    push eax 
    push bx
    push cx
    push dx
    push si 
    push di

    push bp
    mov bp, sp

    mov ah, 0x02
    mov ch, 0x00
    mov dl, 0x00
    mov dh, 0x00 
    mov cl, 0x02

    int 0x13
    jc disk_error

    pop dx
    cmp al, dh
    jne sectors_error

    jmp disk_done

disk_done:
    popa 
    pop di
    pop si 
    pop dx
    pop cx
    pop bx 
    pop bp
    pop eax
    ret

disk_error:
    mov bx, error_msg
    call print
    call print_nl
    mov dh, ah
    call print_hex
    jmp $

sectors_error:
    mov bx, error_msg
    call print

stage2_location: times 30 db 0
