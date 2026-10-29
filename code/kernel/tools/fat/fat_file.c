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

boolean fat_write_file(
    const char * path,
    const void * data,
    uint32_t size
) {
    fat_state_t * state = fat_get_state();

    if (!state->mounted) {
        return FALSE;
    }

    if (path == NULL || (data == NULL && size > 0)) {
        return FALSE;
    }

    fat_dir_entry_t entry;
    uint32_t entry_lba;
    uint32_t entry_offset;

    if (!fat_lookup(path, &entry, &entry_lba, &entry_offset)) {
        return FALSE;
    }

    if (entry.attributes & FAT_ATTR_DIR) {
        return FALSE;
    }

    uint32_t old_first = fat_entry_cluster(&entry);
    if (old_first >= 2 && old_first < FAT_CLUSTER_EOC_MIN) {
        if (!fat_free_chain(old_first)) {
            return FALSE;
        }
    }

    uint32_t new_first = 0;

    if (size > 0) {
        const uint8_t * src = (const uint8_t *)data;
        uint32_t bytes_per_cluster = state->bytes_per_cluster;
        uint32_t sectors_per_cluster = state->sectors_per_cluster;
        uint32_t bytes_per_sector = state->bpb.bytes_per_sector;

        uint32_t remaining = size;
        uint32_t prev_cluster = 0;

        while (remaining > 0) {
            uint32_t cluster = fat_alloc_cluster();
            if (cluster == 0) {
                if (new_first != 0) {
                    fat_free_chain(new_first);
                }
                return FALSE;
            }

            if (new_first == 0) {
                new_first = cluster;
            }

            if (prev_cluster != 0) {
                if (!fat_set_entry(prev_cluster, cluster)) {
                    fat_free_chain(new_first);
                    return FALSE;
                }
            }

            if (!fat_set_entry(cluster, FAT_CLUSTER_EOC_MIN | 0xF)) {
                fat_free_chain(new_first);
                return FALSE;
            }

            uint32_t to_write_cluster =
                (remaining < bytes_per_cluster) ? remaining : bytes_per_cluster;

            for (uint32_t s = 0; s < sectors_per_cluster; s++) {
                uint8_t sector_buf[FAT_SECTOR_SIZE];

                for (uint32_t z = 0; z < bytes_per_sector; z++) {
                    sector_buf[z] = 0;
                }

                uint32_t sector_offset = s * bytes_per_sector;
                if (sector_offset >= to_write_cluster) {
                    uint32_t lba = fat_cluster_lba(cluster, s);
                    if (!fat_write_sector(lba, sector_buf)) {
                        fat_free_chain(new_first);
                        return FALSE;
                    }
                    continue;
                }

                uint32_t bytes_in_sector = to_write_cluster - sector_offset;
                if (bytes_in_sector > bytes_per_sector) {
                    bytes_in_sector = bytes_per_sector;
                }

                for (uint32_t b = 0; b < bytes_in_sector; b++) {
                    sector_buf[b] = src[b];
                }

                uint32_t lba = fat_cluster_lba(cluster, s);
                if (!fat_write_sector(lba, sector_buf)) {
                    fat_free_chain(new_first);
                    return FALSE;
                }

                src += bytes_in_sector;
            }

            remaining -= to_write_cluster;
            prev_cluster = cluster;
        }
    }

    uint8_t sector_buf[FAT_SECTOR_SIZE];
    if (!fat_read_sector(entry_lba, sector_buf)) {
        if (new_first != 0) {
            fat_free_chain(new_first);
        }
        return FALSE;
    }

    fat_dir_entry_t * disk_entry =
        (fat_dir_entry_t *)(sector_buf + entry_offset);

    disk_entry->first_cluster_low  = (uint16_t)(new_first & 0xFFFF);
    disk_entry->first_cluster_high = (uint16_t)((new_first >> 16) & 0xFFFF);
    disk_entry->size = size;

    if (!fat_write_sector(entry_lba, sector_buf)) {
        if (new_first != 0) {
            fat_free_chain(new_first);
        }
        return FALSE;
    }

    return TRUE;
}

void fat_close(fat_file_t * file) {
    if (file == NULL) {
        return;
    }

    file->opened = FALSE;
}
