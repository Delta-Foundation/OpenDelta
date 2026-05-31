irq_common_stub:
	pusha
	mov	ax, ds
	push eax
	mov	ax, 0x10
	mov	ds, ax
	mov	es, ax
	mov fs, ax
	mov	gs, ax
	
	call irq_handler	; different than the ISR code
	
	pop	ebx
	mov	ds, bx
	mov	es, bx
	mov	fs, bx
	mov	gs, bx
	popa
	add	esp, 8
	
	iret

%macro IRQ 2
	global irq%1
	irq%1:
    	cli
    	push dword 0           ; error code (dummy)
    	push dword %2          ; interrupt number (32+IRQ)
    	jmp irq_common_stub
%endmacro

%define IRQ 0, 32
%define IRQ 1, 33
%define IRQ 2, 34
%define IRQ 3, 35
%define IRQ 4, 36
%define IRQ 5, 37
%define IRQ 6, 38
%define IRQ 7, 39
%define IRQ 8, 40
%define IRQ 9, 41
%define IRQ 10, 42
%define IRQ 11, 43
%define IRQ 12, 44
%define IRQ 13, 45
%define IRQ 14, 46
%define IRQ 15, 47

; IRQs
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

irq0:
	cli
	push	byte 0
	push 	byte 32
	jmp	irq_common_stub

irq1:
	cli
	push	byte 1
	push 	byte 33
	jmp	irq_common_stub

irq2:
	cli
	push	byte 2
	push 	byte 34
	jmp	irq_common_stub

irq3:
	cli
	push	byte 3
	push 	byte 35
	jmp	irq_common_stub

irq4:
	cli
	push	byte 4
	push 	byte 36
	jmp	irq_common_stub

irq5:
	cli
	push	byte 5
	push 	byte 37
	jmp	irq_common_stub

irq6:
	cli
	push	byte 6
	push 	byte 38
	jmp	irq_common_stub

irq7:
	cli
	push	byte 7
	push 	byte 39
	jmp	irq_common_stub

irq8:
	cli
	push	byte 8
	push 	byte 40
	jmp	irq_common_stub

irq9:
	cli
	push	byte 9
	push 	byte 41
	jmp	irq_common_stub

irq10:
	cli
	push	byte 10
	push 	byte 42
	jmp	irq_common_stub

irq11:
	cli
	push	byte 11
	push 	byte 43
	jmp	irq_common_stub

irq12:
	cli
	push	byte 12
	push 	byte 44
	jmp	irq_common_stub

irq13:
	cli
	push	byte 13
	push 	byte 45
	jmp	irq_common_stub

irq14:
	cli
	push	byte 14
	push 	byte 46
	jmp	irq_common_stub

irq15:
	cli
	push	byte 15
	push 	byte 47
	jmp	irq_common_stub
