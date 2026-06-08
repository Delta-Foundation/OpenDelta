# asm translator
AS=nasm

# clang tools
CC = clang
CXX = clang++
LD = ld.lld
OBJCOPY = llvm-objcopy

# source code
SRCS = kernel.c
KERNEL_OBJ = obj/kernel.o
KERNEL_ELF = img/kernel.elf
KERNEL_BIN = img/kernel.bin
ENTRY = obj/entry.o
ENTRY_SRC = kernel_entry.asm
BOOT = img/boot.bin
BOOT_SRC = boot/i386/boot.asm
STDBASE = obj/stdbase.o
STDBASE_SRS = lib/source/stdbase.c
STDLIB = obj/stdlib.o 
STDLIB_SRC = lib/source/stdlib.c 
TYPES = obj/types.o
TYPES_SRC = lib/source/types.c
STRING = obj/string.o
STRING_SRC = lib/source/string.c
IDT = obj/idt.o
IDT_SRC = cpu/idt.c
GDT = obj/gdt.o
GDT_SRC = arch/gdt/gdt.c
GDT_ASM = obj/gdt_asm.o
GDT_ASM_SRC = arch/gdt/gdt.asm
MEM = obj/mem.o
MEM_SRC = mem/memory.c
SHM = obj/shm.o
SHM_SRC = mem/shared_memory.c
SCREEN = obj/screen.o
SCREEN_SRC = drvs/screen.c
PORTS = obj/ports.o
PORTS_SRC = ports/ports.c
TTY = obj/tty.o
TTY_SRC = tty/tty.c
CTYPE = obj/ctype.o
CTYPE_SRC = lib/source/ctype.c
IDT_ASM_SRC = cpu/asm/idt.asm
IDT_ASM = obj/idta.o
INTS_ASM_SRC = cpu/asm/ints.asm
INTS_ASM = obj/ints.o
ISR_SRC = cpu/isr.c
ISR = obj/isr.o
PIPE = obj/pipe.o
PIPE_SRC = fs/pipe.c
LIST = obj/list.o
LIST_SRC = fs/list.c
FS = obj/fs.o
FS_SRC = fs/fs.c
FPU = obj/fpu.o
FPU_SRC = fpu/fpu.c
SYS = obj/sys.o
SYS_SRC = syscall/syscall.c
TASK = obj/task.o
TASK_SRC = syscall/task.c
PROC = obj/proc.o
PROC_SRC = syscall/proc.c
PIC = obj/pic.o
PIC_SRC = drvs/pic.c
TOOLS = obj/toolsa.o
TOOLS_SRC = tools/fat/asm/tools.asm 
FAT = obj/fat.o 
FAT_SRC = tools/fat/fat.c
DISK = obj/disk.o 
DISK_SRC = tools/fat/disk.c 
MBR = obj/mbr.o 
MBR_SRC = tools/fat/mbr.c 
ELF = obj/elf.o
ELF_SRC = tools/fat/elf.c 
MIN_DLTSH = obj/mdltsh.o
MIN_DLTSH_SRC = tty/min_dltsh.c
INTS_C = obj/ints_c.o 
INTS_C_SRC = ints/int.c 
MOUSE = obj/mouse.o 
MOUSE_SRC = drvs/mouse.c 

# clang flags for build on i386
FLAGS = -m32 -march=i386 -std=c17 \
		-fno-pie -fno-builtin -fno-stack-protector -fno-exceptions \
		-ffreestanding -nostdlib \
		-Wall -Wextra -Wstrict-prototypes \
		-mno-sse -mno-lsx -mno-80387

LD_FLAGS = $@ $^ -z noexecstack -T link.ld -Map kernel.map

# image file
img = img/open-delta.img

# qemu
QEMU = qemu-system-x86_64
QEMU_FLAGS = -boot c -m 1024 -smp 1 -vga vmware -s \
			 -d int,cpu_reset \
			 -no-reboot -no-shutdown 

all: run

$(BOOT): $(BOOT_SRC)
	$(AS) $< -f bin -o $@

