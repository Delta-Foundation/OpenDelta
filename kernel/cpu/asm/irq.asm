[bits 32]

; =============================================================================
; || IRQ (Interrupt Requests) - Hardware interrupts 0-15 mapped to IDT 32-47 ||
; =============================================================================

irq_common_stub:
	pusha

	mov	ax, ds
	push eax
	
    mov	ax, 0x10
	mov	ds, ax
	mov	es, ax
	mov fs, ax
	mov	gs, ax
    ; mov ss, ax 

    push esp 
	call irq_handler	; different than the ISR code
	add esp, 4 

	pop	eax
	mov	ds, bx
	mov	es, bx
	mov	fs, bx
	mov	gs, bx
    ; mov ss, bx

	popa
	add	esp, 8

	iret

global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

; IRQ 0 - Timer (PIT)
irq0:
    cli
    push byte 0            ; dummy error code
    push byte 32           ; interrupt number
    jmp irq_common_stub

; IRQ 1 - Keyboard
irq1:
    cli
    push byte 1
    push byte 33
    jmp irq_common_stub

; IRQ 2 - Cascade
irq2:
    cli
    push byte 2
    push byte 34
    jmp irq_common_stub

; IRQ 3 - COM2
irq3:
    cli
    push byte 3
    push byte 35
    jmp irq_common_stub

; IRQ 4 - COM1
irq4:
    cli
    push byte 4
    push byte 36
    jmp irq_common_stub

; IRQ 5 - LPT2
irq5:
    cli
    push byte 6
    push byte 37
    jmp irq_common_stub

; IRQ 6 - Floppy
irq6:
    cli
    push byte 6
    push byte 38
    jmp irq_common_stub

; IRQ 7 - LPT1 / Spurious
irq7:
    cli
    push byte 7
    push byte 39
    jmp irq_common_stub

; IRQ 8 - CMOS RTC
irq8:
    cli
    push byte 8
    push byte 40
    jmp irq_common_stub

; IRQ 9 - Free / ACPI
irq9:
    cli
    push byte 9
    push byte 41
    jmp irq_common_stub

; IRQ 10 - Free
irq10:
    cli
    push byte 10 
    push byte 42
    jmp irq_common_stub

; IRQ 11 - Free
irq11:
    cli
    push byte 11 
    push byte 43
    jmp irq_common_stub

; IRQ 12 - PS/2 Mouse
irq12:
    cli
    push byte 12 
    push byte 44
    jmp irq_common_stub

; IRQ 13 - FPU
irq13:
    cli
    push byte 13 
    push byte 45
    jmp irq_common_stub

; IRQ 14 - Primary ATA
irq14:
    cli
    push byte 14
    push byte 46
    jmp irq_common_stub

; IRQ 15 - Secondary ATA
irq15:
    cli
    push byte 15
    push byte 47
    jmp irq_common_stub
