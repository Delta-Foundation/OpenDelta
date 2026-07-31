# asm translator
AS=nasm

# build tools
CC = clang
CXX = clang++
LD = ld.lld
OBJCOPY = llvm-objcopy

# source code
BOOT_SRC = boot/x86_64/boot.asm
ENTRY_SRC = kernel_entry.asm
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
STDBASE_SRC = lib/source/stdbase.c
STDLIB_SRC = lib/source/stdlib.c
CTYPE_SRC = lib/source/ctype.c
TYPES_SRC = lib/source/types.c
STRING_SRC = lib/source/string.c
TTY_SRC = tty/tty.c
MIN_DLTSH_SRC = tty/min_dltsh.c
TOOLS_SRC = tools/fat/asm/tools.asm
FAT_SRC = tools/fat/fat.c
ELF_SRC = tools/fat/elf.c
MBR_SRC = tools/fat/mbr.c
DISK_SRC = tools/fat/disk.c
TOOLS_SRC = tools/fat/asm/tools.asm

# Variables for freestanding binary files 
BOOT = img/boot.bin
KERNEL_ELF = img/kernel.elf
KERNEL_BIN = img/kernel.bin

# Variables for .obj binary files
ENTRY = obj/entry.o
KERNEL_OBJ = obj/kernel.o
GDT = obj/gdt.o
GDT_ASM = obj/gdt_asm.o
IDT_ASM = obj/idta.o
INTS_ASM = obj/ints.o
IDT = obj/idt.o 
ISR = obj/isr.o
INTS_C = obj/ints_c.o 
PIC = obj/pic.o 
HAL = obj/hal.o 
PORTS = obj/ports.o 
FPU = obj/fpu.o
SCREEN = obj/screen.o 
SPEAKER = obj/spkr.o
MOUSE = obj/mouse.o 
KEYBOARD = obj/kbd.o 
MEM = obj/mem.o 
SHM = obj/shm.o 
SYS = obj/sys.o 
TASK = obj/task.o 
PROC = obj/proc.o 
FS = obj/fs.o 
LIST = obj/list.o 
PIPE = obj/pipe.o 
STDBASE = obj/stdbase.o  
STDLIB = obj/stdlib.o 
CTYPE = obj/ctype.o 
TYPES = obj/types.o 
STRING = obj/string.o
TTY = obj/tty.o 
MIN_DLTSH = obj/mdltsh.o 
FAT = obj/fat.o 
ELF = obj/elf.o 
MBR = obj/mbr.o 
DISK = obj/disk.o
TOOLS = obj/toolsa.o

# .obj for .elf kernel binary 
OBJS = $(ENTRY) \
	   $(KERNEL_OBJ) \
	   $(GDT_ASM) \
	   $(GDT) \
	   $(IDT_ASM) \
	   $(INTS_ASM) \
	   $(IDT) \
	   $(ISR) \
	   $(INTS_C) \
	   $(PIC) \
	   $(HAL) \
	   $(PORTS) \
	   $(FPU) \
       $(SCREEN) \
	   $(SPEAKER) \
	   $(MOUSE) \
	   $(KEYBOARD) \
	   $(MEM) \
	   $(SHM) \
	   $(SYS) \
	   $(TASK) \
	   $(PROC) \
	   $(FS) \
	   $(LIST) \
	   $(PIPE) \
	   $(STDBASE) \
	   $(STDLIB) \
	   $(CTYPE) \
	   $(TYPES) \
	   $(STRING) \
	   $(TTY) \
	   $(MIN_DLTSH) \
	   $(FAT) \
	   $(ELF) \
	   $(MBR) \
	   $(DISK) \
	   $(TOOLS)

# Compiler flags for build on i386
FLAGS = --target=x86_64-unknown-elf -m64 -std=c17 \
		-ffreestanding -nostdlib \
		-fno-builtin -fno-stack-protector -nodefaultlibs \
        -O2 -g -Wall

# Linker flags 
LD_FLAGS = -z noexecstack -T link64.ld -Map kernel.map

# Image file
IMG = img/open-delta.img

