#ifndef _TOOLS_MBR_H
#define _TOOLS_MBR_H

#pragma once

#include "./disk.h"
#include "../../../libc/include/types.h"

#define PARTITION_TYPE_FAT32_CHS 0x0B
#define PARTITION_TYPE_FAT32_LBA 0x0C

#define OPEN_DELTA_FAT32_LBA     2048

typedef struct {
    DISK * disk;
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

boolean mbr_find_fat32(
    partition_t * part,
    DISK * disk
);

boolean part_read_sectors(
    partition_t * part,
    uint32_t lba,
    uint8_t sectors,
    void * data_out
);

boolean part_write_sectors(
    partition_t * part,
    uint32_t lba,
    uint8_t sectors,
    const void * data_in
);

#endif
