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

/* ─────────────────────────────────────────
   SNAPSHOT (TEXTO PLANO PARA LA GUI)
   Recorre el árbol en el mismo orden pre-orden
   que escribirNodo(), pero en vez de escribir
   bytes binarios escribe líneas de texto:

     D|<nivel>|<nombre>
     F|<nivel>|<nombre>|<size>
     <contenido literal, si lo hay>
     ~~~

   El delimitador "~~~" marca el fin del
   contenido de un archivo y siempre aparece
   en su propia línea, incluso si el archivo
   está vacío. Este archivo es puramente
   aditivo: ningún otro componente de este
   programa lo lee, solo se escribe para que
   herramientas externas (como un visor en
   Python/Tkinter) lo consuman.
───────────────────────────────────────── */

//! @name Funciones internas de exportación del snapshot
//! @{

//! @brief Escribe recursivamente un nodo y sus hijos en formato texto
//! @param f Archivo de texto abierto en modo escritura
//! @param nodo Nodo a escribir
//! @param nivel Profundidad del nodo (0 = raíz)
//! @details Reutiliza el mismo recorrido pre-orden que escribirNodo(),
//!          adaptado a un formato de texto plano legible por humanos
//!          y fácil de parsear desde Python.
static void escribirNodoTexto(FILE *f, Nodo *nodo, int nivel) {
    if (nodo->tipo == DIRECTORIO) {
        fprintf(f, "D|%d|%s\n", nivel, nodo->nombre);
    } else {
        fprintf(f, "F|%d|%s|%d\n", nivel, nodo->nombre, nodo->size);
        if (nodo->size > 0 && nodo->contenido)
            fprintf(f, "%s\n", nodo->contenido);
        fprintf(f, "~~~\n");
    }

    for (int i = 0; i < nodo->numHijos; i++)
        escribirNodoTexto(f, nodo->hijos[i], nivel + 1);
}

//! @}

//! @name Funciones públicas de exportación del snapshot
//! @{

//! @brief Exporta el sistema de archivos a texto plano legible (snapshot.txt)
//! @param root Nodo raíz del sistema de archivos
//! @details Sobrescribe por completo data/snapshot.txt en cada llamada,
//!          igual que guardarFilesystem() hace con filesystem.dat. Si el
//!          archivo no se puede abrir, se informa el error por stderr y
//!          la función retorna sin interrumpir el resto del programa
//!          (mismo criterio de manejo de errores que el resto del módulo).
void exportarSnapshot(Nodo *root) {
    FILE *f = fopen(RUTA_SNAPSHOT, "w");
    if (!f) { perror("exportarSnapshot: no se pudo abrir"); return; }
    escribirNodoTexto(f, root, 0);
    fclose(f);
}

//! @}
