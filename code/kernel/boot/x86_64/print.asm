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
