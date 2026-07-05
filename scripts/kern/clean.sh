#!/usr/bin/bash

cd ~/OpenDelta/kernel/

function clean {
    echo "clean .obj binaries"
    rm -f obj/kernel.o obj/stdbase.o obj/idt.o obj/mem.o \ 
        obj/string.o obj/types.o obj/screen.o obj/gdt.o \ 
        obj/gdtasm.o obj/intsasm.o obj/isr.o obj/tty.o \
        obj/ctype.o obj/ports.o obj/entry.o \
        obj/shm.o obj/fs.o obj/list.o obj/pipe.o \
        obj/idtasm.o obj/pic.o obj/fpu.o obj/sys.o \
        obj/proc.o obj/task.o obj/speaker.o obj/tools.o  \
        obj/fat.o obj/disk.o obj/mbr.o obj/elf.o \
        obj/min_dltsh.o obj/ints.o obj/mouse.o obj/time.o

    echo "clean kernel.map"
    rm -f kernel.map

    echo "clean .bin and .img binaries"
    rm -f img/kernel.bin img/kernel.elf \ 
        img/boot.bin img/open-delta.img
}

clean
