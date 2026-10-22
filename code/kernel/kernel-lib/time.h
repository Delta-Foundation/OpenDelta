#ifndef TIME_H
#define TIME_H

#include "../libc/include/types.h"

struct timeval {
    uint32_t tv_sec;
    uint32_t tv_usec;
};

uint32_t epoch_now(void);

#endif
