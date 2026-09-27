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
