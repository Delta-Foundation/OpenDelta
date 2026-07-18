#include "gdt.h"

static gdt_entry gdt[3];
static gdt_ptr gp;

__attribute__((noinline, naked))
static void gdt_reload_segments(void) {
    __asm__ __volatile__ (
        "ljmp $0x08, $1f \n\t"
        "1: \n\t"
        "movw $0x10, %ax \n\t"
        "movw %ax, %ds \n\t"
        "movw %ax, %es \n\t"
        "movw %ax, %fs \n\t"
        "movw %ax, %gs \n\t"
        "movw %ax, %ss \n\t"
        "ret \n\t"
    );
}

__attribute__((noinline))
static void gdt_flush(void) {
    __asm__ __volatile__ ( "lgdt %0" : : "m" (gp) : "memory" );
    gdt_reload_segments();
}

__attribute__((noinline))
void gdt_init(void) {
    /* Null descriptor */
    gdt[0] = (gdt_entry) { 0 };
    
    /* Code segment */
    gdt[1] = (gdt_entry) {
        .limit_low = 0xFFFF,
        .base_low = 0x0000,
        .base_middle = 0x00,
        .access = 0x9A,
        .flags_limit_high = 0xCF,
        .base_high = 0x00 
    };

    /* Data segment */
    gdt[2] = (gdt_entry) {
        .limit_low = 0xFFFF,
        .base_low = 0x0000,
        .base_middle = 0x00,
        .access = 0x92,
        .flags_limit_high = 0xCF,
        .base_high = 0x00 
    };

    gp.limit = (uint16_t)(sizeof(gdt) - 1);
    gp.ptr = (gdt_entry *)gdt;

    gdt_flush();
}
