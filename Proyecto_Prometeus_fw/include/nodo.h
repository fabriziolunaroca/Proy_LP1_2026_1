//! @file nodo.h
//! @brief Estructuras y funciones para el sistema de archivos jerárquico

#ifndef NODO_H
#define NODO_H

//! @name Constantes
//! @{

//! @brief Tamaño máximo del nombre de un archivo/directorio
#define MAX_NOMBRE 64

//! @brief Tamaño máximo del contenido de un archivo
#define MAX_CONTENIDO 4096

//! @}

//! @name Tipos de nodo
//! @{

//! @brief Tipos de nodo en el sistema de archivos
typedef enum {
    DIRECTORIO,  //!< Directorio (puede contener hijos)
    ARCHIVO      //!< Archivo (contiene datos)
} TipoNodo;

//! @}

//! @name Estructura principal
//! @{

//! @brief Estructura que representa un nodo del sistema de archivos
typedef struct Nodo {
    char nombre[MAX_NOMBRE];      //!< Nombre del nodo
    TipoNodo tipo;                //!< Tipo de nodo
    struct Nodo *padre;           //!< Puntero al nodo padre
    struct Nodo **hijos;          //!< Arreglo de punteros a hijos
    int numHijos;                 //!< Número de hijos
    char *contenido;              //!< Contenido (solo para ARCHIVO)
    int size;                     //!< Tamaño del contenido
} Nodo;

//! @}

//! @name Funciones básicas
//! @{

//! @brief Crea un nuevo nodo en memoria dinámica
//! @param nombre Nombre del nodo
//! @param tipo Tipo de nodo (DIRECTORIO o ARCHIVO)
//! @return Puntero al nodo creado
Nodo *crearNodo(const char *nombre, TipoNodo tipo);

//! @brief Agrega un nodo hijo a un nodo padre
//! @param padre Nodo padre
//! @param hijo Nodo hijo a agregar
void agregarHijo(Nodo *padre, Nodo *hijo);

//! @brief Busca un nodo hijo por su nombre
//! @param padre Nodo padre donde buscar
//! @param nombre Nombre del hijo a buscar
//! @return Puntero al nodo encontrado, o NULL si no existe
Nodo *buscarHijo(Nodo *padre, const char *nombre);

//! @brief Elimina recursivamente un nodo y todos sus descendientes
//! @param nodo Nodo raíz a eliminar
void eliminarRecursivo(Nodo *nodo);

//! @brief Quita un hijo del arreglo de hijos (sin liberarlo)
//! @param padre Nodo padre
//! @param hijo Nodo hijo a quitar
void quitarHijo(Nodo *padre, Nodo *hijo);

//! @}

#endif
