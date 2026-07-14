#include <stdio.h>
#include <stdint.h>

typedef enum {
    MAX_NUM_OF_USER_PKGS = 8192,
    MAX_NUM_OF_BASE_PKGS = 512,
} BaseConsts;

typedef struct {
    const char *install[64];
    const char *build[64];
    const char *delete[64];
    const char *update[64];
    const char *sync[64];
} Flags;

typedef struct {
    const char *dltkernel[128];
    const char *linux_kernel[128];
    const char *linux_firmware[128];
    const char *haskell_compiler[32];
    const char *clang_llvm[64];
    const char *nasm[8];
} Base;

static Flags flags = {
    .install = " -i\n",
    .delete = " -d\n",
    .build = " -b\n",
    .sync = " -s\n",
    .update = " -u\n"
};

static Base base = {
    .dltkernel = " dltkernel-latest\n",
    .linux_kernel = " linux\n",
    .linux_firmware = " linux-firmware\n",
    .haskell_compiler = " ghc\n",
    .clang_llvm = " clang/llvm\n",
    .nasm = " nasm\n"
};

static void set_flags(void) {
    printf("flags: %s\n", &flags);
}

static void set_base(void) {
    printf("base pkgs: %s\n", &base);
}

int main()
{
    printf("[FPMCP basics]\n");
    set_flags();
    set_base();
    return 0;
}
