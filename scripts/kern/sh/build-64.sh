#!/usr/bin/bash

set -euo pipefail

KERNEL_DIR="$HOME/OpenDelta/code/kernel"

CFLAGS=(
    --target=x86_64-unknown-elf
    -m64
    -std=c17
    -ffreestanding
    -nostdlib
    -fno-builtin
    -fno-stack-protector
    -nodefaultlibs
    -O2
    -g
    -Wall
    -I"$KERNEL_DIR/libc/include"
)

function base-actions {
    cd "$KERNEL_DIR" || return
    mkdir -p img obj

    rm -f obj/*.o img/kernel.elf
}

function build-kern-64 {
    cd "$KERNEL_DIR"

    echo "#---building-asm-code---#"

    nasm boot/x86_64/main.asm          -f bin   -o img/boot.bin
    nasm boot/x86_64/second-stage.asm  -f bin   -o img/stage2.bin
    nasm kernel_entry.asm              -f elf64 -o obj/entry.o
    nasm arch/gdt/gdt.asm              -f elf64 -o obj/gdtasm.o
    nasm cpu/asm/ints.asm              -f elf64 -o obj/ints_asm.o
    nasm cpu/asm/idt.asm               -f elf64 -o obj/idt_asm.o
    nasm tools/fat/asm/tools.asm       -f elf64 -o obj/tools.o

    echo "#---building-c-code---#"

    declare -A seen=()
    local src base
    while IFS= read -r src; do
        base=${src##*/}
        base=${base%.c}

        if [[ -n "${seen[$base]:-}" ]]; then
            echo "error: duplicate object name '$base.o': ${seen[$base]} and $src" >&2
            exit 1
        fi
        seen[$base]=$src

        clang "${CFLAGS[@]}" -c "$src" -o "obj/$base.o"
    done < <(find . -name '*.c' -not -path './obj/*' -not -path './img/*' | sort)

    clang "${CFLAGS[@]}" -c ../cinit/main.c -o obj/main.o

    source "$HOME/OpenDelta/scripts/kern/sh/binaries.sh"

    echo "#---creating-kernel-elf-binary---#"
    ld.lld -m elf_x86_64 -z noexecstack \
        -T link64.ld \
        -Map kernel.map \
        -o img/kernel.elf \
        "${OBJS[@]}"

    echo "#---creating-kernel-binary---#"
    llvm-objcopy -O binary img/kernel.elf img/kernel.bin

    echo "#---creating-os-image---#"

    size=$(stat -c %s img/kernel.bin)
    max=$((256 * 512))

    if [ "$size" -gt "$max" ]; then
        echo "error: img/kernel.bin is $size bytes, bootloader limit is $max bytes" >&2
        exit 1
    fi

	dd if=/dev/zero of=img/open-delta.img bs=512 count=131072 status=none
	dd if=img/boot.bin of=img/open-delta.img conv=notrunc bs=512 count=1 status=none
	dd if=img/stage2.bin of=img/open-delta.img conv=notrunc bs=512 count=5 seek=1 status=none
	dd if=img/kernel.bin of=img/open-delta.img conv=notrunc bs=512 count=256 seek=6
	python3 ~/OpenDelta/scripts/kern/py/mkfat32.py img/open-delta.img
}

function run {
    qemu-system-x86_64 -boot c -m 1024 -smp 1 \
        -vga vmware -s -d int,pcall,cpu_reset \
        -drive file=img/open-delta.img,format=raw,if=ide,media=disk
}

base-actions
build-kern-64
# run
