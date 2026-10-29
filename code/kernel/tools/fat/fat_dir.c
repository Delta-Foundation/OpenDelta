#include "./headers/fat_internal.h"
#include "../../mem/header/memory.h"
#include "../../libc/include/string.h"

#define FAT_ENTRY_SIZE      32
#define FAT_ENTRIES_PER_SEC (FAT_SECTOR_SIZE / FAT_ENTRY_SIZE)
#define FAT_DATE_1980       ((1 << 5) | 1)

static char fat_upper(char c) {
    if (c >= 'a' && c <= 'z') {
        return (char)(c - 'a' + 'A');
    }

    return c;
}

static char fat_short_char(char c) {
    if (c == ' ' || c == '.'
        || c == '"' || c == '*' || c == '+'
        || c == ',' || c == '/' || c == ':'
        || c == ';' || c == '<' || c == '='
        || c == '>' || c == '?' || c == '['
        || c == '\\' || c == ']' || c == '|') {
        return '_';
    }

    return fat_upper(c);
}

static boolean fat_name_to_short(
    const char * name,
    uint8_t short_name[11]
) {
    memset(short_name, ' ', 11);

    uint32_t length = strlen(name);
    if (length == 0) {
        return FALSE;
    }

    const char * dot = NULL;
    for (uint32_t i = 0; i < length; i++) {
        if (name[i] == '.') {
            dot = name + i;
        }
    }

    uint32_t base_length = dot != NULL ? (uint32_t)(dot - name) : length;
    uint32_t ext_length = dot != NULL ? length - base_length - 1 : 0;

    if (base_length == 0 || base_length > 8 || ext_length > 3) {
        return FALSE;
    }

    for (uint32_t i = 0; i < base_length; i++) {
        short_name[i] = (uint8_t)fat_short_char(name[i]);
    }

    for (uint32_t i = 0; i < ext_length; i++) {
        short_name[8 + i] = (uint8_t)fat_short_char(dot[i + 1]);
    }

    return TRUE;
}

static void fat_short_to_name(
    const uint8_t short_name[11],
    char * name
) {
    uint32_t pos = 0;

    for (int i = 0; i < 8 && short_name[i] != ' '; i++) {
        name[pos++] = (char)short_name[i];
    }

    if (short_name[8] != ' ') {
        name[pos++] = '.';

        for (int i = 8; i < 11 && short_name[i] != ' '; i++) {
            name[pos++] = (char)short_name[i];
        }
    }

    name[pos] = '\0';
}

