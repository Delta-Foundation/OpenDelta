#!/usr/bin/bash

cd ~/OpenDelta/kernel/

mkdir -p img/
mkdir -p obj/

function build_x86_64 {
    nasm boot/x86_64/boot.asm -f bin -o img/boot64.bin
    nasm kernel_entry.asm -f elf -o obj/entry.o
    nasm arch/gdt/gdt.asm -f elf -o obj/gdta.o

    clang -m64 -fno-pie -ffreestanding -nostdlib -c kernel.c -o obj/kernel.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c cpu/idt.c -o obj/idt.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c lib/source/stdbase.c -o obj/stdbase.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c lib/source/string.c -o obj/string.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c lib/source/types.c -o obj/types.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c lib/source/ctype.c -o obj/ctype.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c mem/memory.c -o obj/mem.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c ports/ports.c -o obj/ports.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c drvs/screen.c -o obj/screen.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c tty/tty.c -o obj/tty.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c arch/gdt/gdt.c -o obj/gdtc.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c mem/shared_memory.c -o obj/shm.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c fs/list.c -o obj/list.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c fs/pipe.c -o obj/pipe.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c fs/fs.c -o obj/fs.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c drvs/pic.c -o obj/pic.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c syscall/syscall.c -o obj/sys.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c syscall/task.c -o obj/task.o
    clang -m64 -fno-pie -ffreestanding -nostdlib -c syscll/proc.c -o obj/proc.o


    ld.lld -m elf_x64_64 -s obj/kernel.o 
        \ obj/entry.o obj/idt.o obj/mem.o 
        \ obj/shm.o obj/fs.o obj/list.o 
        \ obj/pipe.o obj/stdbase.o obj/tty.o 
        \ obj/ctype.o obj/gdtc.o obj/gdta.o 
        \ obj/string.o obj/types.o obj/ports.o 
        \ obj/sys.o obj/proc.o obj/task.o 
        \ obj/fpu.o obj/pic.o obj/screen.o 
        \ -o img/kernel.bin -z noexecstack -T link64.ld --oformat elf_x86_64

    dd if=/dev/zero/ of=img/open-delta.img bs=512 count=32516 status=none
    mkfas.fat -F32 img/open-delta.img
    dd if=img/boot.bin of=img/open-delta.img conv=ascii bs=1024 count=1
    dd if=img/kernel.bin of=img/open-delta.img conv=ascii bs=2048 count=1
}

build_x86_64
