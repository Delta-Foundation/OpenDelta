#include "../timer.h"
#include "../isr.h"
#include "../ports.h"
#include "../screen.h"
#include "../../libc/include/string.h"
#include "../../libc/include/stdbase.h"

static uint32_t tick = 0;

static void timer_callback(regs_t * regs)
{
    (void)regs;
    tick++;
    prints("Tick: ", 0x07);

    char tick_ascii[256];
    int_to_ascii((int)tick, tick_ascii);
    kprint(tick_ascii, 0x08);
    prints("\n", 0x07);
}

void init_timer(uint32_t freq)
{
    if (freq == 0) {
        return;
    }

    register_interrupt_handler(IRQ0, timer_callback);

    uint32_t divisor = 1193180 / freq;
    if (divisor == 0) {
        divisor = 1;
    }
    if (divisor > 0xFFFF) {
        divisor = 0xFFFF;
    }

    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

    port_byte_out(0x43, 0x36);
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}
