[bits 16]

%include "boot/x86_64/constants.asm"
%include "boot/x86_64/int15.asm"
%include "boot/x86_64/switch64.asm"

section .data
    str_real: dq "16-bit real mode"
    str_pmode: dq "32-bit protected mode"
    str_lmode: dq "64-bit long mode"

	no_cpuid_str: dq "[ERR FATAL]: [CPU does not support CPUID.]"
	no_longmode_str: dq "[ERR FATAL]: [CPU does not support long mode.]"

	disk: db 0x00

section .boot
    global 	startup
    global 	init

startup:
	ljmp 0x10000, flush_segments

flush_segments:
	cld
	mov sp, startup
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax

	movb [disk], dl
	mov ax, 0x0241
	mov bx, 0x7E00
	mov cx, 0x0002
	xor dh , dh
	int 13h

check_cpuid:
	pushfd

	pop eax
	mov ecx, eax
	xor eax, (1 << 21)
	push eax
	
	popfd
	
	pushfd
	pop eax
	push ecx
	popfd
	
	xor eax, ecx
	jz no_cpuid

	mov eax, 0x80000000
	cpuid
	cmp eax, 0x80000001
	jz no_longmode

	mov ax, 0x2401
	int 15h

	mov ax, 0x03
	int 10h

no_cpuid:
	mov si, no_cpuid_str
	call printstr_rmode
	
	cli
	hlt

no_longmode:
	mov si, no_longmode_str
	call printstr_rmode
	
	cli
	hlt

printstr_rmode:
	lodsb
	test a1, a1
	jz .done
	mov ah, 0x0E
	int 10h
	jmp printstr_rmode

.done:
	ret

dummy_IDT:
	dw 	0x00
	dq 	0x00

[bits 64]
long_mode:
	mov ax, KERNEL_DATA_SEGMENT
	mov, ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	cli

	xor rax, rax
	xor rbx, rbx
	xor rcx, rcx
	xor rdx, rdx
	xor rdi, rdi

	mov rsp, KERNEL_STACK
	mov rbp, rsp

	call kmain

	cli
	hlt

syscall_handler:
	systetq

GDT:
.null:
	dq 0x00
.kcode:
	dq 0x00209A0000000000
.kdata
	dq 0x0000920000000000
.ucode:
	dq 0x0000FA0000000000
.udata:
	dq 0x0000F20000000000
GDT_END:
GDT_PTR:
	dw GDT_END - GDT - 1
	dq GDT

times 510 - (. - startup), 0
dw 0xAA55
