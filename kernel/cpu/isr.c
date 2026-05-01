#include "../lib/string.h"
#include "../lib/ports.h"
#include "../lib/isr.h"
#include "../lib/idt.h"
#include "../lib/stdbase.h"
#include "../lib/pic.h"

isr_t interrupt_handlers[256];
isr_t irq_handlers[12];
static const pic_driver_t* g_driver = NULL;

/*---isr---*/
void isr_install(void)
{
	set_idt_gate(0, (uint64_t)isr0);
	set_idt_gate(1, (uint64_t)isr1);
	set_idt_gate(2, (uint64_t)isr2);
	set_idt_gate(3, (uint64_t)isr3);
	set_idt_gate(4, (uint64_t)isr4);
	set_idt_gate(5, (uint64_t)isr5);
	set_idt_gate(6, (uint64_t)isr6);
	set_idt_gate(7, (uint64_t)isr7);
	set_idt_gate(8, (uint64_t)isr8);
	set_idt_gate(9, (uint64_t)isr9);
	set_idt_gate(10, (uint64_t)isr10);
	set_idt_gate(11, (uint64_t)isr11);
	set_idt_gate(12, (uint64_t)isr12);
	set_idt_gate(13, (uint64_t)isr13);
	set_idt_gate(14, (uint64_t)isr14);
	set_idt_gate(15, (uint64_t)isr15);
	set_idt_gate(16, (uint64_t)isr16);
	set_idt_gate(17, (uint64_t)isr17);
	set_idt_gate(18, (uint64_t)isr18);
	set_idt_gate(19, (uint64_t)isr19);
	set_idt_gate(20, (uint64_t)isr20);
	set_idt_gate(21, (uint64_t)isr21);
	set_idt_gate(22, (uint64_t)isr22);
	set_idt_gate(23, (uint64_t)isr23);
	set_idt_gate(24, (uint64_t)isr24);
	set_idt_gate(25, (uint64_t)isr25);
	set_idt_gate(26, (uint64_t)isr26);
	set_idt_gate(27, (uint64_t)isr27);
	set_idt_gate(28, (uint64_t)isr28);
	set_idt_gate(29, (uint64_t)isr29);
	set_idt_gate(30, (uint64_t)isr30);
	set_idt_gate(31, (uint64_t)isr31);

	// Remap the PIC
	portByteOut(0x20, 0x11);
	portByteOut(0xA0, 0x11);
	portByteOut(0x21, 0x20);
	portByteOut(0xA1, 0x28);
	portByteOut(0x21, 0x04);
	portByteOut(0xA1, 0x02);
	portByteOut(0x21, 0x01);
	portByteOut(0xA1, 0x01);
	portByteOut(0x21, 0x0);
	portByteOut(0xA1, 0x0);

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

void isr_handler(regs_t r)
{
	prints("\n", 0x07);
	prints("[received interrupt]: ", 0x07);
	char s[3];
	int_to_ascii(r.int_no, s);
	prints(s, 0x08);
	prints("\n", 0x07);
	prints(exception_messages[r.int_no], 0x08);
	prints("\n", 0x07);
}

void register_interrupt_handler(uint8_t n, isr_t handler) {
	interrupt_handlers[n] = handler;
}

/*---irq---*/
void irq_handler(regs_t r)
{
	if(r.int_no >= 40) {
		portByteOut(0xA0, 0x20); // slave
	}

	portByteOut(0x20, 0x20); // master

	if(interrupt_handlers[r.int_no] != 0) {
		isr_t handler = interrupt_handlers[r.int_no];
		handler(r);
	}
}

void irq_init(void) {
	// Install the IRQs
	set_idt_gate(32, (uint64_t)irq0);
	set_idt_gate(33, (uint64_t)irq1);
	set_idt_gate(34, (uint64_t)irq2);
	set_idt_gate(35, (uint64_t)irq3);
	set_idt_gate(36, (uint64_t)irq4);
	set_idt_gate(37, (uint64_t)irq5);
	set_idt_gate(38, (uint64_t)irq6);
	set_idt_gate(39, (uint64_t)irq7);
	set_idt_gate(40, (uint64_t)irq8);
	set_idt_gate(41, (uint64_t)irq9);
	set_idt_gate(42, (uint64_t)irq10);
	set_idt_gate(43, (uint64_t)irq11);
	set_idt_gate(44, (uint64_t)irq12);
	set_idt_gate(45, (uint64_t)irq13);
	set_idt_gate(46, (uint64_t)irq14);
	set_idt_gate(47, (uint64_t)irq15);

	const pic_driver_t* drivers[] = {
		pic_driver(),
	};

	for (int i = 0; i < sizeof(*drivers); i++) {
		if (drivers[i]->probe()) {
			g_driver = drivers[i];
		}
	}

	if (g_driver == NULL) {
		prints("[WARN]: [no pic found!]", WHITE, MODULE);
		return;
	}

	printf(MODULE, "[INFO]: [found %s PIC.]", g_driver->name);
	g_driver->init(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET + 8, *FALSE);

	for (int i = 0; i < 16; i++) {
		register_interrupt_handler(PIC_REMAP_OFFSET + i, *irq_handlers);
	}

	enable_ints();
}

void irq_regs_handler(int irq, isr_t handler) {
	irq_handlers[irq] = handler;
}
