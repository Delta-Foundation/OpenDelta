#include "gdt.h"

GDTEntry g_gdt[] = {
    GDT_ENTRY(0, 0, 0, 0),

    GDT_ENTRY(0,
              0xFFFFF,
              GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE,
              GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K),

    GDT_ENTRY(0,
              0xFFFFF,
              GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITEABLE,
              GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K),

};

GDTDescriptor g_gdt_descriptor = { sizeof(g_gdt) - 1, g_gdt };

void i386_GDT_init(void) {
    i386_GDT_load(&g_gdt_descriptor, i386_GDT_CODE_SEG, i386_GDT_DATA_SEG);
}
