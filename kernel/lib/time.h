#ifndef TIME_H
#define TIME_H

#include "./types.h"

struct Timeval {
    uint32_t tv_sec;
    uint32_t tv_usec;
};

uint32_t epoch_now(void);

#endif
