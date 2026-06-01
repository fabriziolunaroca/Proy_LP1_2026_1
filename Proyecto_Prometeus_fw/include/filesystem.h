//! @file filesystem.h
//! @brief Operaciones del sistema de archivos virtual

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "nodo.h"
#include "usuario.h"

//! @name Resolución de rutas
//! @{

//! @brief Resuelve una ruta a su nodo correspondiente
//! @param ruta Ruta a resolver (absoluta o relativa)
//! @param actual Nodo actual (para rutas relativas)
//! @param root Nodo raíz (para rutas absolutas)
//! @return Puntero al nodo encontrado, o NULL si no existe
Nodo *resolverRuta(const char *ruta, Nodo *actual, Nodo *root);

//! @brief Separa una ruta en directorio padre y nombre base
//! @param ruta Ruta completa
//! @param padreRuta Buffer para la ruta del padre
//! @param nombre Buffer para el nombre base
void separarRuta(const char *ruta, char *padreRuta, char *nombre);

//! @}

//! @name Comandos del sistema de archivos
//! @{

//! @brief Crea un nuevo directorio
//! @param ruta Ruta donde crear el directorio
//! @param actual Directorio actual
//! @param root Nodo raíz
void fs_mkdir(const char *ruta, Nodo *actual, Nodo *root);

//! @brief Crea un nuevo archivo vacío
//! @param ruta Ruta donde crear el archivo
//! @param actual Directorio actual
//! @param root Nodo raíz
void fs_touch(const char *ruta, Nodo *actual, Nodo *root);

//! @brief Lista el contenido de un directorio
//! @param ruta Ruta del directorio a listar
//! @param actual Directorio actual
//! @param root Nodo raíz
void fs_ls(const char *ruta, Nodo *actual, Nodo *root);

//! @brief Muestra el contenido de un archivo
//! @param ruta Ruta del archivo
//! @param actual Directorio actual
//! @param root Nodo raíz
void fs_cat(const char *ruta, Nodo *actual, Nodo *root);

//! @brief Escribe contenido en un archivo (sobrescribe)
//! @param ruta Ruta del archivo
//! @param texto Texto a escribir
//! @param actual Directorio actual
//! @param root Nodo raíz
void fs_write(const char *ruta, const char *texto, Nodo *actual, Nodo *root);

//! @brief Elimina un archivo o directorio (recursivamente)
//! @param ruta Ruta del elemento a eliminar
//! @param actual Directorio actual
//! @param root Nodo raíz
void fs_rm(const char *ruta, Nodo *actual, Nodo *root);

//! @brief Cambia el directorio actual
//! @param ruta Ruta del directorio destino
//! @param actual Directorio actual
//! @param root Nodo raíz
//! @param usuario Usuario que ejecuta el comando
//! @param userDir Directorio personal del usuario
//! @return Nuevo directorio actual (o el mismo si falla)
Nodo *fs_cd(const char *ruta, Nodo *actual, Nodo *root,
            Usuario *usuario, Nodo *userDir);

//! @}

//! @name Utilidades de rutas
//! @{

//! @brief Construye la ruta absoluta de un nodo
//! @param nodo Nodo cuya ruta se quiere obtener
//! @param root Nodo raíz
//! @param buf Buffer donde se almacenará la ruta
//! @param bufSize Tamaño del buffer
void construirRutaAbsoluta(Nodo *nodo, Nodo *root, char *buf, int bufSize);

//! @}

#endif
