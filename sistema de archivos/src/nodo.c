#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/nodo.h"

Nodo* crearNodo(char* nombre, TipoNodo tipo) {

    Nodo* nodo = malloc(sizeof(Nodo));

    strcpy(nodo->nombre, nombre);

    nodo->tipo = tipo;

    nodo->padre = NULL;

    nodo->hijos = NULL;

    nodo->numHijos = 0;

    nodo->contenido = NULL;

    nodo->size = 0;

    return nodo;
}

void agregarHijo(Nodo* padre, Nodo* hijo) {

    padre->numHijos++;

    padre->hijos = realloc(
        padre->hijos,
        padre->numHijos * sizeof(Nodo*)
    );

    padre->hijos[padre->numHijos - 1] = hijo;

    hijo->padre = padre;
}

Nodo* buscarHijo(Nodo* padre, char* nombre) {

    for(int i=0;i<padre->numHijos;i++) {

        if(strcmp(
            padre->hijos[i]->nombre,
            nombre
        ) == 0) {

            return padre->hijos[i];
        }
    }

    return NULL;
}

void eliminarNodo(Nodo* nodo) {

    if(nodo == NULL)
        return;

    for(int i=0;i<nodo->numHijos;i++) {

        eliminarNodo(nodo->hijos[i]);
    }

    free(nodo->contenido);

    free(nodo->hijos);

    free(nodo);
}