static boolean fat_is_dot_entry(const fat_dir_entry_t * entry) {
    static const uint8_t dot[11] = { '.', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
    static const uint8_t dotdot[11] = { '.', '.', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

    return memcmp(entry->name, dot, 11) == 0
        || memcmp(entry->name, dotdot, 11) == 0;
}

static boolean fat_is_regular_entry(const fat_dir_entry_t * entry) {
    if (entry->name[0] == FAT_ENTRY_FREE || entry->name[0] == FAT_ENTRY_END) {
        return FALSE;
    }

    if (entry->attributes == FAT_ATTR_LFN) {
        return FALSE;
    }

    return (entry->attributes & FAT_ATTR_VOLUME_ID) == 0;
}

boolean fat_dir_walk(
    uint32_t dir_cluster,
    fat_dir_visit_t visit,
    void * context
) {
    fat_state_t * state = fat_get_state();
    uint32_t cluster = dir_cluster;

    while (cluster >= 2 && cluster < FAT_CLUSTER_EOC_MIN) {
        for (uint32_t sector = 0; sector < state->sectors_per_cluster; sector++) {
            uint32_t lba = fat_cluster_lba(cluster, sector);
            if (!fat_read_sector(lba, state->sector)) {
                return FALSE;
            }

            for (uint32_t index = 0; index < FAT_ENTRIES_PER_SEC; index++) {
                uint32_t offset = index * FAT_ENTRY_SIZE;
                fat_dir_entry_t * entry = (fat_dir_entry_t *)(state->sector + offset);

                if (entry->name[0] == FAT_ENTRY_END) {
                    return TRUE;
                }

                if (visit(entry, lba, offset, context)) {
                    return TRUE;
                }
            }
        }

        cluster = fat_next_cluster(cluster);
    }

    return TRUE;
}

typedef struct {
    const uint8_t * short_name;
    fat_dir_entry_t * entry;
    uint32_t lba;
    uint32_t offset;
    boolean found;
} fat_find_context_t;

static boolean fat_find_visit(
    fat_dir_entry_t * entry,
    uint32_t lba,
    uint32_t offset,
    void * context
) {
    fat_find_context_t * ctx = (fat_find_context_t *)context;

    if (!fat_is_regular_entry(entry)) {
        return FALSE;
    }

    if (memcmp(entry->name, ctx->short_name, 11) != 0) {
        return FALSE;
    }

    *ctx->entry = *entry;
    ctx->lba = lba;
    ctx->offset = offset;
    ctx->found = TRUE;
    return TRUE;
}

boolean fat_dir_find(
    uint32_t dir_cluster,
    const char * name,
    fat_dir_entry_t * entry_out,
    uint32_t * lba_out,
    uint32_t * offset_out
) {
    uint8_t short_name[11];
    fat_find_context_t context;

    if (!fat_name_to_short(name, short_name)) {
        return FALSE;
    }

    context.short_name = short_name;
    context.entry = entry_out;
    context.found = FALSE;

    if (!fat_dir_walk(dir_cluster, fat_find_visit, &context)) {
        return FALSE;
    }

    if (!context.found) {
        return FALSE;
    }

    if (lba_out != NULL) {
        *lba_out = context.lba;
    }

    if (offset_out != NULL) {
        *offset_out = context.offset;
    }

    return TRUE;
}

static boolean fat_dir_find_free(
    uint32_t dir_cluster,
    uint32_t * lba_out,
    uint32_t * offset_out
) {
    fat_state_t * state = fat_get_state();
    uint32_t cluster = dir_cluster;
    uint32_t last_cluster = dir_cluster;

    while (cluster >= 2 && cluster < FAT_CLUSTER_EOC_MIN) {
        last_cluster = cluster;

        for (uint32_t sector = 0; sector < state->sectors_per_cluster; sector++) {
            uint32_t lba = fat_cluster_lba(cluster, sector);
            if (!fat_read_sector(lba, state->sector)) {
                return FALSE;
            }

            for (uint32_t index = 0; index < FAT_ENTRIES_PER_SEC; index++) {
                uint32_t offset = index * FAT_ENTRY_SIZE;
                fat_dir_entry_t * entry = (fat_dir_entry_t *)(state->sector + offset);

                if (entry->name[0] == FAT_ENTRY_END
                    || entry->name[0] == FAT_ENTRY_FREE) {
                    *lba_out = lba;
                    *offset_out = offset;
                    return TRUE;
                }
            }
        }

        cluster = fat_next_cluster(cluster);
    }

    uint32_t new_cluster = fat_alloc_cluster();
    if (new_cluster == 0) {
        return FALSE;
    }

    if (!fat_zero_cluster(new_cluster)) {
        return FALSE;
    }

    if (!fat_set_entry(last_cluster, new_cluster)) {
        fat_free_chain(new_cluster);
        return FALSE;
    }

    *lba_out = fat_cluster_lba(new_cluster, 0);
    *offset_out = 0;
    return TRUE;
}

static boolean fat_write_entry(
    uint32_t lba,
    uint32_t offset,
    const fat_dir_entry_t * entry
) {
    fat_state_t * state = fat_get_state();

    if (!fat_read_sector(lba, state->sector)) {
        return FALSE;
    }

    memcpy(state->sector + offset, entry, sizeof(fat_dir_entry_t));
    return fat_write_sector(lba, state->sector);
}

static void fat_fill_entry(
    fat_dir_entry_t * entry,
    const char * name,
    uint8_t attributes,
    uint32_t cluster,
    uint32_t size
) {
    memset(entry, 0, sizeof(fat_dir_entry_t));
    memset(entry->name, ' ', 11);

    if (strcmp(name, ".") == 0) {
        entry->name[0] = '.';
    }
    else if (strcmp(name, "..") == 0) {
        entry->name[0] = '.';
        entry->name[1] = '.';
    }
    else {
        fat_name_to_short(name, entry->name);
    }

    entry->attributes = attributes;
    entry->created_date = FAT_DATE_1980;
    entry->accessed_date = FAT_DATE_1980;
    entry->modified_date = FAT_DATE_1980;
    entry->first_cluster_low = (uint16_t)(cluster & 0xFFFF);
    entry->first_cluster_high = (uint16_t)((cluster >> 16) & 0xFFFF);
    entry->size = size;
}

static boolean fat_mark_deleted(
    uint32_t lba,
    uint32_t offset
) {
    fat_state_t * state = fat_get_state();

    if (!fat_read_sector(lba, state->sector)) {
        return FALSE;
    }

    state->sector[offset] = FAT_ENTRY_FREE;
    return fat_write_sector(lba, state->sector);
}

static boolean fat_split_path(
    const char * path,
    char * parent,
    char * name
) {
    if (path == NULL || path[0] == '\0' || strcmp(path, "/") == 0) {
        return FALSE;
    }

    const char * last_slash = NULL;
    for (const char * p = path; *p != '\0'; p++) {
        if (*p == '/') {
            last_slash = p;
        }
    }

    if (last_slash == NULL) {
        strncpy(parent, "/", FAT_MAX_PATH);
        strncpy(name, path, FAT_MAX_PATH);
    }
    else {
        uint32_t parent_length = (uint32_t)(last_slash - path);
        if (parent_length == 0) {
            strncpy(parent, "/", FAT_MAX_PATH);
        }
        else {
            if (parent_length >= FAT_MAX_PATH) {
                return FALSE;
            }

            memcpy(parent, path, parent_length);
            parent[parent_length] = '\0';
        }

        strncpy(name, last_slash + 1, FAT_MAX_PATH);
    }

    return name[0] != '\0';
}

boolean fat_dir_resolve(
    const char * path,
    uint32_t * cluster_out
) {
    fat_state_t * state = fat_get_state();

    if (path == NULL || path[0] == '\0' || strcmp(path, "/") == 0) {
        *cluster_out = state->root_cluster;
        return TRUE;
    }

    char work[FAT_MAX_PATH];
    strncpy(work, path, FAT_MAX_PATH);

    char * p = work;
    uint32_t cluster = state->root_cluster;

    while (*p != '\0') {
        while (*p == '/') {
            p++;
        }

        if (*p == '\0') {
            break;
        }

        char component[FAT_MAX_PATH];
        uint32_t length = 0;

        while (*p != '\0' && *p != '/') {
            if (length + 1 < FAT_MAX_PATH) {
                component[length++] = *p;
            }
            p++;
        }
        component[length] = '\0';

        if (strcmp(component, ".") == 0) {
            continue;
        }

        fat_dir_entry_t entry;
        if (!fat_dir_find(cluster, component, &entry, NULL, NULL)) {
            return FALSE;
        }

        if ((entry.attributes & FAT_ATTR_DIR) == 0) {
            return FALSE;
        }

        cluster = fat_entry_cluster(&entry);
        if (cluster < 2) {
            return FALSE;
        }
    }

    *cluster_out = cluster;
    return TRUE;
}

boolean fat_lookup(
    const char * path,
    fat_dir_entry_t * entry_out,
    uint32_t * lba_out,
    uint32_t * offset_out
) {
    fat_state_t * state = fat_get_state();

    if (path == NULL || path[0] == '\0' || strcmp(path, "/") == 0) {
        return FALSE;
    }

    char work[FAT_MAX_PATH];
    strncpy(work, path, FAT_MAX_PATH);

    char * p = work;
    uint32_t cluster = state->root_cluster;

    while (*p != '\0') {
        while (*p == '/') {
            p++;
        }

        if (*p == '\0') {
            break;
        }

        char component[FAT_MAX_PATH];
        uint32_t length = 0;

        while (*p != '\0' && *p != '/') {
            if (length + 1 < FAT_MAX_PATH) {
                component[length++] = *p;
            }
            p++;
        }
        component[length] = '\0';

        if (strcmp(component, ".") == 0) {
            continue;
        }

        fat_dir_entry_t entry;
        if (!fat_dir_find(cluster, component, &entry, lba_out, offset_out)) {
            return FALSE;
        }

        if (*p == '\0') {
            *entry_out = entry;
            return TRUE;
        }

        if ((entry.attributes & FAT_ATTR_DIR) == 0) {
            return FALSE;
        }

        cluster = fat_entry_cluster(&entry);
        if (cluster < 2) {
            return FALSE;
        }
    }

    return FALSE;
}

boolean fat_exists(const char * path) {
    if (path == NULL || path[0] == '\0' || strcmp(path, "/") == 0) {
        return TRUE;
    }

    fat_dir_entry_t entry;
    return fat_lookup(path, &entry, NULL, NULL);
}

boolean fat_is_dir(const char * path) {
    if (path == NULL || path[0] == '\0' || strcmp(path, "/") == 0) {
        return TRUE;
    }

    fat_dir_entry_t entry;

    if (!fat_lookup(path, &entry, NULL, NULL)) {
        return FALSE;
    }

    return (entry.attributes & FAT_ATTR_DIR) != 0;
}

typedef struct {
    fat_dir_callback_t callback;
    void * context;
} fat_list_context_t;

static boolean fat_list_visit(
    fat_dir_entry_t * entry,
    uint32_t lba,
    uint32_t offset,
    void * context
) {
    fat_list_context_t * ctx = (fat_list_context_t *)context;
    char name[13];

    (void)lba;
    (void)offset;

    if (!fat_is_regular_entry(entry) || fat_is_dot_entry(entry)) {
        return FALSE;
    }

    fat_short_to_name(entry->name, name);
    ctx->callback(
        name,
        (entry->attributes & FAT_ATTR_DIR) != 0,
        entry->size,
        ctx->context
    );

    return FALSE;
}

boolean fat_list_dir(
    const char * path,
    fat_dir_callback_t callback,
    void * context
) {
    uint32_t cluster;

    if (callback == NULL || !fat_dir_resolve(path, &cluster)) {
        return FALSE;
    }

    fat_list_context_t list_context = {
        .callback = callback,
        .context = context
    };

    return fat_dir_walk(cluster, fat_list_visit, &list_context);
}

typedef struct {
    boolean empty;
} fat_empty_context_t;

static boolean fat_empty_visit(
    fat_dir_entry_t * entry,
    uint32_t lba,
    uint32_t offset,
    void * context
) {
    fat_empty_context_t * ctx = (fat_empty_context_t *)context;

    (void)lba;
    (void)offset;

    if (!fat_is_regular_entry(entry) || fat_is_dot_entry(entry)) {
        return FALSE;
    }

    ctx->empty = FALSE;
    return TRUE;
}

static boolean fat_dir_is_empty(uint32_t cluster) {
    fat_empty_context_t context = { .empty = TRUE };

    if (!fat_dir_walk(cluster, fat_empty_visit, &context)) {
        return FALSE;
    }

    return context.empty;
}

boolean fat_create_file(const char * path) {
    fat_state_t * state = fat_get_state();
    char parent[FAT_MAX_PATH];
    char name[FAT_MAX_PATH];

    if (!state->mounted || !fat_split_path(path, parent, name)) {
        return FALSE;
    }

    uint8_t short_name[11];
    if (!fat_name_to_short(name, short_name)) {
        return FALSE;
    }

    uint32_t cluster;
    if (!fat_dir_resolve(parent, &cluster)) {
        return FALSE;
    }

    fat_dir_entry_t existing;
    if (fat_dir_find(cluster, name, &existing, NULL, NULL)) {
        return FALSE;
    }

    uint32_t lba;
    uint32_t offset;
    if (!fat_dir_find_free(cluster, &lba, &offset)) {
        return FALSE;
    }

    fat_dir_entry_t entry;
    fat_fill_entry(&entry, name, FAT_ATTR_ARCHIVE, 0, 0);
    return fat_write_entry(lba, offset, &entry);
}

boolean fat_create_dir(const char * path) {
    fat_state_t * state = fat_get_state();
    char parent[FAT_MAX_PATH];
    char name[FAT_MAX_PATH];

    if (!state->mounted || !fat_split_path(path, parent, name)) {
        return FALSE;
    }

    uint8_t short_name[11];
    if (!fat_name_to_short(name, short_name)) {
        return FALSE;
    }

    uint32_t parent_cluster;
    if (!fat_dir_resolve(parent, &parent_cluster)) {
        return FALSE;
    }

    fat_dir_entry_t existing;
    if (fat_dir_find(parent_cluster, name, &existing, NULL, NULL)) {
        return FALSE;
    }

    uint32_t lba;
    uint32_t offset;
    if (!fat_dir_find_free(parent_cluster, &lba, &offset)) {
        return FALSE;
    }

    uint32_t new_cluster = fat_alloc_cluster();
    if (new_cluster == 0) {
        return FALSE;
    }

    if (!fat_zero_cluster(new_cluster)) {
        fat_free_chain(new_cluster);
        return FALSE;
    }

    fat_dir_entry_t dot;
    fat_dir_entry_t dotdot;

    fat_fill_entry(&dot, ".", FAT_ATTR_DIR, new_cluster, 0);
    fat_fill_entry(
        &dotdot,
        "..",
        FAT_ATTR_DIR,
        parent_cluster == state->root_cluster ? FAT_ROOT_PARENT : parent_cluster,
        0
    );

    if (!fat_write_entry(fat_cluster_lba(new_cluster, 0), 0, &dot)
        || !fat_write_entry(fat_cluster_lba(new_cluster, 0), FAT_ENTRY_SIZE, &dotdot)) {
        fat_free_chain(new_cluster);
        return FALSE;
    }

    fat_dir_entry_t entry;
    fat_fill_entry(&entry, name, FAT_ATTR_DIR, new_cluster, 0);

    if (!fat_write_entry(lba, offset, &entry)) {
        fat_free_chain(new_cluster);
        return FALSE;
    }

    return TRUE;
}

boolean fat_delete_file(const char * path) {
    fat_dir_entry_t entry;
    uint32_t lba;
    uint32_t offset;

    if (!fat_lookup(path, &entry, &lba, &offset)) {
        return FALSE;
    }

    if ((entry.attributes & FAT_ATTR_DIR) != 0) {
        return FALSE;
    }

    uint32_t cluster = fat_entry_cluster(&entry);
    if (cluster >= 2 && !fat_free_chain(cluster)) {
        return FALSE;
    }

    return fat_mark_deleted(lba, offset);
}

boolean fat_delete_dir(const char * path) {
    fat_dir_entry_t entry;
    uint32_t lba;
    uint32_t offset;

    if (!fat_lookup(path, &entry, &lba, &offset)) {
        return FALSE;
    }

    if ((entry.attributes & FAT_ATTR_DIR) == 0) {
        return FALSE;
    }

    uint32_t cluster = fat_entry_cluster(&entry);
    if (cluster < 2 || !fat_dir_is_empty(cluster)) {
        return FALSE;
    }

    if (!fat_mark_deleted(lba, offset)) {
        return FALSE;
    }

    return fat_free_chain(cluster);
}
