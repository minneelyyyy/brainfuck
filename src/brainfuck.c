#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

ssize_t loop_size = 0;
size_t loop_level[512] = {0};

void push_back(size_t sz) {
	if (loop_size + 1 > sizeof(loop_level)) {
		fprintf(stderr, "brainfuck: error: too many loops\n");
		exit(1);
	}

	loop_level[loop_size++] = sz;
}

size_t pop_back() {
	if (loop_size - 1 < 0) {
		fprintf(stderr, "brainfuck: error: unexpected end of loop\n");
		exit(1);
	}

	return loop_level[--loop_size];
}

size_t get() {
	if (loop_size - 1 < 0) {
		fprintf(stderr, "brainfuck: error: unexpected end of loop\n");
		exit(1);
	}

	return loop_level[loop_size - 1];
}

int main(int argc, char *argv[]) {
	char array[30000] = {0};
	char *ptr = array;
	char *program;
	int fd;
	size_t i;
	char c;
	struct stat st;

	if (argc <= 1) {
		fprintf(stderr, "USAGE: brainfuck <file>\n");
		exit(1);
	}

	fd = open(argv[1], O_RDONLY);

	if (fd < 0) {
		fprintf(stderr, "brainfuck: error: failed to open file %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	fstat(fd, &st);
	program = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	for (i = 0; i < st.st_size; i++) {
		c = program[i];

		if (ptr < array || array + sizeof(array) < ptr) {
			fprintf(stderr, "brainfuck: error: pointer is not within its allocated range (%lu bytes)\n", sizeof(array));
			exit(1);
		}

		switch (c) {
			case '>':
				++ptr;
			break;

			case '<':
				--ptr;
			break;

			case '+':
				++(*ptr);
			break;

			case '-':
				--(*ptr);
			break;

			case '.':
				putchar(*ptr);
			break;

			case ',':
				*ptr = getchar();
			break;

			case '[':
				push_back(i);
			break;

			case ']': {
				size_t home = get();
				
				if (*ptr) {
					i = home;
				}
				else {
					pop_back();
				}
			}
		}
	}

	close(fd);
	munmap(program, st.st_size);

	if (loop_size > 0) {
		fprintf(stderr, "brainfuck: error: unclosed loop\n");
		exit(1);
	}

	return 0;
}
