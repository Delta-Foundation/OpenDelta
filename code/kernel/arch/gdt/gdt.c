#include "gdt.h"

/*
static gdt_entry gdt[] = {
    {0, 0, 0, 0, 0, 0}, // Null entry
    {0xFFFF, 0, 0, 0x9A, 0xCF, 0}, // Code
    {0xFFFF, 0, 0, 0x92, 0xCF, 0}  // Data
};

static gdt_ptr gdt_descriptor = { sizeof(gdt) - 1, gdt };
*/

static gdt_entry gdt[3];
static gdt_ptr gp;

// void i386_gdt_init(void) {
//     gdt_load(&gdt_descriptor, (uint32_t)GDT_CODE_SEG, (uint32_t)GDT_DATA_SEG);
// }

__attribute__((noinline, naked))
static void gdt_flush(uint32_t gdt_ptr) {
    __asm__ __volatile__ ( "lgdt %0" : : "m"(gp) : "memory" );
    
    __asm__ __volatile__ (
        "ljmp $0x08, $1f \n\t"
        "1: \n\t"
        : : : "memory"
    );
    
    __asm__ __volatile__ (
        "movw $0x10, %%ax \n\t"
        "movw %%ax, %%ds \n\t"
        "movw %%ax, %%es \n\t"
        "movw %%ax, %%fs \n\t"
        "movw %%ax, %%gs \n\t"
        : : : "ax", "memory"
    );

    __asm__ __volatile__ (
        "movw $0x10, %%ax \n\t"
        "movw %%ax, %%ss \n\t"
        : : : "ax", "memory"
    );
}
__attribute__((noinline))
void gdt_init(void) {
    gdt[0] = (gdt_entry) { 0 };
    gdt[1] = (gdt_entry) {
        .limit_low = 0xFFFF,
        .base_low = 0x0000,
        .base_middle = 0x00,
        .access = 0x9A,
        .flags_limit_high = 0xCF,
        .base_high = 0x00 
    };

    gdt[2] = (gdt_entry) {
        .limit_low = 0xFFFF,
        .base_low = 0x0000,
        .base_middle = 0x00,
        .access = 0x92,
        .flags_limit_high = 0xCF,
        .base_high = 0x00 
    };

    gp.ptr = (gdt_entry *)gdt;
    gp.limit = (uint16_t)(sizeof(gdt) - 1);

    gdt_flush((uint32_t)(uintptr_t)&gp);
}
