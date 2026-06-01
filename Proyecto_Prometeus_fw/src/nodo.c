//! @file nodo.c
//! @brief Implementación de operaciones básicas sobre nodos del árbol

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/nodo.h"

//! @name Funciones básicas de nodos
//! @{

//! @brief Crea un nuevo nodo en memoria dinámica
//! @param nombre Nombre del nodo
//! @param tipo Tipo de nodo (DIRECTORIO o ARCHIVO)
//! @return Puntero al nodo creado
Nodo *crearNodo(const char *nombre, TipoNodo tipo) {
    Nodo *n = (Nodo *)malloc(sizeof(Nodo));
    if (!n) { perror("malloc nodo"); exit(1); }
    strncpy(n->nombre, nombre, MAX_NOMBRE - 1);
    n->nombre[MAX_NOMBRE - 1] = '\0';
    n->tipo      = tipo;
    n->padre     = NULL;
    n->hijos     = NULL;
    n->numHijos  = 0;
    n->contenido = NULL;
    n->size      = 0;
    return n;
}

//! @brief Agrega un nodo hijo a un nodo padre
//! @param padre Nodo padre
//! @param hijo Nodo hijo a agregar
void agregarHijo(Nodo *padre, Nodo *hijo) {
    padre->numHijos++;
    padre->hijos = (Nodo **)realloc(padre->hijos,
                                    padre->numHijos * sizeof(Nodo *));
    if (!padre->hijos) { perror("realloc hijos"); exit(1); }
    padre->hijos[padre->numHijos - 1] = hijo;
    hijo->padre = padre;
}

//! @brief Busca un nodo hijo por su nombre
//! @param padre Nodo padre donde buscar
//! @param nombre Nombre del hijo a buscar
//! @return Puntero al nodo encontrado, o NULL si no existe
Nodo *buscarHijo(Nodo *padre, const char *nombre) {
    for (int i = 0; i < padre->numHijos; i++) {
        if (strcmp(padre->hijos[i]->nombre, nombre) == 0)
            return padre->hijos[i];
    }
    return NULL;
}

//! @brief Elimina recursivamente un nodo y todos sus descendientes
//! @param nodo Nodo raíz a eliminar
void eliminarRecursivo(Nodo *nodo) {
    if (nodo->tipo == DIRECTORIO) {
        for (int i = 0; i < nodo->numHijos; i++)
            eliminarRecursivo(nodo->hijos[i]);
    }
    free(nodo->contenido);
    free(nodo->hijos);
    free(nodo);
}

//! @brief Quita un hijo del arreglo de hijos (sin liberarlo)
//! @param padre Nodo padre
//! @param hijo Nodo hijo a quitar
void quitarHijo(Nodo *padre, Nodo *hijo) {
    for (int i = 0; i < padre->numHijos; i++) {
        if (padre->hijos[i] == hijo) {
            for (int j = i; j < padre->numHijos - 1; j++)
                padre->hijos[j] = padre->hijos[j + 1];
            padre->numHijos--;
            padre->hijos = (Nodo **)realloc(padre->hijos,
                                             padre->numHijos * sizeof(Nodo *));
            return;
        }
    }
}

//! @}
