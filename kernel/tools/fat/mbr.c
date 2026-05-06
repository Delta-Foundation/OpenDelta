#include "./mbr.h"
#include "../../mem/header/memory.h"
#include "../../lib/stdbase.h"

void mbr_detect_part(partition_t* part, DISK* disk, void* part_entry) 
{
    part->disk = disk;
    if (disk->id < 0x80) {
        part->part_size = (uint32_t)(disk->cylindres) 
            * (uint32_t)(disk->heads)
            * (uint32_t)(disk->sectors);
    }

    else {
        mbr_entry_t* entry = (mbr_entry_t*)segoffset_to_linear(part_entry);
        part->part_offset = entry->lba_start;
        part->part_size = entry->size;
    }
}

boolean part_read_sectors(partition_t* part, uint32_t lba, uint8_t sectors, void* lower_data_out) {
    return disk_read_sects(part->disk, lba + part->part_offset, sectors, lower_data_out);
}
