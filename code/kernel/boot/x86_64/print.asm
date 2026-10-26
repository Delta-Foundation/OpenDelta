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

%ifndef BOOT_STAGE1
print_hex_word:
    pusha
    mov bx, ax

    mov al, bh
    call print_hex_nibble

    mov al, bl
    call print_hex_nibble

    popa
    ret
%endif

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

%ifndef BOOT_STAGE1
; --- Print in 32-bit Protected Mode --- ;
[bits 32]
print_pmode:
    pushad
    mov edi, 0xB8000

.recalc_pmode:
    mov eax, [current_row]
    imul eax, [screen_width]
    add eax, [current_column]
    shl eax, 1
    add edi, eax

.loop_pmode:
    lodsb 
    test al, al 
    jz .done_pmode 

    cmp al, 13
    je .cr_pmode
    cmp al, 10
    je .lf_pmode

    mov ah, 0x0F
    stosw 

    inc dword [current_column]
    cmp dword [current_column], 80
    jb .loop_pmode

    mov dword [current_column], 0
    inc dword [current_row]
    jmp .loop_pmode

.cr_pmode:
    mov dword [current_column], 0
    mov edi, 0xB8000
    jmp .recalc_pmode

.lf_pmode:
    inc dword [current_row]
    mov dword [current_column], 0
    mov edi, 0xB8000
    jmp .recalc_pmode

.done_pmode:
    popad
    ret

; --- Print in 64-bit Long Mode --- ;
[bits 64]
print_lmode:
    push rdi
    push rsi
    push rax
    push rbx

    mov rdi, 0xB8000
    mov ah, 0x0F

.loop_lmode:
    lodsb
    test al, al
    jz .done_lmode

    cmp al, 13
    je .loop_lmode
    cmp al, 10
    je .lf_lmode

    stosw
    jmp .loop_lmode

.lf_lmode:
    mov rax, rdi
    sub rax, 0xB8000
    xor edx, edx
    mov ebx, 160
    div rbx
    inc rax
    imul rax, 160
    lea rdi, [rax + 0xB8000]
    jmp .loop_lmode

.done_lmode:
    pop rbx
    pop rax
    pop rsi
    pop rdi
    ret
%endif

%endif
