//! @file persistencia.c
//! @brief Implementación de persistencia (guardado/carga en disco)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/persistencia.h"

/* ─────────────────────────────────────────
   USUARIOS
   Formato binario simple:
     [int numUsuarios]
     por cada usuario:
       [char id[MAX_ID]]
       [char usuario[MAX_USUARIO]]
       [char password[MAX_PASSWORD]]
       [int  rol]
───────────────────────────────────────── */

//! @name Funciones de persistencia de usuarios
//! @{

//! @brief Guarda la lista de usuarios en un archivo binario
//! @param lista Lista de usuarios a guardar
//! @details Formato: número de usuarios seguido de los datos de cada uno
void guardarUsuarios(ListaUsuarios *lista) {
    FILE *f = fopen(RUTA_USUARIOS, "wb");
    if (!f) { perror("guardarUsuarios: no se pudo abrir"); return; }

    fwrite(&lista->numUsuarios, sizeof(int), 1, f);
    for (int i = 0; i < lista->numUsuarios; i++) {
        fwrite(lista->lista[i].id,       sizeof(char), MAX_ID,       f);
        fwrite(lista->lista[i].usuario,  sizeof(char), MAX_USUARIO,  f);
        fwrite(lista->lista[i].password, sizeof(char), MAX_PASSWORD, f);
        int rol = (int)lista->lista[i].rol;
        fwrite(&rol, sizeof(int), 1, f);
    }
    fclose(f);
}

//! @brief Carga la lista de usuarios desde un archivo binario
//! @param lista Lista donde se cargarán los usuarios
//! @details Si el archivo no existe, la lista queda vacía
void cargarUsuarios(ListaUsuarios *lista) {
    lista->numUsuarios = 0;

    FILE *f = fopen(RUTA_USUARIOS, "rb");
    if (!f) return;

    int n = 0;
    if (fread(&n, sizeof(int), 1, f) != 1) { fclose(f); return; }

    for (int i = 0; i < n && i < MAX_USUARIOS; i++) {
        fread(lista->lista[i].id,       sizeof(char), MAX_ID,       f);
        fread(lista->lista[i].usuario,  sizeof(char), MAX_USUARIO,  f);
        fread(lista->lista[i].password, sizeof(char), MAX_PASSWORD, f);
        int rol = 0;
        fread(&rol, sizeof(int), 1, f);
        lista->lista[i].rol = (RolUsuario)rol;
        lista->numUsuarios++;
    }
    fclose(f);
}

//! @}

/* ─────────────────────────────────────────
   FILESYSTEM
   Recorre el árbol en pre-orden y escribe
   cada nodo con este formato:
     [char nombre[MAX_NOMBRE]]
     [int  tipo]           (0=DIR, 1=ARCHIVO)
     [int  numHijos]
     si tipo==ARCHIVO:
       [int  size]
       [char contenido[size+1]]
   Al cargar se reconstruye recursivamente.
───────────────────────────────────────── */

//! @name Funciones internas de persistencia del filesystem
//! @{

//! @brief Escribe recursivamente un nodo y sus hijos en un archivo
//! @param f Archivo abierto en modo binario
//! @param nodo Nodo a escribir
//! @details Recorre el árbol en pre-orden, escribiendo nombre, tipo,
//!          número de hijos, y para archivos también tamaño y contenido
static void escribirNodo(FILE *f, Nodo *nodo) {
    fwrite(nodo->nombre, sizeof(char), MAX_NOMBRE, f);
    int tipo = (int)nodo->tipo;
    fwrite(&tipo, sizeof(int), 1, f);
    fwrite(&nodo->numHijos, sizeof(int), 1, f);

    if (nodo->tipo == ARCHIVO) {
        fwrite(&nodo->size, sizeof(int), 1, f);
        if (nodo->size > 0 && nodo->contenido)
            fwrite(nodo->contenido, sizeof(char), nodo->size + 1, f);
        else {
            char vacio = '\0';
            fwrite(&vacio, sizeof(char), 1, f);
        }
    }

    for (int i = 0; i < nodo->numHijos; i++)
        escribirNodo(f, nodo->hijos[i]);
}

//! @brief Lee recursivamente un nodo y sus hijos desde un archivo
//! @param f Archivo abierto en modo binario
//! @return Puntero al nodo reconstruido, o NULL si error
static Nodo *leerNodo(FILE *f) {
    char nombre[MAX_NOMBRE];
    if (fread(nombre, sizeof(char), MAX_NOMBRE, f) != MAX_NOMBRE)
        return NULL;

    int tipo = 0;
    if (fread(&tipo, sizeof(int), 1, f) != 1) return NULL;

    int numHijos = 0;
    if (fread(&numHijos, sizeof(int), 1, f) != 1) return NULL;

    Nodo *nodo = crearNodo(nombre, (TipoNodo)tipo);

    if (tipo == (int)ARCHIVO) {
        int size = 0;
        fread(&size, sizeof(int), 1, f);
        nodo->size = size;
        nodo->contenido = (char *)malloc(size + 2);
        if (nodo->contenido)
            fread(nodo->contenido, sizeof(char), size + 1, f);
    }

    for (int i = 0; i < numHijos; i++) {
        Nodo *hijo = leerNodo(f);
        if (hijo) agregarHijo(nodo, hijo);
    }
    return nodo;
}

//! @}

//! @name Funciones públicas de persistencia del filesystem
//! @{

//! @brief Guarda todo el sistema de archivos en un archivo binario
//! @param root Nodo raíz del sistema de archivos
void guardarFilesystem(Nodo *root) {
    FILE *f = fopen(RUTA_FILESYSTEM, "wb");
    if (!f) { perror("guardarFilesystem: no se pudo abrir"); return; }
    escribirNodo(f, root);
    fclose(f);
}

//! @brief Carga el sistema de archivos desde un archivo binario
//! @return Nodo raíz reconstruido, o NULL si el archivo no existe
Nodo *cargarFilesystem(void) {
    FILE *f = fopen(RUTA_FILESYSTEM, "rb");
    if (!f) return NULL;
    Nodo *root = leerNodo(f);
    fclose(f);
    return root;
}

//! @}
