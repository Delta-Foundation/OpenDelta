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
