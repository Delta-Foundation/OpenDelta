section .data align=8

idt:
    times 256 dq 0
idt_end:

idtr:
    dw (idt_end - idt - 1)
    dd idt


section .text
    global setup_idt

[bits 16]
setup_idt:
    lidt [idtr]
    ret

disable_timer:
    pushf
    mov al, 0xFF
    out 0x21, al 
    out 0xA1, al
    popf
    ret
