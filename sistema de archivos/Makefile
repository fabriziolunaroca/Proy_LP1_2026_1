CC=gcc

CFLAGS=-Iinclude

SRC=src/nodo.c \
    src/utils.c \
    src/auth.c \
    src/filesystem.c \
    src/permisos.c

all:
	$(CC) main.c $(SRC) $(CFLAGS) -o sistema

run:
	./sistema
