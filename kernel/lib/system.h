#ifndef SYSTEM_H
#define SYSTEM_H

#include "./time.h"
#include "./types.h"
#include "./string.h"
#include "../fs/headers/list.h"

/*---types---*/
typedef signed int pid_t;
typedef unsigned int user_t;
typedef unsigned int status_t;

/*---macros---*/
#define KERNEL_STACK_SIZE 0x8000
#define NUMSIGNS 38 // numder of signals
#define USER_ROOT_UID (user_t)0
#define IRQ_OFF { __asm__ volatile ("cli"); }
#define PAUSE   { __asm__ volatile ("hlt"); }

#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2

/*---structures---*/
typedef struct page {
    unsigned int present:1;
    unsigned int rw:1;
    unsigned int user:1;
    unsigned int write_through:1;
    unsigned int cache_disable:1;
    unsigned int accessed:1;
    unsigned int dirty:1;
    unsigned int pat:1;
    unsigned int global:1;
    unsigned int unused:3;
    unsigned int frame:20;
} __attribute__((packed)) page_t;

typedef struct PageTable {
    page_t pages[1024];
} page_table_t;

typedef struct PageDir {
    uintptr_t physical_tables[1024];
    page_table_t *tables[1024];
    uintptr_t physical_address;
    int32_t ref_count;
} page_dir_t;

typedef struct thread {
    uintptr_t esp;
    uintptr_t ebp;
    uintptr_t eip;
    uint8_t fpu_enabled;
    uint8_t fp_regs[512];
    uint8_t padding[32];
    page_dir_t *page_dir;
} thread_t;

typedef struct image {
    size_t size;
    uintptr_t entry;
    uintptr_t heap;
    uintptr_t head_actual;
    uintptr_t stack;
    uintptr_t user_stack;
    uintptr_t start;
    uintptr_t shm_heap;
    volatile int lock[2];
} image_t;

typedef struct descriptor_table {
    uint64_t *offset;
    int *modes;
    size_t len;
    size_t capacity;
    size_t refs;
} fd_table_t;

typedef struct signal_table {
    uintptr_t functions[NUMSIGNS+1];
} sig_table_t;

typedef struct tree_node {
    void *value;
    list_t *children;
    struct tree_node *parent;
    char *word;
    int count;
    struct tree_node *left;
    struct tree_node *right;
} tree_node_t;

typedef struct {
    size_t nodes;
    tree_node_t *root;
} tree_t;

typedef struct process {
    pid_t id;
    char *name;
    char *description;
    user_t user;
    user_t real_user;
    int mask;

    char **cmdline;

    pid_t group;
    pid_t job;
    pid_t session;

    thread_t theard;
    tree_node_t *tree_entry;
    image_t image;
    char *wd_name;
    fd_table_t *fds;
    status_t status;
    sig_table_t signals;
    uint8_t finished;
    uint8_t started;
    uint8_t running;
    list_t *wait_queue;
    list_t *shm_mappings;
    list_t *signal_queue;
    thread_t signal_state;
    char *signal_kstack;
    node_t sched_node;
    node_t sleep_node;
    node_t *timed_sleep_node;
    uint8_t is_tasklet;
    volatile uint8_t sleep_interrupted;
    list_t *node_waits;
    int awoken_index;
    node_t *timeout_node;
    struct Timeval start;
    uint8_t suspended;
} sys_process_t;

void initProcTree(void);
void initProc(const char* pname, pid_t pid);
sys_process_t *spawnInit(void);
sys_process_t *spawnKIdle(void);
void initializeProcessTree(void);
void switchPageDirectory(page_dir_t *dir);
void taskingInstall(void);
void taskExit(int retval);
int __attribute((noreturn)) *sysExit(int retval);
int sysGeteuid(void);
int sysOpen(const char *file, int flgas, int mode);
int sysRead(int fd, char *ptr, int len);
int sysWrite(int fd, char *ptr, int len);
int sysClose(int fd);
int sysCreate(const char *file, mode_t mode);
void wait(int ms);
off_t lseek(int fd, off_t offset, int whence);
void syscallInstall(void);
tree_t *treeCreate(void);
tree_node_t *treeNodeCreate(void *value);
void treeSetRoot(tree_t *tree, void *value);
void shmInstall(void);

#endif
