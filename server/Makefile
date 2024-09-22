CC ?= clang

CFLAGS := -I libs -Wall -Wextra -Werror -Wpedantic -std=c99 -O2 -lpthread

all:
	$(CC) main.c libs/*.c $(CFLAGS) -o server