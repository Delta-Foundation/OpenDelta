#ifndef IDT_H
#define IDT_H

#pragma once

#include "types.h"

#define KERNEL_CS 0x08
#define IDT_SIZE  256

#define FLAG_SET(x, flag) x |= (flag)
#define FLAG_UNSET(x, flag) x &= ~(flag)

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
    uint32_t base;
} __attribute__((packed)) idt_reg_t;

typedef struct {
    unsigned short base_low;
    unsigned short reversed;
    unsigned char flags;
    unsigned base_high; 
    unsigned short int offset_lowerbits;
    unsigned short selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short int offset_higherbits;
} __attribute__((packed)) IDTEntry;

typedef struct {    
    uint16_t limit;
    IDTEntry *ptr;
} __attribute__((packed)) IDTDescriptor;

#define IDT_ENTRIES 256

void idt_load(IDTDescriptor *idt_desc);

void idt_enable_gate(int interrupt);
void idt_disable_gate(int interrupt);
void set_idt_gate(int n, uint32_t handler);
void set_idt(void);

#endif
