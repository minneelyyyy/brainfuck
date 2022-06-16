
CC=cc
CFLAGS=-O2 -Wall -Werror
SOURCES=src/brainfuck.c

.PHONY: all clean install uninstall

all: brainfuck

brainfuck: $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm brainfuck

install:
	cp brainfuck /usr/bin/brainfuck

uninstall:
	rm /usr/bin/brainfuck