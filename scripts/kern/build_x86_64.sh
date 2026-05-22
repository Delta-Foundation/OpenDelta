#!/usr/bin/bash

cd ~/OpenDelta/kernel/

mkdir -p img/
mkdir -p obj/

function build_x86_64 {
    #---compile-asm-code---#
    nasm boot/i386/boot.asm -f bin -o img/boot.bin
    nasm kernel_entry.asm -f elf -o obj/entry.o
    nasm arch/gdt/gdt.asm -f elf -o obj/gdtasm.o
    nasm cpu/asm/ints.asm -f elf -o obj/intsa.o # intsa - interrupts asm
    nasm cpu/asm/idt.asm -f elf -o obj/idta.o 
    nasm tools/fat/asm/tools.asm -f elf -o obj/tools.o

    #---compile-kernel---#
    clang -m64 -fno-pie -ffreestanding -nostdlib -c kernel.c -o obj/kernel.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c cpu/idt.c -o obj/idt.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c cpu/isr.c -o obj/isr.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c lib/source/stdbase.c -o obj/stdbase.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c lib/source/string.c -o obj/string.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c mem/memory.c -o obj/mem.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c ports/ports.c -o obj/ports.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c drvs/screen.c -o obj/screen.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c lib/source/types.c -o obj/types.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c tty/tty.c -o obj/tty.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c lib/source/ctype.c -o obj/ctype.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c arch/gdt/gdt.c -o obj/gdt.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c mem/shared_memory.c -o obj/shm.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c fs/list.c -o obj/list.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c fs/pipe.c -o obj/pipe.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c drvs/pic.c -o obj/pic.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c fpu/fpu.c -o obj/fpu.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c syscall/syscall.c -o obj/sys.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c tools/fat/fat.c -o obj/fat.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c tools/fat/disk.c -o obj/disk.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c tools/fat/mbr.c -o obj/mbr.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c tools/fat/elf.c -o obj/elf.o


    #---create-kernel-bin-file---#
    ld.lld -m elf_x86_64 -s obj/kernel.o obj/stdbase.o obj/idt.o obj/idta.o
        \ obj/mem.o obj/string.o obj/shm.o obj/fs.o obj/list.o 
        \ obj/pipe.o obj/types.o obj/screen.o  obj/gdt.o obj/gdtasm.o 
        \ obj/ints.o obj/isr.o obj/tty.o obj/ctype.o obj/ports.o 
        \ obj/entry.o obj/proc.o obj/sys.o obj/task.o obj/pic.o obj/fpu.o 
        \ obj/tools.o obj/fat.o obj/disk.o obj/mbr.o obj/elf.o
        \ -o img/kernel.elf -z noexecstack -T link64.ld

    llvm-objcopy -O binary img/kernel.elf img/kernel.bin 

    #---create-os-image---#
    dd if=/dev/zero of=img/open-delta.img bs=512 count=32516 status=none
    dd if=img/boot.bin of=img/open-delta.img conv=notruc bs=512 count=1
    dd if=img/kernel.bin of=img/open-delta.img conv=notruc bs=512 seek=2
    mkfs.fat -F12 img/open-delta.img
}

build_x86_64
