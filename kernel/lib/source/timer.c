#include "../timer.h"
#include "../isr.h"
#include "../ports.h"
#include "../screen.h"
#include "../string.h"
#include  "../stdbase.h"

uint32_t tick = 0;

static void timer_callback(regs_t regs) 
{
    tick++;
    prints("Tick: ", 0x07);

    char tick_ascii[256];
    int_to_ascii(tick, tick_ascii);
    kprint(tick_ascii, 0x08);
    prints("\n", 0x07);
}

void init_timer(uint32_t freq)
{
    register_interrupt_handler(IRQ0, timer_callback);
    
    uint32_t divisior =  1193180 / freq;
    uint8_t low = (uint8_t)(divisior & 0x0F);
    uint8_t high = (uint8_t)((divisior >> 8) & 0x0F);

    portByteOut(0x43, 0x36);
    portByteOut(0x40, low);
    portByteOut(0x40, high);
}
