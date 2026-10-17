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
    clang++ src/manual.cpp -o ./bin/manual -lncursesw 
    rustc src/dexide.rs -o ./bin/dexide
    rustc src/calc.rs -o ./bin/calc
    rustc src/clocks.rs -o ./bin/clocks
}

function run {
    ./bin/dltsh
}

build && run
