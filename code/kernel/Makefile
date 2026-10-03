include ~/OpenDelta/scripts/kern/make/binaries.mk
include ~/OpenDelta/scripts/kern/make/sources.mk
include ~/OpenDelta/scripts/kern/config.mk

all: 
	$(MAKE) $(BOOT)
	$(MAKE) $(KERNEL_ELF)
	$(MAKE) $(IMG)

$(KERNEL_ELF): $(OBJS)
	$(LD) -m elf_x86_64 -o $@ $(OBJS) $(LD_FLAGS)

$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $(KERNEL_ELF) $(KERNEL_BIN)

# =====================
# | Building ASM code |
# =====================
$(BOOT): $(BOOT_SRC)
	$(AS) -f bin $< -o $@

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
	dd if=$(BOOT) of=$@ conv=notrunc bs=512 count=1 status=none
	dd if=$(KERNEL_BIN) of=$@ conv=notrunc seek=1 bs=512

# Running
run: img/open-delta.img
	$(QEMU) $(QEMU_FLAGS) -drive file=$(IMG),format=raw,if=ide,media=disk

# =========================
# | Removing binary files |
# =========================
clean:
	rm -f $(ENTRY) $(KERNEL_OBJ) $(SCREEN) $(PORTS) \
		  $(STDBASE) $(TYPES) $(MEM) $(STRING) $(IDT) \
		  $(IDT_ASM) $(INTS_ASM) $(ISR) $(TTY) $(CTYPE) \
		  $(GDT_ASM) $(GDT) $(SHM) $(FS) $(LIST) $(PIPE) \
		  $(FPU) $(TASK) $(SYS) $(PROC) $(PIC) $(TOOLS) \
		  $(FAT) $(DISK) $(MBR) $(STDLIB) $(ELF) $(HAL) \
		  $(MIN_DLTSH) $(INTS_C) $(MOUSE) $(SPEAKER) \
		  $(KEYBOARD) $(STAGE2)
	rm -f $(BOOT) $(KERNEL_BIN) $(KERNEL_ELF) $(IMG)
	rm -f kernel.map

.PHONY: $(BOOT) $(KERNEL_BIN) $(img) run clean
