#include "./headers/mbr.h"
#include "./headers/fat.h"
#include "../../mem/header/memory.h"

#define MBR_SIGNATURE_OFFSET 510
#define MBR_SIGNATURE_VALUE  0xAA55
#define MBR_ENTRY_OFFSET     446
#define MBR_ENTRY_COUNT      4
#define MBR_ENTRY_SIZE       16

static boolean mbr_is_valid(const uint8_t * sector) {
    uint16_t signature;

    memcpy(&signature, sector + MBR_SIGNATURE_OFFSET, sizeof(signature));
    return signature == MBR_SIGNATURE_VALUE;
}

static boolean mbr_is_fat32(uint8_t type) {
    return type == PARTITION_TYPE_FAT32_CHS
        || type == PARTITION_TYPE_FAT32_LBA;
}

boolean mbr_find_fat32(
    partition_t * part,
    DISK * disk
) {
    uint8_t sector[FAT_SECTOR_SIZE];

    part->disk = disk;
    part->part_offset = 0;
    part->part_size = 0;

    if (!disk_read_sects(disk, 0, 1, sector)) {
        return FALSE;
    }

    if (!mbr_is_valid(sector)) {
        return FALSE;
    }

    mbr_entry_t * entries = (mbr_entry_t *)(sector + MBR_ENTRY_OFFSET);

    for (int i = 0; i < MBR_ENTRY_COUNT; i++) {
        if (mbr_is_fat32(entries[i].partition_type)
            && entries[i].lba_start != 0
            && entries[i].size != 0) {
            part->part_offset = entries[i].lba_start;
            part->part_size = entries[i].size;
            return TRUE;
        }
    }

    return FALSE;
}

boolean part_read_sectors(
    partition_t * part,
    uint32_t lba,
    uint8_t sectors,
    void * data_out
) {
    return disk_read_sects(
        part->disk,
        part->part_offset + lba,
        sectors,
        data_out
    );
}

boolean part_write_sectors(
    partition_t * part,
    uint32_t lba,
    uint8_t sectors,
    const void * data_in
) {
    return disk_write_sects(
        part->disk,
        part->part_offset + lba,
        sectors,
        data_in
    );
}
