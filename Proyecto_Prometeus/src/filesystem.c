#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/filesystem.h"
#include "../include/permisos.h"

void construirRutaAbsoluta(Nodo *nodo, Nodo *root, char *buf, int bufSize) {
    if (nodo == root) { strncpy(buf, "/", bufSize); return; }

    char piezas[32][MAX_NOMBRE];
    int  count = 0;
    Nodo *cur = nodo;
    while (cur != NULL && cur != root) {
        strncpy(piezas[count++], cur->nombre, MAX_NOMBRE - 1);
        cur = cur->padre;
    }

    buf[0] = '\0';
    for (int i = count - 1; i >= 0; i--) {
        strncat(buf, "/", bufSize - strlen(buf) - 1);
        strncat(buf, piezas[i], bufSize - strlen(buf) - 1);
    }
    if (strlen(buf) == 0) strncpy(buf, "/", bufSize);
}

Nodo *resolverRuta(const char *ruta, Nodo *actual, Nodo *root) {
    Nodo *cursor = (ruta[0] == '/') ? root : actual;

    char copia[512];
    strncpy(copia, ruta, sizeof(copia) - 1);
    copia[sizeof(copia) - 1] = '\0';

    char *token = strtok(copia, "/");
    while (token != NULL) {
        if (strcmp(token, ".") == 0) {
        } else if (strcmp(token, "..") == 0) {
            if (cursor->padre != NULL) cursor = cursor->padre;
        } else {
            cursor = buscarHijo(cursor, token);
            if (cursor == NULL) return NULL;
        }
        token = strtok(NULL, "/");
    }
    return cursor;
}

void separarRuta(const char *ruta, char *padreRuta, char *nombre) {
    strncpy(padreRuta, ruta, 512 - 1);
    padreRuta[511] = '\0';

    char *ultimo = strrchr(padreRuta, '/');
    if (ultimo == NULL) {
        strcpy(nombre,    padreRuta);
        strcpy(padreRuta, ".");
    } else if (ultimo == padreRuta) {
        strcpy(nombre, ultimo + 1);
        strcpy(padreRuta, "/");
    } else {
        strcpy(nombre, ultimo + 1);
        *ultimo = '\0';
    }
}

void fs_mkdir(const char *ruta, Nodo *actual, Nodo *root) {
    char padreRuta[512], nombre[MAX_NOMBRE];
    separarRuta(ruta, padreRuta, nombre);

    Nodo *padre = resolverRuta(padreRuta, actual, root);
    if (!padre || padre->tipo != DIRECTORIO) {
        printf("\033[1;31mError: ruta invalida '%s'\033[0m\n", ruta);
        return;
    }
    if (buscarHijo(padre, nombre)) {
        printf("\033[1;31mError: ya existe '%s'\033[0m\n", nombre);
        return;
    }
    Nodo *nuevo = crearNodo(nombre, DIRECTORIO);
    agregarHijo(padre, nuevo);
    printf("\033[1;32mDirectorio creado: %s\033[0m\n", nombre);
}

void fs_touch(const char *ruta, Nodo *actual, Nodo *root) {
    char padreRuta[512], nombre[MAX_NOMBRE];
    separarRuta(ruta, padreRuta, nombre);

    Nodo *padre = resolverRuta(padreRuta, actual, root);
    if (!padre || padre->tipo != DIRECTORIO) {
        printf("\033[1;31mError: ruta invalida\033[0m\n");
        return;
    }
    if (buscarHijo(padre, nombre)) {
        return;
    }
    Nodo *nuevo = crearNodo(nombre, ARCHIVO);
    agregarHijo(padre, nuevo);
    printf("\033[1;32mArchivo creado: %s\033[0m\n", nombre);
}

void fs_ls(const char *ruta, Nodo *actual, Nodo *root) {
    Nodo *nodo = resolverRuta(ruta, actual, root);
    if (!nodo) { printf("\033[1;31mls: no existe '%s'\033[0m\n", ruta); return; }
    if (nodo->tipo != DIRECTORIO) { printf("\033[1;31mls: no es directorio\033[0m\n"); return; }

    if (nodo->numHijos == 0) {
        printf("\033[2m(vacio)\033[0m\n");
        return;
    }
    for (int i = 0; i < nodo->numHijos; i++) {
        if (nodo->hijos[i]->tipo == DIRECTORIO)
            printf("\033[1;34m%s/\033[0m  ", nodo->hijos[i]->nombre);
        else
            printf("\033[0m%s\033[0m  ", nodo->hijos[i]->nombre);
    }
    printf("\n");
}

void fs_cat(const char *ruta, Nodo *actual, Nodo *root) {
    Nodo *nodo = resolverRuta(ruta, actual, root);
    if (!nodo) { printf("\033[1;31mcat: no existe '%s'\033[0m\n", ruta); return; }
    if (nodo->tipo != ARCHIVO) { printf("\033[1;31mcat: es un directorio\033[0m\n"); return; }
    if (!nodo->contenido || nodo->size == 0) { printf("\033[2m(archivo vacio)\033[0m\n"); return; }
    printf("%s\n", nodo->contenido);
}

void fs_write(const char *ruta, const char *texto, Nodo *actual, Nodo *root) {
    Nodo *nodo = resolverRuta(ruta, actual, root);
    if (!nodo) { printf("\033[1;31mwrite: no existe '%s'\033[0m\n", ruta); return; }
    if (nodo->tipo != ARCHIVO) { printf("\033[1;31mwrite: no es archivo\033[0m\n"); return; }

    free(nodo->contenido);
    int len = (int)strlen(texto); nodo->contenido = (char*)malloc(len+1); if(nodo->contenido){ memcpy(nodo->contenido,texto,len+1); }
    nodo->size = len;
    printf("\033[1;32mEscrito (%d bytes)\033[0m\n", nodo->size);
}

void fs_rm(const char *ruta, Nodo *actual, Nodo *root) {
    Nodo *nodo = resolverRuta(ruta, actual, root);
    if (!nodo) { printf("\033[1;31mrm: no existe '%s'\033[0m\n", ruta); return; }
    if (!nodo->padre)  { printf("\033[1;31mrm: no se puede eliminar root\033[0m\n"); return; }

    Nodo *padre = nodo->padre;
    quitarHijo(padre, nodo);
    eliminarRecursivo(nodo);
    printf("\033[1;33mEliminado: %s\033[0m\n", ruta);
}

Nodo *fs_cd(const char *ruta, Nodo *actual, Nodo *root,
            Usuario *usuario, Nodo *userDir) {
    Nodo *destino = resolverRuta(ruta, actual, root);
    if (!destino || destino->tipo != DIRECTORIO) {
        printf("\033[1;31mcd: no existe o no es directorio: '%s'\033[0m\n", ruta);
        return actual;
    }
    if (!tienePermiso(destino, userDir, usuario)) {
        printf("\033[1;31mcd: permiso denegado\033[0m\n");
        return actual;
    }
    return destino;
}
