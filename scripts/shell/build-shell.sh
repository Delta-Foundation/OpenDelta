#!/usr/bin/env bash

function build {
    cd ~/open-delta/kernel/shell/
    clang src/term.c src/files.c src/dltsh.c src/commands.c src/simple_comms.c -o bin/dltsh
    clang++ src/help_table.cpp -o bin/table -lncursesw
	rustc src/dexide.rs -o bin/dexide
    rustc src/calc.rs -o bin/calc
    rustc src/clocks.rs -o bin/clocks
}

function run {
    ./bin/term
}

function clean {
    echo "removing binaries"
    rm -f bin/dltsh 
        \ bin/dexide bin/calc 
        \ bin/clocks bin/table
}

build && run
