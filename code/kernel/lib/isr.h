#ifndef ISR_H
#define ISR_H

#include "types.h"

// ISRs reserved for CPU exceptions
extern void __attribute__((cdecl)) isr0(void);
extern void __attribute__((cdecl)) isr1(void);
extern void __attribute__((cdecl)) isr2(void);
extern void __attribute__((cdecl)) isr3(void);
extern void __attribute__((cdecl)) isr4(void);
extern void __attribute__((cdecl)) isr5(void);
extern void __attribute__((cdecl)) isr6(void);
extern void __attribute__((cdecl)) isr7(void);
extern void __attribute__((cdecl)) isr8(void);
extern void __attribute__((cdecl)) isr9(void);
extern void __attribute__((cdecl)) isr10(void);
extern void __attribute__((cdecl)) isr11(void);
extern void __attribute__((cdecl)) isr12(void);
extern void __attribute__((cdecl)) isr13(void);
extern void __attribute__((cdecl)) isr14(void);
extern void __attribute__((cdecl)) isr15(void);
extern void __attribute__((cdecl)) isr16(void);
extern void __attribute__((cdecl)) isr17(void);
extern void __attribute__((cdecl)) isr18(void);
extern void __attribute__((cdecl)) isr19(void);
extern void __attribute__((cdecl)) isr20(void);
extern void __attribute__((cdecl)) isr21(void);
extern void __attribute__((cdecl)) isr22(void);
extern void __attribute__((cdecl)) isr23(void);
extern void __attribute__((cdecl)) isr24(void);
extern void __attribute__((cdecl)) isr25(void);
extern void __attribute__((cdecl)) isr26(void);
extern void __attribute__((cdecl)) isr27(void);
extern void __attribute__((cdecl)) isr28(void);
extern void __attribute__((cdecl)) isr29(void);
extern void __attribute__((cdecl)) isr30(void);
extern void __attribute__((cdecl)) isr31(void);
// IRQ Definitions
extern void __attribute__((cdecl)) irq0(void);
extern void __attribute__((cdecl)) irq1(void);
extern void __attribute__((cdecl)) irq2(void);
extern void __attribute__((cdecl)) irq3(void);
extern void __attribute__((cdecl)) irq4(void);
extern void __attribute__((cdecl)) irq5(void);
extern void __attribute__((cdecl)) irq6(void);
extern void __attribute__((cdecl)) irq7(void);
extern void __attribute__((cdecl)) irq8(void);
extern void __attribute__((cdecl)) irq9(void);
extern void __attribute__((cdecl)) irq10(void);
extern void __attribute__((cdecl)) irq11(void);
extern void __attribute__((cdecl)) irq12(void);
extern void __attribute__((cdecl)) irq13(void);
extern void __attribute__((cdecl)) irq14(void);
extern void __attribute__((cdecl)) irq15(void);

#define IRQ0	32
#define IRQ1	33
#define IRQ2	34
#define IRQ3	35
#define IRQ4	36
#define IRQ5	37
#define IRQ6	38
#define IRQ7	39
#define IRQ8	40
#define IRQ9	41
#define IRQ10	42
#define IRQ11	43
#define IRQ12	44
#define IRQ13	45
#define IRQ14	46
#define IRQ15	47

#define PIC_REMAP_OFFSET 0x20
#define MODULE "PIC"

typedef struct {
	uint32_t ds;
	uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax;
	uint32_t int_no, err_code;
	uint32_t eip, cs, eflags, esp, ss;
} __attribute__((packed)) regs_t;

typedef void (*isr_t)(regs_t*);

uint8_t enable_ints(void);
uint8_t disable_ints(void);
void panic(void);
void install_isr_and_irq(void);
void isr_handler(regs_t *r);
void irq_handler(regs_t *r);
void irq_regs_handler(int irq, isr_t handler);

void register_interrupt_handler(uint8_t n, isr_t handler);

#endif
