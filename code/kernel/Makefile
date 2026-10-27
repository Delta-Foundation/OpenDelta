include ../../scripts/kern/make/binaries.mk
include ../../scripts/kern/make/sources.mk
include ../../scripts/kern/config.mk

all: 
	$(MAKE) $(BOOT_MAIN)
	$(MAKE) $(BOOT_STAGE2)
	$(MAKE) $(KERNEL_ELF)
	$(MAKE) $(IMG)

$(KERNEL_ELF): $(OBJS)
	$(LD) -m elf_x86_64 -o $@ $(OBJS) $(LD_FLAGS)

$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $(KERNEL_ELF) $(KERNEL_BIN)

# =====================
# | Building ASM code |
# =====================
$(BOOT_MAIN): $(BOOT_DEPS)
	$(AS) -f bin $< -o $@

$(BOOT_STAGE2): $(STAGE2_DEPS)
	$(AS) -f bin $< -o $@

$(ENTRY): $(ENTRY_SRC)
	$(AS) $< -f elf64 -o $@

$(GDT_ASM): $(GDT_ASM_SRC)
	$(AS) $< -f elf64 -o $@

$(IDT_ASM): $(IDT_ASM_SRC)
	$(AS) $< -f elf64 -o $@

$(INTS_ASM): $(INTS_ASM_SRC) cpu/asm/isr.asm cpu/asm/irq.asm
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

obj/%.o: libc/src/%.c
	$(CC) $(FLAGS) -c $< -o $@

libc/src/%.o: libc/src/%.c
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

$(CINIT): $(CINIT_SRC)
	$(CC) $(FLAGS) -c $< -o $@

# =====================
# | Creating OS image |
# =====================
$(IMG): $(BOOT_MAIN) $(BOOT_STAGE2) $(KERNEL_BIN)
	@size=$$(stat -c %s $(KERNEL_BIN)); max=$$(( $(KERNEL_SECTORS) * 512 )); \
	if [ $$size -gt $$max ]; then \
		echo "error: $(KERNEL_BIN) is $$size bytes, bootloader limit is $$max bytes" >&2; \
		exit 1; \
	fi
	dd if=/dev/zero of=$@ bs=512 count=4096 status=none
	dd if=$(BOOT_MAIN) of=$@ conv=notrunc bs=512 count=1 status=none
	dd if=$(BOOT_STAGE2) of=$@ conv=notrunc bs=512 count=5 seek=1 status=none
	dd if=$(KERNEL_BIN) of=$@ conv=notrunc bs=512 count=$(KERNEL_SECTORS) seek=6

# Running
run: img/open-delta.img
	$(QEMU) $(QEMU_FLAGS) -drive file=$(IMG),format=raw,if=ide,media=disk

# =========================
# | Removing binary files |
# =========================
clean:
	~/OpenDelta/scripts/kern/sh/clean.sh

.PHONY: $(BOOT) $(KERNEL_BIN) $(img) run clean
