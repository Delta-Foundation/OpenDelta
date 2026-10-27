[bits 64]

; =============================================================================
; | IRQ (Interrupt Requests) - Hardware interrupts 0-15 mapped to IDT 32-47   |
; =============================================================================

global irq_common_stub

extern irq_handler

%macro IRQ 2
global irq%1
irq%1:
    push qword 0            ; dummy error code
    push qword %2           ; interrupt number
    jmp irq_common_stub
%endmacro

IRQ 0,  32
IRQ 1,  33
IRQ 2,  34
IRQ 3,  35
IRQ 4,  36
IRQ 5,  37
IRQ 6,  38
IRQ 7,  39
IRQ 8,  40
IRQ 9,  41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

irq_common_stub:
    cld

    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov r12, rsp            ; keep the frame pointer (r12 is callee-saved)
    mov rdi, rsp
    and rsp, -16            ; SysV requires 16-byte stack alignment
    call irq_handler
    mov rsp, r12

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 16             ; interrupt number + error code
    iretq
