[bits 64]

%include "cpu/asm/isr.asm"
%include "cpu/asm/irq.asm"

extern isr_handler
extern irq_handler

global enable_ints
global disable_ints
global panic 
global crash_me

enable_ints:
    sti
    ret

disable_ints:
    cli
    ret

panic:
    cli
    hlt

crash_me:
    ; div by 0
    ; mov rcx, 0x1337
    ; mov rax, 0
    ; div rax
    int 0x80
    ret
