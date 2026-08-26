; --- Print in 16-bit Real Mode --- ;
print_rmode:
    pusha
.loop:
    lodsb
    test al, al 
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp .loop
.done:
    popa 
    ret 

; --- Print in 32-bit Protected Mode --- ;
print_pmode:
    push edi 
    push esi 
.loop:
    lodsb 
    test al, al 
    jz .done 
    stosw 
    jmp .loop 
.done:
    pop esi 
    pop edi 
    ret

; --- Print in 64-bit Long Mode --- ;
print_lmode:
    push rdi
    push rsi
.loop:
    lodsb
    test al, al
    jz .done
    stosw
    jmp .loop
.done:
    pop rsi
    pop rdi
    ret
