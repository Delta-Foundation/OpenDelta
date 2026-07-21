#include "gdt.h"

static gdt_entry gdt[3];
static gdt_ptr gp;

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

    gp.limit = sizeof(gdt) - 1;
    gp.ptr = (gdt_entry *)gdt;

    gdt_load(&gp, GDT_DATA_SEG, GDT_CODE_SEG);
}
