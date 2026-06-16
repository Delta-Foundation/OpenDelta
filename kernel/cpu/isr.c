#include "../lib/string.h"
#include "../lib/ports.h"
#include "../lib/isr.h"
#include "../lib/idt.h"
#include "../lib/stdbase.h"
#include "../lib/pic.h"

isr_t interrupt_handlers[IDT_SIZE] = { 0 };
isr_t irq_handlers[16];
static const pic_driver_t* g_driver = NULL;

void install_isr_and_irq(void)
{
    // Install the ISR
	set_idt_gate(0,  (uint32_t)isr0,  0x08, 0x8E);
	set_idt_gate(1,  (uint32_t)isr1,  0x08, 0x8E);
	set_idt_gate(2,  (uint32_t)isr2,  0x08, 0x8E);
	set_idt_gate(3,  (uint32_t)isr3,  0x08, 0x8E);
	set_idt_gate(4,  (uint32_t)isr4,  0x08, 0x8E);
	set_idt_gate(5,  (uint32_t)isr5,  0x08, 0x8E);
	set_idt_gate(6,  (uint32_t)isr6,  0x08, 0x8E);
	set_idt_gate(7,  (uint32_t)isr7,  0x08, 0x8E);
	set_idt_gate(8,  (uint32_t)isr8,  0x08, 0x8E);
	set_idt_gate(9,  (uint32_t)isr9,  0x08, 0x8E);
	set_idt_gate(10, (uint32_t)isr10, 0x08, 0x8E);
	set_idt_gate(11, (uint32_t)isr11, 0x08, 0x8E);
	set_idt_gate(12, (uint32_t)isr12, 0x08, 0x8E);
	set_idt_gate(13, (uint32_t)isr13, 0x08, 0x8E);
	set_idt_gate(14, (uint32_t)isr14, 0x08, 0x8E);
	set_idt_gate(15, (uint32_t)isr15, 0x08, 0x8E);
	set_idt_gate(16, (uint32_t)isr16, 0x08, 0x8E);
	set_idt_gate(17, (uint32_t)isr17, 0x08, 0x8E);
	set_idt_gate(18, (uint32_t)isr18, 0x08, 0x8E);
	set_idt_gate(19, (uint32_t)isr19, 0x08, 0x8E);
	set_idt_gate(20, (uint32_t)isr20, 0x08, 0x8E);
	set_idt_gate(21, (uint32_t)isr21, 0x08, 0x8E);
	set_idt_gate(22, (uint32_t)isr22, 0x08, 0x8E);
	set_idt_gate(23, (uint32_t)isr23, 0x08, 0x8E);
	set_idt_gate(24, (uint32_t)isr24, 0x08, 0x8E);
	set_idt_gate(25, (uint32_t)isr25, 0x08, 0x8E);
	set_idt_gate(26, (uint32_t)isr26, 0x08, 0x8E);
	set_idt_gate(27, (uint32_t)isr27, 0x08, 0x8E);
	set_idt_gate(28, (uint32_t)isr28, 0x08, 0x8E);
	set_idt_gate(29, (uint32_t)isr29, 0x08, 0x8E);
	set_idt_gate(30, (uint32_t)isr30, 0x08, 0x8E);
	set_idt_gate(31, (uint32_t)isr31, 0x08, 0x8E);

    // Remaping the PIC
    pic_remap();

    // Install the IRQ
    set_idt_gate(32, (uint32_t)irq0,  0x08, 0x8E);
    set_idt_gate(33, (uint32_t)irq1,  0x08, 0x8E);
    set_idt_gate(34, (uint32_t)irq2,  0x08, 0x8E);
    set_idt_gate(35, (uint32_t)irq3,  0x08, 0x8E);
    set_idt_gate(36, (uint32_t)irq4,  0x08, 0x8E);
    set_idt_gate(37, (uint32_t)irq5,  0x08, 0x8E);
    set_idt_gate(38, (uint32_t)irq6,  0x08, 0x8E);
    set_idt_gate(39, (uint32_t)irq7,  0x08, 0x8E);
    set_idt_gate(40, (uint32_t)irq8,  0x08, 0x8E);
    set_idt_gate(41, (uint32_t)irq9,  0x08, 0x8E);
    set_idt_gate(42, (uint32_t)irq10, 0x08, 0x8E);
    set_idt_gate(43, (uint32_t)irq11, 0x08, 0x8E);
    set_idt_gate(44, (uint32_t)irq12, 0x08, 0x8E);
    set_idt_gate(45, (uint32_t)irq13, 0x08, 0x8E);
    set_idt_gate(46, (uint32_t)irq14, 0x08, 0x8E);
    set_idt_gate(47, (uint32_t)irq15, 0x08, 0x8E);

    set_idt();
}

char *exception_messages[] =
{
	    "Division By Zero",
    	"Debug",
    	"Non Maskable Interrupt",
    	"Breakpoint",
    	"Into Detected Overflow",
    	"Out of Bounds",
    	"Invalid Opcode",
    	"No Coprocessor",

    	"Double Fault",
    	"Coprocessor Segment Overrun",
    	"Bad TSS",
    	"Segment Not Present",
    	"Stack Fault",
    	"General Protection Fault",
    	"Page Fault",
    	"Unknown Interrupt",

    	"Coprocessor Fault",
    	"Alignment Check",
    	"Machine Check",
    	"Reserved",
    	"Reserved",
    	"Reserved",
    	"Reserved",
    	"Reserved",

    	"Reserved",
    	"Reserved",
    	"Reserved",
    	"Reserved",
    	"Reserved",
    	"Reserved",
    	"Reserved",
    	"Reserved"
};

void isr_handler(regs_t *r)
{
	if (r->int_no < 32) {
        if (interrupt_handlers[r->int_no]) {
            interrupt_handlers[r->int_no](r);
        }
        else {
            prints("[ERR]: [EXCEPTION]: ", RED);
            prints(exception_messages[r->int_no], RED);
            prints("\n", RED);
            prints("[ERR]: [System Handled!]\n", RED);

            __asm__ volatile ( "cli" );
            for (;;) {
                __asm__ volatile ( "hlt" );
            }
        }
    }
    else if (interrupt_handlers[r->int_no]) {
        interrupt_handlers[r->int_no](r);
    }
}

/*---irq---*/
void irq_handler(regs_t *r) {
    if (r->int_no >= 40) {
        outb(0xA0, 0x20);
    }

    outb(0x20, 0x20);

    if (interrupt_handlers[r->int_no]) {
        interrupt_handlers[r->int_no](r);
    }
}

void register_interrupt_handler(uint8_t n, isr_t handler) {
	interrupt_handlers[n] = handler;
}

void irq_regs_handler(int irq, isr_t handler) {
	irq_handlers[irq] = handler;
	register_interrupt_handler(PIC_REMAP_OFFSET + irq, handler);

	if (g_driver) {
		g_driver->unmask(irq);
	}
}
