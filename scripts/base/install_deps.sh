#!/usr/bin/bash

# !NO DEBIAN AND APT

clear

function kernel_deps {
    while true; do 
        echo "choose your distro group"
        echo "1.arch, 2.void"
        read group

        if [ "$group" == "1" ]; then
            sudo pacman -S clang llvm nasm make
            sudo pacman -S qemu-system-i386 qemu-common qemu-firmware 
            sudo pacman -S lld git
            break
        elif [ "$group" == "2" ]; then
            sudo xbps-install -S clang llvm nasm make
            sudo xbps-install -S git lld
            sudo xbps-install -S qemu-system-i386 qemu-common qemu-firmware
            break
        else
            echo "choose only 1 or 2"
        fi
    done
}

function shell_deps {
    while true; do 
        echo "choose your distro group"
        echo "1.arch, 2.void"
        read group 

        if [ "$group" == "1" ]; then
            sudo pacman -S rust cargo
            sudo pacman -S ncurses ncurses-devel ncurses-libs
        elif [ "$group" == "2" ]; then
            sudo xbps-install -S rust cargo
            sudo xbps-install -S ncurses ncurses-devel ncurses-libs
        else
            echo "choose only 1 or 2"
        fi 
    done
}

function main {
    echo "install kernel dependencies"
    kernel_deps

    echo "install shell dependencies"
    shell_deps
}
