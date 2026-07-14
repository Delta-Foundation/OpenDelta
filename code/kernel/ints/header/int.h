#ifndef INT_H
#define INT_H

#include "../../lib/types.h"

boolean are_ints_enabled(void);

void intended_usage(void);
void irqrestore(unsigned long flags);
void cpuid(int code, uint32_t *a, uint32_t *d);

#endif
