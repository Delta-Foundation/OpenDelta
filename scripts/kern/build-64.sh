#!/usr/bin/bash


CFLAGS=(
    --target=x86_64
    -std=c17
    -m64
    -fno-pie
    -fno-builtin
    -fno-stack-protector
    -nostdlib
    -nodefaultlibs
    -ffreestanding
    -O0
    -g
    -Wall
)

OBJS=(
    obj/kernel.o
    obj/gdtasm.o
    obj/gdt.o
    obj/idtasm.o
    obj/intsasm.o
    obj/idt.o
    obj/isr.o
    obj/ints.o
    obj/pic.o
    obj/hal.o
    obj/ports.o
    obj/fpu.o
    obj/screen.o
    obj/speaker.o
    obj/mouse.o
    obj/time.o
    obj/kbd.o
    obj/mem.o
    obj/shm.o
    obj/sys.o
    obj/task.o
    obj/proc.o
    obj/fs.o
    obj/list.o
    obj/pipe.o
    obj/stdbase.o
    obj/stdlib.o
    obj/ctype.o
    obj/types.o
    obj/string.o
    obj/tty.o
    obj/min_dltsh.o
    obj/fat.o
    obj/elf.o
    obj/mbr.o
    obj/disk.o
)

function base-actions {
    cd ~/OpenDelta/code/kernel/
    if [! -d "./img" ]; then
        mkdir -p ./img/
    elif [! -d "./obj" ]; then
        mkdir -p ./obj/
    fi
}

function build-kern-64 {
    echo "#---building-asm-code---#"
    nasm boot/x86_64/boot.asm    -f bin -o img/boot.bin
    nasm kernel_entry.asm        -f elf -o obj/entry.o
    nasm arch/gdt/gdt.asm        -f elf -o obj/gdtasm.o
    nasm cpu/asm/ints.asm        -f elf -o obj/intsasm.o
    nasm cpu/asm/idt.asm         -f elf -o obj/idtasm.o
    nasm tools/fat/asm/tools.asm -f elf -o obj/tools.o

    echo "#---building-c-code---#"

    # Main file
    clang "${CFLAGS[@]}" -c kernel.c           -o obj/kernel.o

    # GDT, IDT, ISR, IRQ, PIC, HAL, FPU
    clang "${CFLAGS[@]}" -c arch/gdt/gdt.c     -o obj/gdt.o
    clang "${CFLAGS[@]}" -c cpu/idt.c          -o obj/idt.o
    clang "${CFLAGS[@]}" -c cpu/isr.c          -o obj/isr.o
    clang "${CFLAGS[@]}" -c ints/int.c         -o obj/ints.o
    clang "${CFLAGS[@]}" -c drvs/pic.c         -o obj/pic.o
    clang "${CFLAGS[@]}" -c hal/hal.c          -o obj/hal.o
    clang "${CFLAGS[@]}" -c ports/ports.c      -o obj/ports.o
    clang "${CFLAGS[@]}" -c fpu/fpu.c          -o obj/fpu.o

    # Drivers
    clang "${CFLAGS[@]}" -c drvs/screen.c      -o obj/screen.o
    clang "${CFLAGS[@]}" -c drvs/speaker.c     -o obj/speaker.o
    clang "${CFLAGS[@]}" -c drvs/mouse.c       -o obj/mouse.o
    clang "${CFLAGS[@]}" -c drvs/time.c        -o obj/time.o
    clang "${CFLAGS[@]}" -c drvs/keyboard.c    -o obj/kbd.o

    # Memory
    clang "${CFLAGS[@]}" -c mem/memory.c        -o obj/mem.o
    clang "${CFLAGS[@]}" -c mem/shared_memory.c -o obj/shm.o

    # Syscalls, tasks, processes
    clang "${CFLAGS[@]}" -c syscall/syscall.c  -o obj/sys.o
    clang "${CFLAGS[@]}" -c syscall/task.c     -o obj/task.o
    clang "${CFLAGS[@]}" -c syscall/proc.c     -o obj/proc.o

    # File System
    clang "${CFLAGS[@]}" -c fs/fs.c             -o obj/fs.o
    clang "${CFLAGS[@]}" -c fs/list.c           -o obj/list.o
    clang "${CFLAGS[@]}" -c fs/pipe.c           -o obj/pipe.o

    # Kernel libc
    clang "${CFLAGS[@]}" -c lib/source/stdbase.c -o obj/stdbase.o
    clang "${CFLAGS[@]}" -c lib/source/stdlib.c  -o obj/stdlib.o
    clang "${CFLAGS[@]}" -c lib/source/ctype.c   -o obj/ctype.o
    clang "${CFLAGS[@]}" -c lib/source/types.c   -o obj/types.o
    clang "${CFLAGS[@]}" -c lib/source/string.c  -o obj/string.o

    # TTY and Minimal dltsh
    clang "${CFLAGS[@]}" -c tty/tty.c -o obj/tty.o
    clang "${CFLAGS[@]}" -c tty/min_dltsh.c -o obj/min_dltsh.o

    # Experimental
    clang "${CFLAGS[@]}" -c tools/fat/fat.c -o obj/fat.o
    clang "${CFLAGS[@]}" -c tools/fat/elf.c -o obj/elf.o
    clang "${CFLAGS[@]}" -c tools/fat/mbr.c -o obj/mbr.o
    clang "${CFLAGS[@]}" -c tools/fat/disk.c -o obj/disk.o

    echo "#---creating-kernel-elf-binary---#"
    ld.lld -m elf_x86_64 -z noexecstack -T link64.ld -Map kernel.map "${OBJS[@]}"

    echo "#---creating-kernel-binary---#"
    llvm-objcopy -O binary img/kernel.elf img/kernel.bin

    echo "#---creating-os-image---#"
    dd if=/dev/zero of=img/open-delta.img bs=512 count=4096 status=none
    dd if=img/boot.bin of=img/open-delta.img bs=512 count=1 conv=notrunc
    dd if=img/kernel.bin of=img/open-delta.img conv=notrunc seek=1 bs=512
}

function run {
    qemu-system-x86_64 -boot c -m 1024 -smp 1 \
        -vga vmware -s -d int,pcall,cpu_reset \
        -drive file=img/open-delta.img,format=raw,if=ide,media=disk
}

base-actions
build-kern-64
run
