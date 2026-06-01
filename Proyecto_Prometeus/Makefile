CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c11 -Iinclude
TARGET  = sistema
SRCS    = main.c src/nodo.c src/usuario.c src/auth.c \
          src/filesystem.c src/permisos.c src/utils.c \
          src/persistencia.c
OBJS    = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run
