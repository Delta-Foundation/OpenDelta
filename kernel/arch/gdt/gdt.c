#include "gdt.h"

GDTEntry g_gdt[] = {
    {0, 0, 0, 0, 0, 0}, // Null entry
    {0xFFFF, 0, 0, 0x9A, 0xCF, 0}, // Code
    {0xFFFF, 0, 0, 0x92, 0xCF, 0}  // Data
};

GDTDescriptor g_gdt_descriptor = { sizeof(g_gdt) - 1, g_gdt };

void i386_GDT_init(void) {
    i386_GDT_load(&g_gdt_descriptor, (uint32_t)i386_GDT_CODE_SEG, (uint32_t)i386_GDT_DATA_SEG);
}
