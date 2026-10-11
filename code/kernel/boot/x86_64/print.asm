%ifndef DBL64_PRINT_ASM
%define DBL64_PRINT_ASM

; --- Print in 16-bit Real Mode --- ;
[bits 16]
print_rmode:
    pusha
.loop_rmode:
    lodsb
    test al, al 
    jz .done_rmode
    mov ah, 0x0E
    int 0x10
    jmp .loop_rmode 
.done_rmode:
    popa 
    ret 

print_hex_word:
    pusha
    mov bx, ax

    mov al, bh
    call print_hex_nibble

    mov al, bl
    call print_hex_nibble

    popa
    ret

print_hex_byte:
    pusha
    mov ah, 0x0E
    mov bl, al

    shr al, 4
    call print_hex_nibble

    mov al, bl
    and al, 0x0F
    call print_hex_nibble

    popa
    ret

print_hex_nibble:
    cmp al, 9
    jbe .digit
    add al, 'A' - 10
    jmp .print
.digit:
    add al, '0'
.print:
    int 0x10
    ret

; --- Print in 32-bit Protected Mode --- ;
[bits 32]
print_pmode:
    push edi 
    push esi 
.loop_pmode:
    lodsb 
    test al, al 
    jz .done_pmode 
    stosw 
    jmp .loop_pmode
.done_pmode:
    pop esi 
    pop edi 
    ret

; --- Print in 64-bit Long Mode --- ;
[bits 64]
print_lmode:
    push rdi
    push rsi
.loop_lmode:
    lodsb
    test al, al
    jz .done_lmode
    stosw
    jmp .loop_lmode 
.done_lmode:
    pop rsi
    pop rdi
    ret

%endif
