#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/filesystem.h"
#include "../include/utils.h"

void mkdirFS(char* nombre, Nodo* actual) {

    Nodo* nuevo = crearNodo(
        nombre,
        DIRECTORIO
    );

    agregarHijo(actual,nuevo);
}

void touchFS(char* nombre, Nodo* actual) {

    if(!extensionValida(nombre)) {

        printf("Extension invalida\n");

        return;
    }

    Nodo* nuevo = crearNodo(
        nombre,
        ARCHIVO
    );

    agregarHijo(actual,nuevo);
}

void lsFS(Nodo* actual) {

    for(int i=0;i<actual->numHijos;i++) {

        printf("%s\n",
            actual->hijos[i]->nombre
        );
    }
}

Nodo* cdFS(
    Nodo* actual,
    char* ruta,
    Usuario user
) {

    if(strcmp(ruta,"..")==0) {

        if(user.tipo == COMUN) {

            if(actual->padre != NULL &&
               strcmp(
                actual->padre->nombre,
                "home"
               ) == 0) {

                printf(
                    "No puede subir mas\n"
                );

                return actual;
            }
        }

        if(actual->padre != NULL)
            return actual->padre;

        return actual;
    }

    Nodo* hijo = buscarHijo(actual,ruta);

    if(hijo == NULL) {

        printf("No existe\n");

        return actual;
    }

    if(hijo->tipo != DIRECTORIO) {

        printf("No es directorio\n");

        return actual;
    }

    return hijo;
}

void writeFS(
    char* nombre,
    char* texto,
    Nodo* actual
) {

    Nodo* archivo = buscarHijo(
        actual,
        nombre
    );

    if(archivo == NULL)
        return;

    free(archivo->contenido);

    archivo->contenido = malloc(
        strlen(texto)+1
    );

    strcpy(
        archivo->contenido,
        texto
    );

    archivo->size = strlen(texto);
}

void catFS(
    char* nombre,
    Nodo* actual
) {

    Nodo* archivo = buscarHijo(
        actual,
        nombre
    );

    if(archivo == NULL)
        return;

    printf("%s\n",
        archivo->contenido
    );
}

void rmFS(
    char* nombre,
    Nodo* actual,
    Nodo* papelera
) {

    Nodo* archivo = buscarHijo(
        actual,
        nombre
    );

    if(archivo == NULL)
        return;

    agregarHijo(
        papelera,
        archivo
    );

    printf(
        "Movido a papelera\n"
    );
}
