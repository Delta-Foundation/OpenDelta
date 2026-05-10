#ifndef _TOOLS_MBR_H
#define _TOOLS_MBR_H

#pragma once

#include "disk.h"
#include "../../../lib/types.h"

typedef struct {
    DISK* disk;
    uint32_t part_offset;
    uint32_t part_size;
} partition_t;

typedef struct {
    uint8_t attributes;
    uint8_t chs_start[3];
    uint8_t partition_type;
    uint8_t chs_end[3];
    uint32_t lba_start;
    uint32_t size;
} __attribute__((packed)) mbr_entry_t;

void mbr_detect_part(partition_t* part, DISK* disk, void* part_entry);
boolean part_read_sectors(partition_t* part, uint32_t lba, uint8_t sectors, void* lower_data_out);

#endif 
