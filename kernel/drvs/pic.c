#include "../lib/pic.h"
#include "../lib/pic.h"
#include "../lib/stdbase.h"

void iowait(void) {
    outb(UNUSED_PORT, 0);
}

void set_mask(uint16_t new_mask) {
    g_pic_mask = new_mask;
    outb(PIC1_DATA_PORT, g_pic_mask & 0xFF);
    iowait();
    outb(PIC2_DATA_PORT, g_pic_mask >> 8);
    iowait();
}

uint16_t get_mask(void) {
    return inb(PIC1_COM_PORT) | (inb(PIC2_COM_PORT) << 8);
}

void pic_configure(uint8_t offset1, uint8_t offset2, boolean auto_eoi) 
{
    set_mask(0xFFFF);

    outb(PIC1_COM_PORT, PIC_ICW1_ICW4 | PIC_ICW1_INIT);
    iowait();
    outb(PIC2_COM_PORT, PIC_ICW1_ICW4 | PIC_ICW1_INIT);
    iowait();

    outb(PIC1_DATA_PORT, offset1);
    iowait();
    outb(PIC2_DATA_PORT, offset2);
    iowait();

    outb(PIC1_DATA_PORT, 0x4);
    iowait();
    outb(PIC2_DATA_PORT, 0x2);
    iowait();

    uint8_t icw4 = PIC_ICW4_8086;
    if (auto_eoi) {
        icw4 |= PIC_ICW4_AUTO_EOI;
    }

    outb(PIC1_DATA_PORT, icw4);
    iowait();
    outb(PIC2_DATA_PORT, icw4);

    set_mask(0xFFFF);
}

void send_end_of_int(int irq) {
    if (irq >= 8) {
        outb(PIC2_COM_PORT, PIC_CMD_END_OF_INT);
    }
    outb(PIC1_COM_PORT, PIC_CMD_END_OF_INT);
}

void pic_disable(void) {
    set_mask(0xFFFF);
}

void mask(int irq) {
    set_mask(g_pic_mask | (1 << irq));
}

void unmask(int irq) {
    set_mask(g_pic_mask & ~(1 << irq));
}

uint16_t read_irq_request_reg(void) {
    outb(PIC1_COM_PORT, PIC_CMD_READ_IRR);
    outb(PIC2_COM_PORT, PIC_CMD_READ_IRR);
    return ((uint16_t)inb(PIC2_COM_PORT)) | (((uint16_t)inb(PIC1_COM_PORT)) << 8);
}

boolean probe(void) {
    pic_disable();
    set_mask(0x1337);
    return get_mask() == 0x1337;
}

static const pic_driver_t g_pic_driver = {
    .name = "8259 PIC",
    .probe = &probe,
    .init = &pic_configure,
    .disable = &pic_disable,
    .send_end_of_int = &send_end_of_int,
    .mask = &mask,
    .unmask = &unmask,
}; 

const pic_driver_t* pic_driver(void) {
    return &g_pic_driver;
}
