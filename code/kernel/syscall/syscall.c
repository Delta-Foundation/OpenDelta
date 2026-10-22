#include "../kernel-lib/syscall.h"
#include "../kernel-lib/system.h"
#include "../libc/include/stdbase.h"
#include "../libc/include/types.h"

static sys_process_t * current_process = {0};

__attribute__((noreturn))
int * sys_exit(int retval) {
    task_exit((retval & 0xFF) << 8);
    for (;;) { ; }
}

int sys_get_euid(void) {
    return current_process->user;
}

int sys_open(
    const char * file,
    int flags,
    int mode
) {
    int fd = 0;
    
    if (flags & O_CREAT) {
        mode = ((mode_t *)&flags)[1];
    }
    
    __asm__ volatile (
        "int $0x80\n"
        : "=a"(fd)
        : "a"(5),
          "b"(file),
          "c"(flags),
          "d"(mode)
        : "memory"
    );

    return fd;
}

int sys_create(
    const char * file,
    mode_t mode
) {
    int fd;
    __asm__ volatile (
        "int $0x80\n"
        : "=a"(fd)
        : "a"(8),
          "b"(file),
          "c"(mode)
        : "memory"
    );

    return fd;
}

int sys_read(
    int fd,
    char * ptr,
    int len
) {
    return fd;
}

int sys_write(
    int fd, 
    char * ptr,
    int len
) {
    return fd;
}

int sys_close(int fd) {
    return 0;
}

off_t lseek(
    int fd, 
    off_t offset, 
    int whence
) {
    off_t res;
    __asm__ volatile (
        "int $0x80\n"
        : "=a"(res)
        : "a"(19),
          "b"(fd),
          "c"(offset),
          "d"(whence)
        : "memory"
    );
    return res;
}

void wait(int ms) {
    volatile int count;
    while (ms--) {
        count = 100000;
        while (count--) {
            __asm__ ( "nop" );
        }
    }
}

static int (*syscalls[]) = {
    [SYS_EXIT] = (int *)sys_exit,
    [SYS_GET_EUID] = (int *)sys_get_euid,
    [SYS_OPEN] = (int *)sys_open,
    [SYS_READ] = (int *)sys_read,
    [SYS_WRITE] = (int *)sys_write,
    [SYS_CLOSE] = (int *)sys_close,
    [SYS_CREATE] = (int *)sys_create
};

uint32_t num_syscalls = sizeof(syscalls) / sizeof(*syscalls);

void syscall_install(void)
{
    char syscall_ascii[2] = "";
    int_to_ascii(num_syscalls, syscall_ascii);
    prints("\n[info]: [initializing syscall table with ]", WHITE);
    prints(syscall_ascii, 0x08);
    prints(" functions\n", WHITE);
}
