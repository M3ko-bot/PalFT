CC ?= clang

CFLAGS := -I libs -Wall -Wextra -Werror -Wpedantic -std=c99 -O2 -lpthread -I /usr/include/postgresql -lpq

all:
	$(CC) main.c libs/*.c $(CFLAGS) -o server