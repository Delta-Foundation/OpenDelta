#!/usr/bin/bash

CFLAGS=(
    --target=x86_64
    -std=c17 
    -m32 
    -march=x86_64
    -fno-pie 
    -fno-builtin 
    -fno-stack-protector
    -nostdlib 
    -nodefaultlibs 
    -ffreestanding
    -mno-sse 
    -mno-avx 
    -msoft-float 
    -mno-red-zone 
    -mno-80387
    -O0 
    -g 
    -Wall 
)

function base_actions {
    mkdir -p ~/open-delta/kernel/src/img/
    mkdir -p ~/open-delta/kernel/src/obj/
}

function build-kern-64 {
    cd ~/open-delta/kernel/src/
        
    echo "#---building-asm-code---#"
    nasm boot/x86_64/boot.asm      -f bin -o img/boot.bin
    nasm kernel_entry.asm        -f elf -o obj/entry.o 
    nasm arch/gdt/gdt.asm        -f elf -o obj/gdtasm.o 
    nasm cpu/asm/ints.asm        -f elf -o obj/intsasm.o
    nasm cpu/asm/idt.asm         -f elf -o obj/idtasm.o 
    nasm tools/fat/asm/tools.asm -f elf -o obj/tools.o

    echo "#---building-c-code---#"
    clang "${CFLAGS[@]}" -c kernel.c           -o obj/kernel.o
    clang "${CFLAGS[@]}" -c arch/gdt/gdt.c     -o obj/gdt.o
    clang "${CFLAGS[@]}" -c drvs/speaker.c     -o obj/speaker.o
    clang "${CFLAGS[@]}" -c drvs/mouse.c       -o obj/mouse.o
    clang "${CFLAGS[@]}" -c drvs/pic.c         -o obj/pic.o
    clang "${CFLAGS[@]}" -c drvs/screen.c      -o obj/screen.o
    clang "${CFLAGS[@]}" -c drvs/time.c        -o obj/time.o 
    clang "${CFLAGS[@]}" -c cpu/idt.c          -o obj/idt.o
    clang "${CFLAGS[@]}" -c cpu/isr.c          -o obj/isr.o
    clang "{$CFLAGS[@]}" -c ints/int.c         -o obj/ints.o
    clang "${CFLAGS[@]}" -c ports/ports.c      -o obj/ports.o
    clang "${CFLAGS[@]}" -c syscall/syscall.c  -o obj/sys.o
    clang "${CFLAGS[@]}" -c syscall/proc.c     -o obj/proc.o
    clang "${CFLAGS[@]}" -c syscall/task.c     -o obj/task.o
    clang "${CFLAGS[@]}" -c mem/memory.c       -o obj/mem.o 
    clang "${CFLAGS[@]}" -c mem/shared_memory.c -o obj/shm.o 
    clang "${CFLAGS[@]}" -c fpu/fpu.c           -o obj/fpu.o 
    clang "${CFLAGS[@]}" -c fs/fs.c             -o obj/fs.o 
    clang "${CFLAGS[@]}" -c fs/list.c           -o obj/list.o 
    clang "${CFLAGS[@]}" -c fs/pipe.c           -o obj/pipe.o
    clang "${CFLAGS[@]}" -c lib/source/stdbase.c -o obj/stdbase.o 
    clang "${CFLAGS[@]}" -c lib/source/stdlib.c  -o obj/stdlib.o
    clang "${CFLAGS[@]}" -c lib/source/string.c  -o obj/string.o 
    clang "${CFLAGS[@]}" -c lib/source/ctype.c   -o obj/ctype.o 
    clang "${CFLAGS[@]}" -c lib/source/types.c   -o obj/types.o 
    clang "${CFLAGS[@]}" -c tools/fat/fat.c -o obj/fat.o
    clang "${CFLAGS[@]}" -c tools/fat/elf.c -o obj/elf.o
    clang "${CFLAGS[@]}" -c tools/fat/mbr.c -o obj/mbr.o
    clang "${CFLAGS[@]}" -c tools/fat/disk.c -o obj/disk.o
    clang "${CFLAGS[@]}" -c tty/tty.c -o obj/tty.o 
    clang "${CFLAGS[@]}" -c tty/min_dltsh.c -o obj/min_dltsh.o 

    echo "#---creating-kernel-elf-binary---#"
    ld.lld -m elf_x86_64 -z noexecstack -T link.ld -Map kernel.map \
        obj/entry.o obj/kernel.o obj/gdt.o obj/gdtasm.o obj/speaker.o \
        obj/mouse.o obj/pic.o obj/screen.o obj/time.o \
        obj/idt.o obj/isr.o obj/ints.o obj/idtasm.o obj/intsasm.o \
        obj/ports.o obj/sys.o obj/proc.o obj/task.o obj/mem.o obj/shm.o \
        obj/fpu.o obj/fs.o obj/list.o obj/pipe.o \
        obj/stdbase.o obj/stdlib.o obj/string.o obj/ctype.o obj/types.o \
        obj/fat.o obj/elf.o obj/mbr.o obj/disk.o obj/tools.o \
        obj/tty.o obj/min_dltsh.o -o img/kernel.elf

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

base_actions
build-kern-32
run
