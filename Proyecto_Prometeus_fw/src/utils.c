//! @file utils.c
//! @brief Implementación de funciones utilitarias multiplataforma

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/utils.h"

#ifdef _WIN32
  #include <conio.h>
  #include <windows.h>
#else
  #include <termios.h>
  #include <unistd.h>
#endif

//! @name Utilidades de consola
//! @{

//! @brief Limpia el buffer del teclado (stdin)
//! @details Lee y descarta todos los caracteres hasta encontrar un salto de línea
void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

//! @brief Limpia la pantalla de la terminal/consola
//! @details Windows: system("cls") | Unix/Linux/Mac: secuencia ANSI
void limpiarPantalla(void) {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[H\033[J");
#endif
}

//! @}

//! @name Entrada de datos
//! @{

//! @brief Lee una línea de texto desde stdin con prompt
//! @param prompt Texto a mostrar antes de leer
//! @param buf Buffer donde se almacenará el texto
//! @param size Tamaño máximo del buffer
void leerLinea(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buf, size, stdin))
        buf[strcspn(buf, "\n")] = '\0';
}

//! @brief Lee una contraseña sin mostrar caracteres en pantalla
//! @param prompt Texto a mostrar antes de leer
//! @param buf Buffer donde se almacenará la contraseña
//! @param size Tamaño máximo del buffer
//! @details Muestra asteriscos (*) en lugar de los caracteres reales.
//!          Soporta tecla Backspace para borrar.
//!          Funciona en Windows (conio.h) y Unix (termios.h)
void leerPassword(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    fflush(stdout);

#ifdef _WIN32
    int i = 0;
    int c;
    while (i < size - 1) {
        c = _getch();
        if (c == '\r' || c == '\n') break;
        if (c == '\b') {
            if (i > 0) { i--; printf("\b \b"); fflush(stdout); }
        } else {
            buf[i++] = (char)c;
            printf("*"); fflush(stdout);
        }
    }
    buf[i] = '\0';
    printf("\n");
#else
    struct termios old, nuevo;
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
#endif
}

//! @}
