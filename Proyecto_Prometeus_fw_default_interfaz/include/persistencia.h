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

//! @brief Ruta del archivo de texto plano exportado para la GUI (Tkinter)
#define RUTA_SNAPSHOT "data/snapshot.txt"

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

//! @name Funciones de exportación para interfaces externas
//! @{

//! @brief Exporta el sistema de archivos a texto plano legible (snapshot.txt)
//! @param root Nodo raíz del sistema de archivos
//! @details Genera un archivo de texto plano pensado para ser consumido por
//!          herramientas externas (por ejemplo, un visor en Python/Tkinter).
//!          No reemplaza ni afecta a guardarFilesystem(); es puramente
//!          aditivo y de solo escritura desde la perspectiva de este programa.
void exportarSnapshot(Nodo *root);

//! @}

#endif
