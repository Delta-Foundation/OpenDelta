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

uint32_t fat_next_cluster(partition_t* disk, uint32_t current_cluster) 
{
    uint32_t fat_index;
    
    if (fat_type == 12) {
        fat_index = current_cluster * 3 / 2;
    }
    else if (fat_type == 16) {
        fat_index = current_cluster * 2;
    }
    else {
        fat_index = current_cluster * 4;
    }

    uint32_t fat_index_sector = fat_index / SECTOR_SIZE;
    if (fat_index_sector < data->fat_cache_pos 
        || fat_index_sector >= data->fat_cache_pos + FAT_CACHE_SIZE) {
        fat_read_fat(disk, fat_index_sector);
        data->fat_cache_pos = fat_index_sector;
    }

    fat_index -= (data->fat_cache_pos * SECTOR_SIZE);

    uint32_t next_cluster;
    if (fat_type == 12) {
        
        if (current_cluster % 2 == 0) {
            next_cluster = (*(uint16_t*)(data->fat_cache + fat_index)) & 0x0FFF;
        }
        else {
            next_cluster = (*(uint16_t*)(data->fat_cache + fat_index)) >> 4;
        }

        if (next_cluster >= 0xFF8) {
            next_cluster |= 0xFFFFF000;
        }
    }

    else if (fat_type == 16)  {
        next_cluster = *(uint16_t*)(data->fat_cache + fat_index);
        if (next_cluster >= 0xFFF8) {
            next_cluster = 0xFFFF0000;
        }
    }
    
    else {
        next_cluster = *(uint16_t*)(data->fat_cache + fat_index);
    }

    return next_cluster;
}

uint32_t fat_read(partition_t* disk, fat_file_t* file, uint32_t byte_count, void* data_out)
{
    fat_file_data_t* fd = (file->handle == ROOT_DIR_HANDLE) 
        ? &data->root_dir
        : &data->opened_files[file->handle];

    uint8_t* u8DataOut = (uint8_t*)data_out;

    if (!fd->fspublic.is_dir || (fd->fspublic.is_dir && fd->fspublic.size != 0)) {
        byte_count = min(byte_count, fd->fspublic.size - fd->fspublic.position);
    }

    while (byte_count > 0) {
        uint32_t left_in_buffer = SECTOR_SIZE - (fd->fspublic.position % SECTOR_SIZE);
        uint32_t take = min(byte_count, left_in_buffer);

        memcpy(u8DataOut, fd->buffer + fd->fspublic.position % SECTOR_SIZE, take);
        u8DataOut += take;
        fd->fspublic.position += take;
        byte_count -= take;

        if (left_in_buffer == take) {
            if (fd->fspublic.handle == ROOT_DIR_HANDLE) {
                ++fd->current_cluster;

                if (!part_read_sectors(disk, fd->current_cluster, 1, fd->buffer)) 
                {
                    prints("[FATAL FAT ERROR]: [FAT: read error!]\r\n", RED);
                    break;
                }
            }
            else {
                if (++fd->current_sector_in_cluster >= data->BS.boot_sect.sects_per_cluster) {
                    fd->current_sector_in_cluster = 0;
                    fd->current_cluster = fat_next_cluster(disk, fd->current_cluster);
                }

                if (fd->current_cluster >= 0xFFFFFFF8) {
                    fd->fspublic.size = fd->fspublic.position;
                    break;
                }

                if (!part_read_sectors(disk, fat_cluster_to_lba(fd->current_cluster) + fd->current_sector_in_cluster, 1, fd->buffer)) 
                {
                    prints("[FATAL FAT ERROR]: [read error!]\r\n", RED);
                    break;
                }
            }
        }
    }

    return u8DataOut - (uint8_t*)data_out;
}

boolean fat_read_entry(partition_t* disk, fat_file_t* file, dir_entry_t* dir_entry) {
    return fat_read(disk, file, sizeof(dir_entry_t), dir_entry) == sizeof(dir_entry_t);
}

