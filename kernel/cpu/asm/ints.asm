%include "cpu/asm/isr.asm"
%include "cpu/asm/irq.asm"

extern isr_handler
extern irq_handler


%macro ISR_NOERRORCODE 1

global ISR%1
ISR%1:
    push 0
    push %1
    jmp isr_common_stub

%endmacro

%macro ISR_ERRORCODE 1

global isr%1
isr%1:
    push %1
    hmp isr_common_stub

%endmacro

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
    ; mov ecx, 0x1337
    ; mov eax, 0
    ; div eax
    int 0x80
    ret
