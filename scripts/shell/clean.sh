#!/usr/bin/env bash

BINARIES=(
    bin/dltsh 
    bin/table 
    bin/manual 
    bin/dexide
    bin/calc 
    bin/clocks
)

function clean {
    rm -f "${BINARIES[@]}"
}