void fat_close(fat_file_t* file) {
    if (file->handle == ROOT_DIR_HANDLE) {
        file->position = 0;
        data->root_dir.current_cluster = data->root_dir.first_cluster;
    }
    else {
        data->opened_files[file->handle].opened = (int)FALSE;
    }
}

void fat_get_short_name(const char* name, char short_name[12])
{
    memset(short_name, ' ', 12);
    short_name[11] = '\0';

    const char* ext = strchar(name, '.');
    if (ext == NULL) {
        ext = name + 11;
    }

    for (int i = 0; i < 8 && name[i] && name + i < ext; i++)
        short_name[i] = toupper(name[i]);

    if (ext != name + 11) {
        for (int i = 0; i < 3 && ext[i + 1]; i++) {
            short_name[i + 8] = toupper(ext[i + 1]);
        }
    }
}

boolean fat_find_file(partition_t* disk, fat_file_t* file, const char* name, dir_entry_t* entry_out)
{
    char short_name[12];
    char long_name[256];
    dir_entry_t entry;

    fat_get_short_name(name, short_name);

    while (fat_read_entry(disk, file, &entry)) {
        if (entry.attributes == FAT_ATTR_LFN) {
            fat_long_file_entry_t* lfn = (fat_long_file_entry_t*)&entry;

            int idx = data->lfn_count;
            data->lfn_blocks[idx].order = lfn->order & (FAT_LFN_LAST - 1);
            memcpy(data->lfn_blocks[idx].chars, lfn->chars1, sizeof(lfn->chars1));
            memcpy(data->lfn_blocks[idx].chars + 5, lfn->chars2, sizeof(lfn->chars2));
            memcpy(data->lfn_blocks[idx].chars + 11, lfn->chars1, sizeof(lfn->chars3));

            if ((lfn->order & FAT_LFN_LAST) != 0) {
                qsort(data->lfn_blocks, data->lfn_count, sizeof(fat_lfn_blocks_t), fat_compare_lfn_blocks);
                char* namePos = long_name;
                for (int i = 0; i < data->lfn_count; i++) {
                    int16_t* chars = data->lfn_blocks[i].chars;
                    int16_t* chars_limit = chars + 13;

                    while (chars < chars_limit && *chars != 0) {
                        int codepoint;
                        chars = (int16_t*)utf16_to_codepoint((_wchar_t *)chars, &codepoint);
                        namePos = codepoint_to_utf8(codepoint, namePos);
                    }
                }
                *namePos = 0;
                printf("LFN: %s\n", long_name);
            }
        }

        if (memcmp(short_name, entry.name, 11) == 0) {
            *entry_out = entry;
            return *TRUE;
        }
    }
    
    return *FALSE;
}

fat_file_t* fat_open(partition_t* disk, const char* path)
{
    char name[MAX_PATH_SIZE];

    if (path[0] == '/') {
        path++;
    }

    fat_file_t* current = &data->root_dir.fspublic;

    while (*path) {
        boolean is_last = *FALSE;
        const char* delim = strchar(path, '/');
        
        if (delim != NULL) {
            memcpy(name, path, delim - path);
            name[delim - path] = '\0';
            path = delim + 1;
        }
        else {
            unsigned len = strlen(path);
            memcpy(name, path, len);
            name[len + 1] = '\0';
            path += len;
            is_last = *TRUE;
        }

        dir_entry_t entry;
        if (fat_find_file(disk, current, name, &entry))
        {
            fat_close(current);

            if (!is_last && entry.attributes & FAT_ATTR_DIR == 0) {
                printf("[FAT]: [%s not a directory]\r\n", name);
                return NULL;
            }

            // open new directory entry
            current = fat_open_entry(disk, &entry);
        }
        else {
            fat_close(current);

            printf("[FAT]: [%s not found]\r\n", name);
            return NULL;
        }
    }

    return current;
}
