//! @file persistencia.h
//! @brief Persistencia de datos (guardado/carga en disco)

#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include "usuario.h"
#include "nodo.h"

//! @name Rutas de archivos de persistencia
//! @{

//! @brief Ruta del archivo binario para almacenar los usuarios
#define RUTA_USUARIOS   "data/usuarios.dat"

//! @brief Ruta del archivo binario para almacenar el sistema de archivos
#define RUTA_FILESYSTEM "data/filesystem.dat"

//! @}

//! @name Funciones de persistencia de usuarios
//! @{

//! @brief Guarda la lista de usuarios en un archivo binario
//! @param lista Lista de usuarios a guardar
void guardarUsuarios(ListaUsuarios *lista);

//! @brief Carga la lista de usuarios desde un archivo binario
//! @param lista Lista donde se cargarán los usuarios
void cargarUsuarios(ListaUsuarios *lista);

//! @}

//! @name Funciones de persistencia del sistema de archivos
//! @{

//! @brief Guarda todo el sistema de archivos en un archivo binario
//! @param root Nodo raíz del sistema de archivos
void guardarFilesystem(Nodo *root);

//! @brief Carga el sistema de archivos desde un archivo binario
//! @return Nodo raíz reconstruido, o NULL si el archivo no existe
Nodo *cargarFilesystem(void);

//! @}

#endif
