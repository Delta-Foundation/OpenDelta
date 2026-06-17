#include "../lib/stdbase.h"
#include "../lib/system.h"

void play_sound(unsigned int nfreq)
{
    unsigned int div;
    unsigned char tmp;

    div = 1193180 / nfreq;
    outb((unsigned short)0x43, 0xb6);
    outb((unsigned short)0x42, (unsigned char) (div));
    outb((unsigned short)0x42, (unsigned char) (div >> 8));

    tmp = inb(0x61);
    if (tmp != (tmp | 3)) {
        outb(0x61, tmp | 3);
    }
}

void no_sound(void) {
    unsigned char tmp = inb((unsigned short)0x61) & 0xFC;
    outb((unsigned short)0x61, tmp);
}

void beep_boop(void) {
    play_sound(1000);
    wait(10);
    no_sound();
}
