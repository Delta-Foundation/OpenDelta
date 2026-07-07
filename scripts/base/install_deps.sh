#!/usr/bin/bash

clear

function kernel_deps {
    while true; do 
        echo "choose your distro group"
        echo "1.arch, 2.void, 3.debian"
        read group

        if [ "$group" == "1" ]; then
            sudo pacman -S clang llvm nasm make
            sudo pacman -S qemu-system-x86 qemu-common qemu-base qemu-desktop
            sudo pacman -S lld git
            break
        elif [ "$group" == "2" ]; then
            sudo xbps-install -S clang llvm nasm make
            sudo xbps-install -S git lld
            sudo xbps-install -S qemu-system-i386 qemu-common qemu-firmware
            break
        elif [ "$group" == "3" ]; then 
            sudo apt install clang lld llvm nasm
            sudo apt install git make
            sudo apt install qemu-system qemu-system-x86
        else
            echo "choose only 1 or 2"
        fi
    done
}

function shell_deps {
    while true; do 
        echo "choose your distro group"
        echo "1.arch, 2.void, 3.debian"
        read group 

        if [ "$group" == "1" ]; then
            sudo pacman -S rustup cargo
            sudo pacman -S ncurses ncurses-devel ncurses-libs
            break 
        elif [ "$group" == "2" ]; then
            sudo xbps-install -S rustup cargo
            sudo xbps-install -S ncurses ncurses-devel ncurses-libs
            break 
        elif [ "$group" == "3" ]; then 
            sudo apt install clang rustup cargo
            sudo apt install ncurses 
            break
        else
            echo "choose only 1 or 2 or 3"
        fi 
    done
}

function main {
    echo "install kernel dependencies"
    kernel_deps

    echo "install shell dependencies"
    shell_deps
}

main
