//! @file permisos.c
//! @brief Implementación del sistema de permisos y control de acceso

#include <stddef.h>
#include "../include/permisos.h"

//! @name Funciones de permisos
//! @{

//! @brief Verifica si un usuario tiene permiso para acceder a un directorio
//! @param destino Directorio al que se quiere acceder
//! @param userDir Directorio raíz personal del usuario (su "home")
//! @param usuario Usuario que solicita el acceso
//! @return 1 si tiene permiso, 0 en caso contrario
//!
//! @details Los administradores (ROL_ADMIN) tienen acceso total.
//!          Los usuarios comunes solo pueden acceder a directorios
//!          dentro de su árbol personal. La verificación se hace
//!          recorriendo hacia arriba desde el destino hasta encontrar
//!          el directorio personal del usuario.
int tienePermiso(Nodo *destino, Nodo *userDir, Usuario *usuario) {
    // Los administradores tienen acceso completo al sistema
    if (usuario->rol == ROL_ADMIN) return 1;

    // Para usuarios comunes: verificar que destino esté dentro de su árbol
    Nodo *cursor = destino;
    while (cursor != NULL) {
        if (cursor == userDir) return 1;  // Encontró el directorio personal
        cursor = cursor->padre;            // Sube al padre
    }
    return 0;  // No está dentro del árbol personal
}

//! @}
