//! @file utils.h
//! @brief Funciones utilitarias multiplataforma

#ifndef UTILS_H
#define UTILS_H

//! @name Entrada de datos
//! @{

//! @brief Lee una línea de texto desde stdin con prompt
//! @param prompt Texto a mostrar antes de leer
//! @param buf Buffer donde se almacenará el texto
//! @param size Tamaño máximo del buffer
void leerLinea(const char *prompt, char *buf, int size);

//! @brief Lee una contraseña sin mostrar caracteres en pantalla
//! @param prompt Texto a mostrar antes de leer
//! @param buf Buffer donde se almacenará la contraseña
//! @param size Tamaño máximo del buffer
//! @details Muestra asteriscos (*) en lugar de los caracteres reales
void leerPassword(const char *prompt, char *buf, int size);

//! @}

//! @name Utilidades de consola
//! @{

//! @brief Limpia el buffer del teclado (stdin)
void limpiarBuffer(void);

//! @brief Limpia la pantalla de la terminal/consola
//! @details Funciona en Windows (cls) y Unix/Linux/Mac (ANSI)
void limpiarPantalla(void);

//! @}

#endif