$(ENTRY): $(ENTRY_SRC)
	$(AS) $< -f elf32 -o $@

$(GDT_ASM): $(GDT_ASM_SRC)
	$(AS) $< -f elf32 -o $@

$(IDT_ASM): $(IDT_ASM_SRC)
	$(AS) $< -f elf32 -o $@

$(INTS_ASM): $(INTS_ASM_SRC)
	$(AS) $< -f elf32 -o $@

$(TOOLS): $(TOOLS_SRC)
	$(AS) $< -f elf32 -o $@

$(KERNEL_ELF): $(ENTRY) $(TOOLS) $(INTS_C) \
	$(STDLIB) $(ELF) \
	$(KERNEL_OBJ) $(STDBASE) \
	$(MIN_DLTSH) $(PIC) \
	$(SCREEN) $(PORTS) \
	$(IDT) $(IDT_ASM) \
	$(ISR) $(INTS_ASM) \
	$(STRING) $(MEM) \
	$(TYPES) $(TTY) \
	$(CTYPE) $(GDT_ASM) \
	$(GDT) $(SHM) $(FS) \
	$(LIST) $(PIPE) $(FPU) \
	$(SYS) $(TASK) $(PROC) \
	$(DISK) $(FAT) $(MBR) $(MOUSE)
	$(LD) -m elf_i386 -s -o $(LD_FLAGS) 

$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $(KERNEL_ELF) $(KERNEL_BIN)

$(MEM): $(MEM_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(SHM): $(SHM_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(FAT): $(FAT_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(DISK): $(DISK_SRC)
	$(CC) $(FLAGS) -c $< -o $@ 

$(MBR): $(MBR_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(PIC): $(PIC_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(GDT): $(GDT_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(MOUSE): $(MOUSE_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(ELF): $(ELF_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(IDT): $(IDT_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(INTS_C): $(INTS_C_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(MIN_DLTSH): $(MIN_DLTSH_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(TASK): $(TASK_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(PROC): $(PROC_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(SYS): $(SYS_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(ISR): $(ISR_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(TTY): $(TTY_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(PIPE): $(PIPE_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(FPU):  $(FPU_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(LIST): $(LIST_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(FS): $(FS_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(CTYPE): $(CTYPE_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(STRING): $(STRING_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(STDBASE): $(STDBASE_SRS)
	$(CC) $(FLAGS) -c $< -o $@

$(STDLIB): $(STDLIB_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(TYPES): $(TYPES_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(SCREEN): $(SCREEN_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(PORTS): $(PORTS_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(KERNEL_OBJ): $(SRCS)
	$(CC) $(FLAGS) -c $< -o $@

$(img): $(BOOT) $(KERNEL_BIN)
	dd if=/dev/zero of=$@ bs=512 count=4096 status=none
	dd if=$(BOOT) of=$@ conv=notrunc bs=512 count=1
	dd if=$(KERNEL_BIN) of=$@ conv=notrunc seek=1 bs=512

run: img/open-delta.img
	$(QEMU) $(QEMU_FLAGS) -drive file=$(img),format=raw,if=ide,media=disk

clean:
	rm -f $(ENTRY) $(KERNEL_OBJ) $(SCREEN) $(PORTS)    \
		  $(STDBASE) $(TYPES) $(MEM) $(STRING) $(IDT)   \
		  $(IDT_ASM) $(INTS_ASM) $(ISR) $(TTY) $(CTYPE)  \
		  $(GDT_ASM) $(GDT) $(SHM) $(FS) $(LIST) $(PIPE)  \
		  $(FPU) $(TASK) $(SYS) $(PROC) $(PIC) $(TOOLS)    \
		  $(FAT) $(DISK) $(MBR) $(STDLIB) $(ELF)            \
		  $(MIN_DLTSH) $(INTS_C) $(MOUSE)
	rm -f $(BOOT) $(KERNEL_BIN) $(KERNEL_ELF) $(img)
	rm -f kernel.map

.PHONY: $(BOOT) $(KERNEL_BIN) $(img) run clean
