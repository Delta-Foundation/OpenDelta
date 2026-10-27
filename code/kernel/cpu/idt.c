#include "../kernel-lib/idt.h"

static idt_gate_t idt[IDT_ENTRIES];
static idt_reg_t idt_reg;
static idt_reg_t idt_descriptor = { sizeof(idt) - 1, (idt_gate_t *)idt };

void idt_enable_gate(int interrupt) {
    FLAG_SET(idt[interrupt].flags, IDT_FLAG_PRESENT);
}

void idt_disable_gate(int interrupt) {
    FLAG_UNSET(idt[interrupt].flags, IDT_FLAG_PRESENT);
}

void set_idt_gate(
    int n, 
    uint64_t base, 
    uint16_t sel, 
    uint8_t flags
) {
    idt[n].low_offset  = (uint16_t)(base & 0xFFFF);
    idt[n].sel         = sel;
    idt[n].ist         = 0;
    idt[n].flags       = flags;
    idt[n].mid_offset  = (uint16_t)((base >> 16) & 0xFFFF);
    idt[n].high_offset = (uint32_t)((base >> 32) & 0xFFFFFFFF);
    idt[n].reserved    = 0;
}

/*
void set_idt(void) {
    idt_reg.base = (idt_gate_t *)idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
    __asm__ __volatile__ ("lidtl (%0)" : : "r" (&idt_reg));
}
*/

void idt_init(void) {
    idt_load(&idt_descriptor);
}
