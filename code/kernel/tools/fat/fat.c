#include "./headers/fat_internal.h"
#include "../../mem/header/memory.h"
#include "../../libc/include/string.h"

#define FAT_BOOT_SIGNATURE 0xAA55

static fat_state_t * state = (fat_state_t *)MEMORY_FAT_ADDR;

fat_state_t * fat_get_state(void) {
    return state;
}

boolean fat_read_sector(uint32_t lba, void * data_out) {
    return part_read_sectors(&state->part, lba, 1, data_out);
}

boolean fat_write_sector(uint32_t lba, const void * data_in) {
    return part_write_sectors(&state->part, lba, 1, (void *)data_in);
}

uint32_t fat_cluster_lba(uint32_t cluster, uint32_t sector) {
    if (cluster < 2 || cluster >= state->cluster_count + 2) {
        return 0;
    }

    return state->data_lba
        + (cluster - 2) * state->sectors_per_cluster
        + sector;
}

uint32_t fat_get_entry(uint32_t cluster) {
    if (cluster < 2 || cluster >= state->cluster_count + 2) {
        return 0;
    }

    uint32_t offset = cluster * 4;
    uint32_t lba = state->fat_lba + offset / FAT_SECTOR_SIZE;
    uint32_t in_sector = offset % FAT_SECTOR_SIZE;
    uint32_t value;

    if (!state->fat_sector_valid || state->fat_sector_lba != lba) {
        if (!fat_read_sector(lba, state->fat_sector)) {
            return 0;
        }

        state->fat_sector_lba = lba;
        state->fat_sector_valid = TRUE;
    }

    memcpy(&value, state->fat_sector + in_sector, sizeof(value));
    return value & FAT_CLUSTER_MASK;
}

boolean fat_set_entry(uint32_t cluster, uint32_t value) {
    if (cluster < 2 || cluster >= state->cluster_count + 2) {
        return FALSE;
    }

    uint32_t offset = cluster * 4;
    uint32_t sector_index = offset / FAT_SECTOR_SIZE;
    uint32_t in_sector = offset % FAT_SECTOR_SIZE;

    for (uint32_t copy = 0; copy < state->bpb.fat_count; copy++) {
        uint32_t lba = state->fat_lba
            + copy * state->bpb.fat_size32
            + sector_index;

        if (!fat_read_sector(lba, state->fat_io)) {
            return FALSE;
        }

        uint32_t old;
        memcpy(&old, state->fat_io + in_sector, sizeof(old));
        old = (old & 0xF0000000) | (value & FAT_CLUSTER_MASK);
        memcpy(state->fat_io + in_sector, &old, sizeof(old));

        if (!fat_write_sector(lba, state->fat_io)) {
            return FALSE;
        }

        if (copy == 0) {
            memcpy(state->fat_sector, state->fat_io, FAT_SECTOR_SIZE);
            state->fat_sector_lba = lba;
            state->fat_sector_valid = TRUE;
        }
    }

    return TRUE;
}

uint32_t fat_next_cluster(uint32_t cluster) {
    uint32_t next = fat_get_entry(cluster);

    if (next >= FAT_CLUSTER_EOC_MIN || next == FAT_CLUSTER_BAD) {
        return FAT_CLUSTER_EOC_MIN;
    }

    return next;
}

uint32_t fat_alloc_cluster(void) {
    for (uint32_t i = 0; i < state->cluster_count; i++) {
        uint32_t cluster = 2
            + (state->next_free - 2 + i) % state->cluster_count;

        if (fat_get_entry(cluster) == FAT_CLUSTER_FREE) {
            if (!fat_set_entry(cluster, FAT_CLUSTER_EOC_MIN)) {
                return 0;
            }

            state->next_free = cluster + 1;
            if (state->next_free >= state->cluster_count + 2) {
                state->next_free = 2;
            }

            return cluster;
        }
    }

    return 0;
}

boolean fat_free_chain(uint32_t first_cluster) {
    uint32_t cluster = first_cluster;

    for (uint32_t i = 0; i < state->cluster_count; i++) {
        if (cluster < 2 || cluster >= FAT_CLUSTER_EOC_MIN) {
            break;
        }

        uint32_t next = fat_get_entry(cluster);
        if (!fat_set_entry(cluster, FAT_CLUSTER_FREE)) {
            return FALSE;
        }

        cluster = next;
    }

    return TRUE;
}

