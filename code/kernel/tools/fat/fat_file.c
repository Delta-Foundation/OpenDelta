#include "./headers/fat_internal.h"
#include "../../mem/header/memory.h"
#include "../../libc/include/string.h"

static uint32_t fat_min_u32(uint32_t a, uint32_t b) {
    return a < b ? a : b;
}

fat_file_t * fat_open(const char * path) {
    fat_state_t * state = fat_get_state();

    if (!state->mounted) {
        return NULL;
    }

    fat_dir_entry_t entry;
    if (!fat_lookup(path, &entry, NULL, NULL)) {
        return NULL;
    }

    fat_file_t * file = NULL;
    for (int i = 0; i < FAT_MAX_HANDLES; i++) {
        if (!state->handles[i].opened) {
            file = &state->handles[i];
            break;
        }
    }

    if (file == NULL) {
        return NULL;
    }

    file->is_dir = (entry.attributes & FAT_ATTR_DIR) != 0;
    file->opened = TRUE;
    file->position = 0;
    file->size = entry.size;
    file->first_cluster = fat_entry_cluster(&entry);
    file->current_cluster = file->first_cluster;
    file->sector_in_cluster = 0;

    return file;
}

uint32_t fat_read(
    fat_file_t * file,
    uint32_t count,
    void * data_out
) {
    fat_state_t * state = fat_get_state();
    uint8_t * out = (uint8_t *)data_out;
    uint32_t total = 0;

    if (file == NULL || !file->opened || file->is_dir) {
        return 0;
    }

    while (count > 0 && file->position < file->size) {
        if (file->current_cluster < 2
            || file->current_cluster >= FAT_CLUSTER_EOC_MIN) {
            break;
        }

        uint32_t lba = fat_cluster_lba(
            file->current_cluster,
            file->sector_in_cluster
        );

        if (lba == 0 || !fat_read_sector(lba, state->sector)) {
            break;
        }

        uint32_t offset = file->position % FAT_SECTOR_SIZE;
        uint32_t take = fat_min_u32(
            count,
            fat_min_u32(
                FAT_SECTOR_SIZE - offset,
                file->size - file->position
            )
        );

        memcpy(out, state->sector + offset, take);

        out += take;
        total += take;
        count -= take;
        file->position += take;

        if (offset + take == FAT_SECTOR_SIZE) {
            file->sector_in_cluster++;

            if (file->sector_in_cluster >= state->sectors_per_cluster) {
                file->current_cluster = fat_next_cluster(file->current_cluster);
                file->sector_in_cluster = 0;
            }
        }
    }

    return total;
}

void fat_close(fat_file_t * file) {
    if (file == NULL) {
        return;
    }

    file->opened = FALSE;
}
