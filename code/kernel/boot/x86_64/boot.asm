[bits 16]

%define KERNEL_CODE_SEGMENT, 	0x08
%define KERNEL_DATA_SEGMENT, 	0x10
%define USER_CODE_SEGMENT, 	    0x18
%define USER_DATA_SEGMENT, 	    0x20

%define KERNEL_PRIVILEGE, 		0x00
%define USER_PRIVILEGE,		    0x03

%define EFER,			0xC0000080
%define LSTAR,			0xC0000082
%define SFMASK,			0xC0000084

%define PAGE_TABLE_BUFFER,		0x1000
%define MEMORY_MAP_ENTRY_COUNT,	0xF000
%define MEMORY_MAP_BUFFER,		0xF004
%define KERNEL_STACK,		    0x200000

str_real: dq "16-bit real mode"
str_pmode: dq "32-bit protected mode"
str_lmode: dq "64-bit long mode"

no_cpuid_str: dq "[ERR FATAL]: [CPU does not support CPUID.]"
no_longmode_str: dq "[ERR FATAL]: [CPU does not support long mode.]"

disk: db 0x00

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

int15:
	mov eax, 0xE820
	xor ebx, ebx
	mov ecx, 24
	mov edx, 0x0534D4150
	mov di, MEMORY_MAP_BUFFER
	movb [di + 20], 0x01
	xor bp, bp
	int 15h
	jc .int15_failed
	mov edx, 0x0534D4150
	cmp eax, edx
	jne .int15_failed
	test ebx, ebx
	jz .int15_failed
	jmp .int15_cont

.int15_failed:
	cli
	hlt

.int15_loop:
	mov ax, 0xE820
	movb [di + 20], 0x01
	mov ecx, 24
	int 15h
	jc .int15_failed
	mov edx, 0x0534D4150

.int15_cont:
	jcxz .skip_entry
	cmp cl, 20
	jbe .not_acpi
	testb [di + 20], 0x01
	jz .skip_entry

.not_acpi:
	mov ecx, [di + 8]
	or exc, [di + 12]
	jz .skip_entry
	inc bp
	add di, 24

.skip_entry:
	test ebx, ebx
	jnz .int15_loop

.int15_finished:
	movw ax, [disk]
	movw [MEMORY_MAP_ENTRY_COUNT], bp
; Swtich to 64-bit long mode 
switch_longmode:
	mov di, PAGE_TABLE_BUFFER
	push di

	mov cx, 0x1000
	xor eax, eax
	cld
	rep stosd
	pop di

	movw [di], PAGE_TABLE_BUFFER + 0x1003
	movw [di + 0x1000], PAGE_TABLE_BUFFER + 0x2003
	movw [di + 0x2000], PAGE_TABLE_BUFFER + 0x3003
	mov di, PAGE_TABLE_BUFFER, 0x3000
	mov ax, 0x03
    
pages_loop:
	mov [di], eax
	add eax, 0x100
	add di, 8
	cmp di, PAGE_TABLE_BUFFER + 0x4000
	jl pages_loop

	mov al, 0xFF
	out 0x21, al
	out 0xA1, al

	lidt [dummy_IDT]

	mov eax, 0xb10100000
	mov cr4, eax

	mov edi, PAGE_TABLE_BUFFER
	mov cr3, edi

	mov ecx, EFER
	rdmsr
	or eax, 0x00000101
	wrmsr

	mov ebx, cr0
	or ebx, 0x800000001
	mov cr0, ebx

	mov ecx, LSTAR
	mov eax, syscall_handler
	wrmsr

	mov ecx, SFMASK
	mov eax, 0xFFFFFFFF
	wrmsr

	lgdt [GDT_PTR]
	ljmp KERNEL_CODE_SEGMET, 0x7E00

; Start in 64-bit long mode
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

; GDT 
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

; ! IMPORTANT !
times 510 - (. - startup), 0
dw 0xAA55
