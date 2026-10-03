#!/usr/bin/bash

cd ~/OpenDelta/code/kernel/

source ~/OpenDelta/scripts/kern/sh/binaries.sh

function clean {
    echo "clean .obj binaries"
    rm -f "${OBJS[@]}"

    echo "clean kernel.map"
    rm -f kernel.map

    echo "clean .bin and .img binaries"
    rm -f "${BINS[@]}"
}

clean
