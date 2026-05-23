#include "./header/int.h"
#include "../lib/types.h"

boolean are_ints_enabled(void) {
    unsigned long flags;
    __asm__ volatile ("pushf\n\t"
                        "pop %0"
                        : "=g"(flags));
    return flags & (1 << 9);
}

static inline unsigned long save_irqdisable(void) {
    unsigned long flags;
    __asm__ volatile ("pushf\n\tcli\n\tpop %0" : "=r"(flags) : : "memory");
    return 0;
}

void irqrestore(unsigned long flags) {
    __asm__ ("push %0\n\tpopf" : : "rm"(flags) : "memory","cc");
}

void intended_usage(void) {
    unsigned long f = save_irqdisable();
    irqrestore(f);
}

void cpuid(int code, uint32_t *a, uint32_t *b) {
    __asm__ volatile ("cpuid" : "=a"(*a), "=d"(*b) : "0"(code) : "ebx", "ecx");
}

static inline unsigned long read_cr0(void) {
    unsigned long val;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(val));
    return val;
}
