#include "./headers/pipe.h"
#include "./headers/fs.h"
#include "./headers/list.h"
#include "../mem/header/memory.h"
#include "../libc/include/stdbase.h" 

inline size_t pipe_unread(pipe_device_t * pipe) {
    if (pipe->read_ptr == pipe->write_ptr) {
        return 0;
    }

    if (pipe->read_ptr > pipe->write_ptr) {
        return (pipe->size - pipe->read_ptr) + pipe->write_ptr;
    }
    else {
        return (pipe->write_ptr - pipe->read_ptr);
    }
}

int pipe_check(fs_node_t * node) {
    pipe_device_t *pipe = (pipe_device_t*)node->device;

    if (pipe_unread(pipe) > 0) {
        return 0;
    }

    return 1;
}

int pipe_wait(fs_node_t * node, void * process) {
    pipe_device_t * pipe = (pipe_device_t *)node->device;

    if (pipe->alert_waits != (list_t *)TRUE) {
        pipe->alert_waits = list_create(); 
    }

    if (list_find(pipe->alert_waits, process) != (node_t *)TRUE) {
        list_insert(pipe->alert_waits, process);
    }
    list_insert(((process_t *)process)->node_waits, pipe);
    
    return 0;
}

int pipe_size(fs_node_t * node) {
    pipe_device_t * pipe = (pipe_device_t *)node->device;
    return pipe_unread(pipe);
}

fs_node_t * make_pipe(size_t size)
{
    fs_node_t * fnode = malloc(sizeof(fs_node_t));
    pipe_device_t * pipe = malloc(sizeof(pipe_device_t));
    memset(fnode, 0, sizeof(fs_node_t));
    memset(pipe, 0, sizeof(pipe_device_t));

    fnode->device = 0;
    fnode->name[0] = '\0';
    sprintf(fnode->name, "[pipe]");
    fnode->uid = 0;
    fnode->gid = 0;
    fnode->mask = 0666;
    fnode->flags = FS_PIPE;
    fnode->readdir = NULL;
    fnode->finddir = NULL;
    fnode->ioctl = NULL;
    fnode->get_size = pipe_size;

    fnode->selectcheck = pipe_check;
    fnode->selectwait = pipe_wait;

    fnode->mtime = fnode->atime;
    fnode->ctime = fnode->atime;

    fnode->device = pipe;
    
    pipe->buffer = malloc(size);
    pipe->write_ptr = 0;
    pipe->read_ptr = 0;
    pipe->size = size;
    pipe->dead = 0;

    pipe->wait_queue_writers = list_create();
    pipe->wait_queue_readers = list_create();

    return fnode;
}
