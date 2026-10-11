%ifndef DBL64_RMODE_FUNCTIONS_ASM
%define DBL64_RMODE_FUNCTIONS_ASM

%include "./boot/x86_64/rmode/data.asm"
%include "./boot/x86_64/print.asm"

[bits 16]
bios_check_extensions_present:
    pusha 

    mov     ah,     0x41 
    mov     dl,     [boot_drive]
    mov     bx,     0x55AA
    int     0x13 

    jc      .not_found
    cmp     bx,     0xAA55 
    jne     .not_found
    test    cx,     1
    jz      .not_found

    mov     si,     str_bios_disk_extension_present
    call    print_rmode
    mov     ax,     cx
    call    print_hex_word
    popa
    ret

    .not_found:
        mov     si,     str_bios_disk_extension_not_present
        call    print_rmode 
        jmp     endless_loop

bios_extended_read_sectors_from_drive:
    pusha

    mov     si,              bios_dap_read_packet
    mov     byte [si],       0x10
    mov     byte [si + 1],   0
    mov     word [si + 2],   bx
    mov     word [si + 4],   ax
    mov     word [si + 6],   0x0000
    mov     dword [si + 8],  ecx
    mov     dword [si + 12], 0

    mov     ah,     0x42
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

endless_loop:
    cli 
    .end:
        hlt
        jmp .end

%endif