# QEMU and QEMU flags 
QEMU = qemu-system-x86_64
QEMU_FLAGS = -boot c -m 1024 -smp 1 -vga vmware -s \
			 -d int,cpu_reset \
			 -no-reboot -no-shutdown \
             -display gtk,full-screen=off,gl=off,show-cursor=on

all: run

$(KERNEL_ELF): $(OBJS)
	$(LD) -m elf_x86_64 -o $@ $(OBJS) $(LD_FLAGS)

$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $(KERNEL_ELF) $(KERNEL_BIN)

# =====================
# | Building ASM code |
# =====================
$(BOOT): $(BOOT_SRC)
	$(AS) $< -f bin -o $@

$(ENTRY): $(ENTRY_SRC)
	$(AS) $< -f elf64 -o $@

$(GDT_ASM): $(GDT_ASM_SRC)
	$(AS) $< -f elf64 -o $@

$(IDT_ASM): $(IDT_ASM_SRC)
	$(AS) $< -f elf64 -o $@

$(INTS_ASM): $(INTS_ASM_SRC)
	$(AS) $< -f elf64 -o $@

$(TOOLS): $(TOOLS_SRC)
	$(AS) $< -f elf64 -o $@

# ===================
# | Building C code |
# ===================
$(KERNEL_OBJ): $(KERNEL_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(GDT): $(GDT_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(IDT): $(IDT_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(ISR): $(ISR_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(INTS_C): $(INTS_C_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(PIC): $(PIC_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(HAL): $(HAL_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(PORTS): $(PORTS_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(FPU): $(FPU_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(SCREEN): $(SCREEN_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(SPEAKER): $(SPEAKER_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(MOUSE): $(MOUSE_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(KEYBOARD): $(KEYBOARD_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(MEM): $(MEM_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(SHM): $(SHM_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(SYS): $(SYS_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(TASK): $(TASK_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(PROC): $(PROC_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(FS): $(FS_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(LIST): $(LIST_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(PIPE): $(PIPE_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(STDBASE): $(STDBASE_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(STDLIB): $(STDLIB_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(CTYPE): $(CTYPE_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(TYPES): $(TYPES_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(STRING): $(STRING_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(TTY): $(TTY_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(MIN_DLTSH): $(MIN_DLTSH_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(FAT): $(FAT_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(ELF): $(ELF_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(MBR): $(MBR_SRC)
	$(CC) $(FLAGS) -c $< -o $@

$(DISK): $(DISK_SRC)
	$(CC) $(FLAGS) -c $< -o $@

# =====================
# | Creating OS image |
# =====================
$(IMG): $(BOOT) $(KERNEL_BIN)
	dd if=/dev/zero of=$@ bs=512 count=4096 status=none
	dd if=$(BOOT) of=$@ conv=notrunc bs=512 count=1
	dd if=$(KERNEL_BIN) of=$@ conv=notrunc seek=1 bs=512

# Running
run: img/open-delta.img
	$(QEMU) $(QEMU_FLAGS) -drive file=$(IMG),format=raw,if=ide,media=disk

# =========================
# | Removing binary files |
# =========================
clean:
	rm -f $(ENTRY) $(KERNEL_OBJ) $(SCREEN) $(PORTS)    \
		  $(STDBASE) $(TYPES) $(MEM) $(STRING) $(IDT)   \
		  $(IDT_ASM) $(INTS_ASM) $(ISR) $(TTY) $(CTYPE)  \
		  $(GDT_ASM) $(GDT) $(SHM) $(FS) $(LIST) $(PIPE)  \
		  $(FPU) $(TASK) $(SYS) $(PROC) $(PIC) $(TOOLS)    \
		  $(FAT) $(DISK) $(MBR) $(STDLIB) $(ELF) $(HAL)     \
		  $(MIN_DLTSH) $(INTS_C) $(MOUSE) $(SPEAKER) $(KEYBOARD)
	rm -f $(BOOT) $(KERNEL_BIN) $(KERNEL_ELF) $(IMG)
	rm -f kernel.map

.PHONY: $(BOOT) $(KERNEL_BIN) $(img) run clean
