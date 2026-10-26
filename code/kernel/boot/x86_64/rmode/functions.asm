%ifndef DBL64_RMODE_FUNCTIONS_ASM
%define DBL64_RMODE_FUNCTIONS_ASM

%include "./boot/x86_64/rmode/data.asm"
%include "./boot/x86_64/print.asm"

[bits 16]
bios_check_extensions_present:
    pusha

    mov     ah,     0x41
    mov     bx,     0x55AA
    mov     dl,     [boot_drive]
    int     0x13

    jc      .not_found
    cmp     bx,     0xAA55
    jne     .not_found
    test    cx,     1
    jz      .not_found

    mov     si,     str_bios_disk_extension_present
    call    print_rmode

    popa
    ret

    .not_found:
        mov     si,     str_bios_disk_extension_not_present
        call    print_rmode
        jmp     endless_loop

[bits 16]
bios_extended_read_sectors_from_drive:
    pusha

    mov     si,                 bios_dap_read_packet

    mov     byte [si + 0],      0x10          ; размер пакета
    mov     byte [si + 1],      0             ; reserved

    mov     word [si + 2],      bx            ; количество секторов
    mov     word [si + 4],      di            ; смещение в сегменте назначения
    mov     word [si + 6],      cx            ; сегмент назначения

    mov     dword [si + 8],     eax           ; LBA (младшие 32 бита)
    mov     dword [si + 12],    0             ; LBA (старшие 32 бита)

    mov     ah,     0x42
    mov     si,     bios_dap_read_packet
    mov     dl,     [boot_drive]
    int     0x13

    jc      .error

    popa
    ret

.error:
    mov     [bios_disk_status], ah
    mov     si,     str_bios_disk_extension_loading_error
    call    print_rmode

    mov     al,     [bios_disk_status]
    call    print_hex_byte

    jmp     endless_loop

[bits 16]
endless_loop:
    cli
.end:
    hlt
    jmp .end

%endif
