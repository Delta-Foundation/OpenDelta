# Variables for freestanding binary files 
BOOT_MAIN = img/boot.bin
BOOT_STAGE2 = img/stage2.bin
KERNEL_ELF = img/kernel.elf
KERNEL_BIN = img/kernel.bin

# Variables for .obj binary files
ENTRY = obj/entry.o
KERNEL_OBJ = obj/kernel.o
GDT = obj/gdt.o
GDT_ASM = obj/gdt_asm.o
IDT_ASM = obj/idt_asm.o
INTS_ASM = obj/ints_asm.o
IDT = obj/idt.o 
ISR = obj/isr.o
INTS_C = obj/ints.o 
PIC = obj/pic.o 
HAL = obj/hal.o 
PORTS = obj/ports.o 
FPU = obj/fpu.o
SCREEN = obj/screen.o 
SPEAKER = obj/speaker.o
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
MIN_DLTSH = obj/min_dltsh.o 
FAT = obj/fat.o 
ELF = obj/elf.o 
MBR = obj/mbr.o 
DISK = obj/disk.o
TOOLS = obj/tools_asm.o

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
