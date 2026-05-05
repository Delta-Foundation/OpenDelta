#ifndef _TOOLS_DISK_H
#define _TOOLS_DISK_H

#include "../../lib/stdbase.h"
#include "../../lib/types.h"

#pragma once

typedef struct {
    FILE * file;
    uint8_t id;
    uint16_t cylindres;
    uint16_t sectors;
    uint16_t heads;
} DISK;

boolean disk_init(DISK *disk, const char *filename);
void disk_lba2chs(DISK* disk, uint32_t lba, uint16_t* cylinder_out, uint16_t* sector_out, uint16_t head_out);

#endif 
