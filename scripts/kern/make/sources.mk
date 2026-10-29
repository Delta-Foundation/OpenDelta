BOOT_SRC = boot/x86_64/main.asm
STAGE2_SRC = boot/x86_64/second-stage.asm
ENTRY_SRC = kernel_entry.asm

BOOT_DEPS = $(BOOT_SRC) boot/x86_64/print.asm \
			boot/x86_64/rmode/functions.asm boot/x86_64/rmode/data.asm

STAGE2_DEPS = $(STAGE2_SRC) \
			boot/x86_64/global-functions.asm boot/x86_64/global-data.asm \
			boot/x86_64/print.asm boot/x86_64/macros.asm \
			boot/x86_64/pmode/functions.asm boot/x86_64/pmode/data.asm \
			boot/x86_64/rmode/functions.asm boot/x86_64/rmode/data.asm

KERNEL_SRC = kernel.c
GDT_SRC = arch/gdt/gdt.c
GDT_ASM_SRC = arch/gdt/gdt.asm
IDT_ASM_SRC = cpu/asm/idt.asm
INTS_ASM_SRC = cpu/asm/ints.asm
IDT_SRC = cpu/idt.c
ISR_SRC = cpu/isr.c
INTS_C_SRC = ints/int.c
PIC_SRC = drvs/pic.c
HAL_SRC = hal/hal.c
PORTS_SRC = ports/ports.c
FPU_SRC = fpu/fpu.c
SCREEN_SRC = drvs/screen.c
SPEAKER_SRC = drvs/speaker.c
MOUSE_SRC = drvs/mouse.c
KEYBOARD_SRC = drvs/keyboard.c
MEM_SRC = mem/memory.c
SHM_SRC = mem/shared_memory.c
SYS_SRC = syscall/syscall.c
TASK_SRC = syscall/task.c
PROC_SRC = syscall/proc.c
FS_SRC = fs/fs.c
LIST_SRC = fs/list.c
PIPE_SRC = fs/pipe.c

# dltlibc
STDBASE_SRC = libc/src/stdbase/alloc.c \
			  libc/src/stdbase/base.c \
			  libc/src/stdbase/files.c \
			  libc/src/stdbase/input-output.c
STDLIB_SRC = libc/src/stdlib/stdlib.c
CTYPE_SRC = libc/src/ctype/ctype.c
TYPES_SRC = libc/src/types/types.c
STRING_SRC = libc/src/string/string.c
DEBUG_SRC = libc/src/debug/debug.c

TTY_SRC = tty/tty.c
MIN_DLTSH_SRC = tty/min_dltsh.c
FS_SHELL_SRC = tty/fs_shell.c
DEXIDE_SRC = tty/dexide.c
TOOLS_SRC = tools/fat/asm/tools.asm
FAT_SRC = tools/fat/fat.c
FAT_DIR_SRC = tools/fat/fat_dir.c
FAT_FILE_SRC = tools/fat/fat_file.c
ELF_SRC = tools/fat/elf.c
MBR_SRC = tools/fat/mbr.c
DISK_SRC = tools/fat/disk.c
TOOLS_SRC = tools/fat/asm/tools.asm
CINIT_SRC = ../cinit/main.c
