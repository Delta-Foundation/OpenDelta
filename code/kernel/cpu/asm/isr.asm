[bits 64]

; ==============================================================
; ISR (Interrupt Service Routines) - Expection handlers 0-31
; ==============================================================

global isr_common_stub

isr_common_stub:
	push rdi 
    push rsi 
    push rbp
    push rsp 
    push rbx 
    push rdx 
    push rcx 
    cld

    xor rax, rax
    mov	ax, ds		; lower 16-bits of eax = ds
	push rax             ; save the data segment descriptor

    mov	ax, 0x10	; kernel data segment descriptor
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax

    push rsp
    extern isr_handler
    call	isr_handler
    add rsp, 4

	pop	rax
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax

	pop rcx
    pop rdx 
    pop rbx 
    pop rsp 
    pop rbp
    pop rsi 
    pop rdi
	add rsp, 8		; cleans up the pushed error code and pushed ISR number
    iret			; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31

; 0: Divide by Zero (no error code)
isr0:
    cli
    push byte 0
    push byte 0        ; interrupt number
    jmp isr_common_stub

; 1: Debug (no error code)
isr1:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

; 2: NMI (no error code)
isr2:
    cli
    push byte 0
    push byte 2
    jmp isr_common_stub

; 3: Breakpoint (no error code)
isr3:
    cli
    push byte 0
    push byte 3
    jmp isr_common_stub

; 4: Overflow (no error code)
isr4:
    cli
    push byte 0
    push byte 4
    jmp isr_common_stub

; 5: Bound Range Exceeded (no error code)
isr5:
    cli
    push byte 0
    push byte 5
    jmp isr_common_stub

; 6: Invalid Opcode (no error code)
isr6:
    cli
    push byte 0
    push byte 6
    jmp isr_common_stub

; 7: Device Not Available (no error code)
isr7:
    cli
    push byte 0
    push byte 7
    jmp isr_common_stub

; 8: Double Fault (HAS error code from CPU)
isr8:
    cli
    ; CPU pushes error code
    push byte 8
    jmp isr_common_stub

; 9: Coprocessor Segment Overrun (no error code)
isr9:
    cli
    push byte 0
    push byte 9
    jmp isr_common_stub

; 10: Invalid TSS (HAS error code)
isr10:
    cli
    push byte 10
    jmp isr_common_stub

; 11: Segment Not Present (HAS error code)
isr11:
    cli
    push byte 11
    jmp isr_common_stub

; 12: Stack-Segment Fault (HAS error code)
isr12:
    cli
    push byte 12
    jmp isr_common_stub

; 13: General Protection Fault (HAS error code)
isr13:
    cli
    push byte 13
    jmp isr_common_stub

; 14: Page Fault (HAS error code)
isr14:
    cli
    push byte 14
    jmp isr_common_stub

; 15: Reserved (no error code)
isr15:
    cli
    push byte 0
    push byte 15
    jmp isr_common_stub

; 16: x87 FPU Error (no error code)
isr16:
    cli
    push byte 0
    push byte 16
    jmp isr_common_stub

; 17: Alignment Check (HAS error code)
isr17:
    cli
    push byte 17
    jmp isr_common_stub

; 18: Machine Check (no error code)
isr18:
    cli
    push byte 0
    push byte 18
    jmp isr_common_stub

; 19: SIMD FP Exception (no error code)
isr19:
    cli
    push byte 0
    push byte 19
    jmp isr_common_stub

; 20: Virtualization Exception (no error code)
isr20:
    cli
    push byte 0
    push byte 20
    jmp isr_common_stub

; 21: Control Protection Exception (HAS error code)
isr21:
    cli
    push byte 21
    jmp isr_common_stub

; 22: Reserved (no error code)
isr22:
    cli
    push byte 0
    push byte 22
    jmp isr_common_stub

; 23: Reserved (no error code)
isr23:
    cli
    push byte 0
    push byte 23
    jmp isr_common_stub

; 24: Reserved (no error code)
isr24:
    cli
    push byte 0
    push byte 24
    jmp isr_common_stub

; 25: Reserved (no error code)
isr25:
    cli
    push byte 0
    push byte 25
    jmp isr_common_stub

; 26: Reserved (no error code)
isr26:
    cli
    push byte 0
    push byte 26
    jmp isr_common_stub

; 27: Reserved (no error code)
isr27:
    cli
    push byte 0
    push byte 27
    jmp isr_common_stub

; 28: Reserved (no error code)
isr28:
    cli
    push byte 0
    push byte 28
    jmp isr_common_stub

; 29: VMM Communication Exception (HAS error code)
isr29:
    cli
    push byte 29
    jmp isr_common_stub

; 30: Security Exception (HAS error code)
isr30:
    cli
    push byte 30
    jmp isr_common_stub

; 31: Reserved (no error code)
isr31:
    cli
    push byte 0
    push byte 31
    jmp isr_common_stub
