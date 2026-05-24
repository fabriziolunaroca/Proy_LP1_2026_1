CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c11 -Iinclude
SRCS    = main.c src/nodo.c src/usuario.c src/auth.c \
          src/filesystem.c src/permisos.c src/utils.c \
          src/persistencia.c
OBJS    = $(SRCS:.c=.o)

# ── Detectar sistema operativo ──────────────────
ifeq ($(OS), Windows_NT)
    TARGET  = sistema.exe
    CFLAGS += -D_WIN32
    RM      = del /Q
else
    TARGET  = sistema
    RM      = rm -f
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run
