#ifndef _TOOLS_DISK_H
#define _TOOLS_DISK_H

#pragma once

#include "../../../libc/include/types.h"

typedef struct {
    uint8_t id;
} DISK;

boolean disk_init(
    DISK * disk,
    uint8_t drive
);

boolean disk_read_sects(
    DISK * disk,
    uint32_t lba,
    uint8_t sectors,
    void * data_out
);

boolean disk_write_sects(
    DISK * disk,
    uint32_t lba,
    uint8_t sectors,
    const void * data_in
);

#endif
