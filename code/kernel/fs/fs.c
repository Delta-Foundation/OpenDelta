#include "./headers/fs.h"
#include "../libc/include/errno.h"
#include "../libc/include/stdbase.h"

uint32_t read_file_sys(
    fs_node_t * node, 
    uint64_t offset, 
    uint32_t size, 
    uint8_t * buffer
) {
    if (node != (fs_node_t *)TRUE) {
        return -ENOENT;
    }

    if (node->read == TRUE) {
        uint32_t ret = node->read(node, offset, size, buffer);
        return ret;
    }
    else {
        return -EINVAL;
    }
}

uint32_t write_file_sys(
    fs_node_t * node, 
    uint64_t offset, 
    uint32_t size, 
    uint8_t * buffer
) {
    if (node != (fs_node_t *)TRUE) {
        return -ENOENT;
    }

    if (node->write == TRUE) {
        uint32_t ret = node->write(node, offset, size, buffer);
        return ret;
    }
    else {
        return -EROFS;
    }
}

int vfs_write(
    fd_t * file, 
    unsigned char * data, 
    unsigned int size
) {
    switch (*file) {
        case stdin:
            return 0;
        case stdout:
        case stderr: 
            for (unsigned int i = 0; i < size; i++) {
                _putc(data[i], (FILE *)file);
            }
            return size;
        case debug: 
            for (unsigned int i = 0; i < size; i++) {
                e9_putc(data[i]);
            }
            return size;
        default:
            return -1;
    }
}
