#include "../lib/idt.h"

static idt_gate_t idt[IDT_ENTRIES];
static idt_reg_t idt_reg;

void idt_enable_gate(int interrupt) {
    FLAG_SET(idt[interrupt].flags, IDT_FLAG_PRESENT);
}

void idt_disable_gate(int interrupt) {
    FLAG_UNSET(idt[interrupt].flags, IDT_FLAG_PRESENT);
}

void set_idt_gate(int n, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[n].low_offset = low_16(base);
    idt[n].sel = sel;
    idt[n].always0 = 0;
    idt[n].flags = flags;
    idt[n].high_offset = high_16(base);
}

void set_idt(void) {
    idt_reg.base = (uint32_t)&idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
    __asm__ __volatile__ ("lidtl (%0)" : : "r" (&idt_reg));
}