boolean fat_zero_cluster(uint32_t cluster) {
    memset(state->fat_io, 0, FAT_SECTOR_SIZE);

    for (uint32_t sector = 0; sector < state->sectors_per_cluster; sector++) {
        uint32_t lba = fat_cluster_lba(cluster, sector);
        if (!fat_write_sector(lba, state->fat_io)) {
            return FALSE;
        }
    }

    return TRUE;
}

uint32_t fat_entry_cluster(const fat_dir_entry_t * entry) {
    return entry->first_cluster_low
        | ((uint32_t)entry->first_cluster_high << 16);
}

static boolean fat_valid_bpb(const fat32_bpb_t * bpb) {
    if (bpb->bytes_per_sector != FAT_SECTOR_SIZE) {
        return FALSE;
    }

    if (bpb->sectors_per_cluster == 0
        || (bpb->sectors_per_cluster & (bpb->sectors_per_cluster - 1)) != 0) {
        return FALSE;
    }

    if (bpb->reserved_sectors == 0 || bpb->fat_count == 0) {
        return FALSE;
    }

    if (bpb->root_entry_count != 0 || bpb->fat_size16 != 0
        || bpb->fat_size32 == 0) {
        return FALSE;
    }

    if (bpb->root_cluster < 2) {
        return FALSE;
    }

    return bpb->total_sectors16 != 0 || bpb->total_sectors32 != 0;
}

static boolean fat_load_bpb(partition_t * part) {
    uint16_t signature;

    if (!part_read_sectors(part, 0, 1, state->sector)) {
        return FALSE;
    }

    memcpy(&signature, state->sector + FAT_SECTOR_SIZE - 2, sizeof(signature));
    if (signature != FAT_BOOT_SIGNATURE) {
        return FALSE;
    }

    memcpy(&state->bpb, state->sector, sizeof(state->bpb));
    return fat_valid_bpb(&state->bpb);
}

boolean fat_mount(uint8_t drive) {
    partition_t part = {
        .disk = &state->disk,
        .part_offset = 0,
        .part_size = 0
    };
    boolean mounted = FALSE;

    memset(state, 0, sizeof(fat_state_t));
    state->fat_sector_lba = 0xFFFFFFFF;

    if (!disk_init(&state->disk, drive)) {
        return FALSE;
    }

    /* 1. Normal MBR partition table with a FAT32 entry. */
    if (mbr_find_fat32(&part, &state->disk)) {
        state->part = part;
        mounted = fat_load_bpb(&state->part);
    }

    /* 2. Superfloppy: a FAT32 BPB directly at LBA 0. */
    if (!mounted) {
        state->part.disk = &state->disk;
        state->part.part_offset = 0;
        state->part.part_size = 0;
        mounted = fat_load_bpb(&state->part);
    }

    /* 3. OpenDelta's fixed volume location. */
    if (!mounted) {
        state->part.part_offset = OPEN_DELTA_FAT32_LBA;
        state->part.part_size = 0;
        mounted = fat_load_bpb(&state->part);
    }

    if (!mounted) {
        return FALSE;
    }

    state->total_sectors = state->bpb.total_sectors16 != 0
        ? state->bpb.total_sectors16
        : state->bpb.total_sectors32;

    state->sectors_per_cluster = state->bpb.sectors_per_cluster;
    state->bytes_per_cluster = state->sectors_per_cluster * FAT_SECTOR_SIZE;
    state->fat_lba = state->bpb.reserved_sectors;
    state->data_lba = state->fat_lba
        + state->bpb.fat_count * state->bpb.fat_size32;
    state->root_cluster = state->bpb.root_cluster;
    state->cluster_count = (state->total_sectors - state->data_lba)
        / state->sectors_per_cluster;
    state->next_free = 2;

    if (state->cluster_count < 1
        || state->data_lba >= state->total_sectors) {
        return FALSE;
    }

    memcpy(state->label, state->bpb.volume_label, 11);
    state->label[11] = '\0';

    for (int i = 0; i < FAT_MAX_HANDLES; i++) {
        state->handles[i].handle = i;
        state->handles[i].opened = FALSE;
    }

    state->mounted = TRUE;
    return TRUE;
}

boolean fat_mounted(void) {
    return state->mounted;
}

const char * fat_volume_label(void) {
    return state->label;
}
