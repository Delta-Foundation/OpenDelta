#ifndef IDT_H
#define IDT_H

#pragma once

#include "types.h"

#define KERNEL_CS 0x08
#define IDT_SIZE  256

#define FLAG_SET(x, flag) x |= (flag)
#define FLAG_UNSET(x, flag) x &= ~(flag)

#define low_16(address) (uint16_t)((address) & 0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)

typedef enum {
    IDT_FLAG_GATE_TASK              = 0x5,
    IDT_FLAG_GATE_16BIT_INT         = 0x6,
    IDT_FLAG_GATE_16BIT_TRAP        = 0x7,
    IDT_FLAG_GATE_32BIT_INT         = 0xE,
    IDT_FLAG_GATE_32BIT_TRAP        = 0xF,

    IDT_FLAG_RING0                  = (0 << 5),
    IDT_FLAG_RING1                  = (1 << 5),
    IDT_FLAG_RING2                  = (2 << 5),
    IDT_FLAG_RING3                  = (3 << 5),

    IDT_FLAG_PRESENT                = 0x80,
} IDT_FLAGS;

typedef struct {
    uint16_t low_offset;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t high_offset;
} __attribute__((packed)) idt_gate_t;

typedef struct {
    uint16_t limit;
    idt_gate_t* base;
} __attribute__((packed)) idt_reg_t;

#define IDT_ENTRIES 256

void __attribute__((cdecl)) idt_load(idt_reg_t *idt_desc);

void idt_enable_gate(int interrupt);
void idt_disable_gate(int interrupt);
void set_idt_gate(int n, uint32_t base, uint16_t sel, uint8_t flags);
void idt_init(void);


#endif
