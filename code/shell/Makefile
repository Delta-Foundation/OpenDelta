all: build

build: src/term.c src/commands.c src/files.c src/dltsh.c src/simple_comms.c
	clang src/term.c src/commands.c src/files.c src/dltsh.c src/simple_comms.c -o bin/dltsh
	clang++ src/help_table.cpp -o bin/table -lncursesw
	rustc src/dexide.rs -o bin/dexide
	rustc src/calc.rs -o bin/calc
	rustc -O src/clocks.rs -o ./bin/clock

run:
	./bin/dltsh

clean:
	rm -f bin/dltsh

.PHONY: build run clean all
