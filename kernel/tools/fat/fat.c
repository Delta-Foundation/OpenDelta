#include "./headers/fat.h"
#include "./headers/mbr.h"
#include "../../mem/header/memory.h"

boot_sect_t boot_sect;
uint8_t* fat = NULL;
dir_entry_t* root_dir = NULL;
uint32_t root_dir_end;

static fat_data_t actual_data;
static fat_data_t* data;

static uint8_t *_fat = NULL;
static uint8_t fat_type;

static uint32_t data_sect_lba;
static uint32_t total_sects;
static uint32_t sectors_per_fat;

int fat_compare_lfn_blocks(const void* block_a, const void* block_b) {
    fat_lfn_blocks_t* a = (fat_lfn_blocks_t*)block_a;
    fat_lfn_blocks_t* b = (fat_lfn_blocks_t*)block_b;
    return ((int)a->order) - ((int)b->order);
}

boolean fat_read_boot_sect(partition_t* disk) {
    return part_read_sectors(disk, 0, 1, data->BS.boot_sect_bytes);
}

boolean fat_read_fat(partition_t* disk, unsigned int lba_index) {
    return part_read_sectors(disk, data->BS.boot_sect.reversed_sects + lba_index, FAT_CACHE_SIZE, data->fat_cache);
}

void fat_detect(partition_t* disk) 
{
    uint32_t data_cluster = (total_sects - data_sect_lba) / data->BS.boot_sect.sects_per_cluster;

    if (data_cluster < 0XFF5) {
        fat_type = 12;        
    }
    else if (data->BS.boot_sect.sects_per_fat != 0) {
        fat_type = 16;
    }
    else {
        fat_type = 32;
    }
}

uint32_t fat_cluster_to_lba(uint32_t cluster) {
    return data_sect_lba + (cluster - 2) * data->BS.boot_sect.sects_per_cluster;
}

boolean fat_init(partition_t* disk) 
{
    data = (fat_data_t*)MEMORY_FAT_ADDR;

    if (!fat_read_boot_sect(disk)) {
        prints("[FAT ERR]: [read boor sector failed!\r\n]", RED);
        return *FALSE;
    }

    data->fat_cache_pos = 0xFFFFFFFF;

    total_sects = data->BS.boot_sect.total_sects;
    
    if (total_sects == 0) {
        total_sects = data->BS.boot_sect.large_sects_count;
    }

    boolean *is_fat32 = FALSE;
    sectors_per_fat = data->BS.boot_sect.sects_per_fat;

    if (sectors_per_fat == 0) {
        is_fat32 = TRUE;
        sectors_per_fat = data->BS.boot_sect.EBR32.sectors_per_fat;
    }

    uint32_t root_dir_lba;
    uint32_t root_dir_size;

    if (is_fat32) {
        data_sect_lba = data->BS.boot_sect.reversed_sects + sectors_per_fat * data->BS.boot_sect.fat_count;
        root_dir_lba = fat_cluster_to_lba(data->BS.boot_sect.EBR32.root_dir_cluster);
        root_dir_size = 0;
    }
    else {
        root_dir_lba = data->BS.boot_sect.reversed_sects + sectors_per_fat * data->BS.boot_sect.fat_count;
        root_dir_size = sizeof(dir_entry_t) * data->BS.boot_sect.dir_entry_count;
        uint32_t root_dir_sectors = (root_dir_size + data->BS.boot_sect.bytes_per_sect - 1) / data->BS.boot_sect.bytes_per_sect;
        data_sect_lba = root_dir_lba + root_dir_sectors;
    }

    data->root_dir.fspublic.handle = ROOT_DIR_HANDLE;
    data->root_dir.fspublic.is_dir = *TRUE;
    data->root_dir.fspublic.position = 0;
    data->root_dir.fspublic.size = sizeof(dir_entry_t) * data->BS.boot_sect.dir_entry_count;
    data->root_dir.opened = *TRUE;
    data->root_dir.first_cluster = root_dir_lba;
    data->root_dir.current_cluster = root_dir_lba;
    data->root_dir.current_sector_in_cluster = 0;

    if (!part_read_sectors(disk, root_dir_lba, 1, data->root_dir.buffer)) {
        prints("[FAT ERR]: [read root directory failed!\r\n]", RED);
        return *FALSE;
    }

    fat_detect(disk);

    for (int i = 0; i < MAX_FILE_HANDLES; i++) {
        data->opened_files[i].opened = *FALSE;
    }
    data->lfn_count = 0;

    return *TRUE;
}

fat_file_t* fat_open_entry(partition_t* disk, dir_entry_t* entry)
{
    int handle = -1;
    for (int i = 0; i < MAX_FILE_HANDLES && handle < 0; i++) {
        if (!data->opened_files[i].opened) {
            handle = i;
        }
    }

    if (handle < 0) {
        prints("[FAT ERR]: [out of file handles!\r\n]", RED);
        return (fat_file_t *)FALSE;
    }

    fat_file_data_t* fd = &data->opened_files[handle];
    fd->fspublic.handle = handle;
    fd->fspublic.is_dir = (entry->attributes & FAT_ATTR_DIR) != 0;
    fd->fspublic.position = 0;
    fd->fspublic.size = entry->size;
    fd->first_cluster = entry->first_cluster_low + ((uint32_t)entry->first_cluster_high << 16);
    fd->current_cluster = fd->first_cluster;
    fd->current_sector_in_cluster = 0;

    if (!part_read_sectors(disk, fat_cluster_to_lba(fd->current_cluster), 1, fd->buffer)) {
        printf("[FAT ERR]: [open entry failed - read error cluster=%u lba=%u\n]", fd->current_cluster, fat_cluster_to_lba(fd->current_cluster));
        for (int i = 0; i < 11; i++) {
            printf("%c", entry->name[i]);
        }
        printf("\n");
        return (fat_file_t*)FALSE;
    }

    fd->opened = *TRUE;
    return &fd->fspublic;
}
