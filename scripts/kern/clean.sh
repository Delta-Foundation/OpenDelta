#!/usr/bin/bash

cd ~/OpenDelta/code/kernel/

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

function clean {
    echo "clean .obj binaries"
    rm -f "${OBJS[@]}"

    echo "clean kernel.map"
    rm -f kernel.map

    echo "clean .bin and .img binaries"
    rm -f img/kernel.bin img/kernel.elf img/boot.bin img/open-delta.img
}

clean
