[bits 64]

global idt_load
idt_load:
    push rbp
    mov rbp, rsp

    mov rax, [rbp + 8]
    lidt [rax]

    mov rsp, rbp
    sti
    pop rbp
    ret
