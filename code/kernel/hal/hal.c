#include "../kernel-lib/idt.h"
#include "../kernel-lib/isr.h"
#include "../kernel-lib/pic.h"
#include "../arch/gdt/gdt.h"

void hal_init(void) {
    gdt_init();
    idt_init();
    mask_all();
    pic_remap();
    install_isr_and_irq();
}
