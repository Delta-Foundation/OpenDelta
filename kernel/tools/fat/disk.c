#include "./disk.h"

boolean disk_init(DISK *disk, const char *filename) {
    disk->file = fopen((const char *)filename, "r");
    return (disk->file != NULL);
}
