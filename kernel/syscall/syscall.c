#include "../lib/syscall.h"
#include "../lib/system.h"
#include "../lib/stdbase.h"
#include "../lib/types.h"

static sys_process_t *current_process = {0};

int __attribute((noreturn)) sysExit(int retval) {
    taskExit((retval & 0xFF) << 8);
    for (;;) { ; }
}

int sysGeteuid(void) {
    return current_process->user;
}

int sysOpen(const char *file, int flags, int mode) {
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

int sysCreate(const char *file, mode_t mode) {
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

int sysRead(int fd, char *ptr, int len) {
    return fd;
}

int sysWrite(int fd, char *ptr, int len) {
    return fd;
}

int sysClose(int fd) {
    return 0;
}

off_t lseek(int fd, off_t offset, int whence) {
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

static int (*syscalls[]) = {
    [SYS_EXT] = sysExit,
    [SYS_GETEUID] = sysGeteuid,
    [SYS_OPEN] = sysOpen,
    [SYS_READ] = sysRead,
    [SYS_WRITE] = sysWrite,
    [SYS_CLOSE] = sysClose,
    [SYS_CREATE] = sysCreate
};

uint32_t num_syscalls = sizeof(syscalls) / sizeof(*syscalls);

void syscallInstall(void)
{
    char syscallAscii[2] = "";
    int_to_ascii(num_syscalls, syscallAscii);
    prints("\n[info]: [initializing syscall table with ]", WHITE);
    prints(syscallAscii, 0x08);
    prints(" functions\n", WHITE);
}
