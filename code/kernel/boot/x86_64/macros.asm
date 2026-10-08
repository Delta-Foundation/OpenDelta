%ifndef DBL64_MACROS_ASM
%define DBL64_MACROS_ASM

%macro pusha64 0
    push rax
    push rcx 
    push rdx 
    push rbx 
    push rsp 
    push rbp 
    push rsi 
    push rdi
%endmacro

%macro pushaq 0
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro pushacr 0
	mov rax, cr0
    push rax

    mov rax, cr2
    push rax

    mov rax, cr3
    push rax

    mov rax, cr4
    push rax

	mov rax, cr8
    push rax
%endmacro

%macro popa64 0 
    pop rdi 
    pop rsi 
    pop rbp 
    pop rsp 
    pop rbx 
    pop rdx 
    pop rcx 
    pop rax 
%endmacro 

%macro popaq 0
    pop r15 
    pop r14 
    pop r13 
    pop r12 
    pop r11 
    pop r10 
    pop r9 
    pop r8 
    pop rsi 
    pop rdi 
    pop rbp 
    pop rdx 
    pop rcx 
    pop rbx 
    pop rax 
%endmacro 

%endif
