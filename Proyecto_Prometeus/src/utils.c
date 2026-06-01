#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "../include/utils.h"

void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void limpiarPantalla(void) {
    printf("\033[H\033[J");
}

void leerLinea(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buf, size, stdin)) {
        buf[strcspn(buf, "\n")] = '\0';
    }
}

void leerPassword(const char *prompt, char *buf, int size) {
    struct termios old, nuevo;
    printf("%s", prompt);
    fflush(stdout);

    tcgetattr(STDIN_FILENO, &old);
    nuevo = old;
    nuevo.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &nuevo);

    int i = 0;
    char c;
    while (i < size - 1 && read(STDIN_FILENO, &c, 1) == 1) {
        if (c == '\n' || c == '\r') break;
        if (c == 127 || c == '\b') {
            if (i > 0) { i--; printf("\b \b"); fflush(stdout); }
        } else {
            buf[i++] = c;
            printf("*"); fflush(stdout);
        }
    }
    buf[i] = '\0';
    printf("\n");

    tcsetattr(STDIN_FILENO, TCSANOW, &old);
}
