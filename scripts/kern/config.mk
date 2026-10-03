include ~/OpenDelta/scripts/kern/make/binaries.mk
include ~/OpenDelta/scripts/kern/make/binaries.mk

# asm translator
AS=nasm

# build tools
CC = clang
CXX = clang++
LD = ld.lld
OBJCOPY = llvm-objcopy

# Compiler flags for build on i386
FLAGS = --target=x86_64-unknown-elf -m64 -std=c17 \
		-ffreestanding -nostdlib \
		-fno-builtin -fno-stack-protector -nodefaultlibs \
        -O2 -g -Wall

# Linker flags 
LD_FLAGS = -z noexecstack -T link64.ld -Map kernel.map

# Image file
IMG = img/open-delta.img

# QEMU and QEMU flags 
QEMU = qemu-system-x86_64
QEMU_FLAGS = -boot c -m 1024 -smp 1 -vga vmware -s \
			 -d int,cpu_reset \
			 -no-reboot -no-shutdown \
             -display gtk,full-screen=off,gl=off,show-cursor=on

