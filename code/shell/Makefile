all: build

C_SOURCES = src/term.c \
		    src/commands.c \
		    src/files.c \
		    src/simple_comms.c \
			src/dltsh.c \
		    src/editor.c \

build: src/term.c src/commands.c src/files.c src/dltsh.c src/simple_comms.c
	clang $(C_SOURCES) -o ./bin/dltsh
	clang++ src/help_table.cpp -o ./bin/table -lncursesw
	rustc src/dexide.rs -o ./bin/dexide
	rustc src/calc.rs   -o ./bin/calc
	rustc src/clocks.rs -o ./bin/clocks

run:
	./bin/dltsh

clean:
	rm -f bin/dltsh

.PHONY: build run clean all
