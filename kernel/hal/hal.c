#include "../lib/idt.h"
#include "../lib/isr.h"
#include "../lib/pic.h"
#include "../arch/gdt/gdt.h"

void hal_init(void) {
    gdt_init();
    idt_init();
    mask_all();
    pic_remap();
    install_isr_and_irq();
}
