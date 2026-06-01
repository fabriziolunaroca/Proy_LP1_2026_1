//! @file permisos.h
//! @brief Sistema de permisos y control de acceso

#ifndef PERMISOS_H
#define PERMISOS_H

#include "nodo.h"
#include "usuario.h"

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
//!          dentro de su árbol personal (userDir o sus subdirectorios).
int tienePermiso(Nodo *destino, Nodo *userDir, Usuario *usuario);

//! @}

#endif
