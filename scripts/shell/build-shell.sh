#!/usr/bin/env bash

C_SOURCES=(
    src/term.c
    src/commands.c
    src/files.c
    src/dltsh.c
    src/editor.c
    src/simple_comms.c
)

if [ ! -d "~/OpenDelta/code/shell/bin" ]; then
    mkdir -p ~/OpenDelta/code/shell/bin
fi

function build {
    cd ~/OpenDelta/code/shell/
    clang "${C_SOURCES[@]}" -o ./bin/dltsh
    clang++ src/help_table.cpp -o ./bin/table -lncursesw
    rustc src/dexide.rs -o ./bin/dexide
    rustc src/calc.rs -o ./bin/calc
    rustc src/clocks.rs -o ./bin/clocks
}

function run {
    ./bin/dltsh
}

function clean {
    rm -f ./bin/dltsh
    rm -f ./bin/table
    rm -f ./bin/dexide
    rm -f ./bin/calc
    rm -f ./bin/clocks
}

build && run
