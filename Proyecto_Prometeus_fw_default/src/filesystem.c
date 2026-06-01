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
        printf("Error: ruta invalida '%s'\n", ruta);
        return;
    }
    if (buscarHijo(padre, nombre)) {
        printf("Error: ya existe '%s'\n", nombre);
        return;
    }
    Nodo *nuevo = crearNodo(nombre, DIRECTORIO);
    agregarHijo(padre, nuevo);
    printf("Directorio creado: %s\n", nombre);
}

void fs_touch(const char *ruta, Nodo *actual, Nodo *root) {
    char padreRuta[512], nombre[MAX_NOMBRE];
    separarRuta(ruta, padreRuta, nombre);

    Nodo *padre = resolverRuta(padreRuta, actual, root);
    if (!padre || padre->tipo != DIRECTORIO) {
        printf("Error: ruta invalida\n");
        return;
    }
    if (buscarHijo(padre, nombre)) {
        return;
    }
    Nodo *nuevo = crearNodo(nombre, ARCHIVO);
    agregarHijo(padre, nuevo);
    printf("Archivo creado: %s\n", nombre);
}

void fs_ls(const char *ruta, Nodo *actual, Nodo *root) {
    Nodo *nodo = resolverRuta(ruta, actual, root);
    if (!nodo) { printf("ls: no existe '%s'\n", ruta); return; }
    if (nodo->tipo != DIRECTORIO) { printf("ls: no es directorio\n"); return; }

    if (nodo->numHijos == 0) {
        printf("(vacio)\n");
        return;
    }
    for (int i = 0; i < nodo->numHijos; i++) {
        if (nodo->hijos[i]->tipo == DIRECTORIO)
            printf("%s/  ", nodo->hijos[i]->nombre);
        else
            printf("%s  ", nodo->hijos[i]->nombre);
    }
    printf("\n");
}

void fs_cat(const char *ruta, Nodo *actual, Nodo *root) {
    Nodo *nodo = resolverRuta(ruta, actual, root);
    if (!nodo) { printf("cat: no existe '%s'\n", ruta); return; }
    if (nodo->tipo != ARCHIVO) { printf("cat: es un directorio\n"); return; }
    if (!nodo->contenido || nodo->size == 0) { printf("(archivo vacio)\n"); return; }
    printf("%s\n", nodo->contenido);
}

void fs_write(const char *ruta, const char *texto, Nodo *actual, Nodo *root) {
    Nodo *nodo = resolverRuta(ruta, actual, root);
    if (!nodo) { printf("write: no existe '%s'\n", ruta); return; }
    if (nodo->tipo != ARCHIVO) { printf("write: no es archivo\n"); return; }

    free(nodo->contenido);
    int len = (int)strlen(texto); nodo->contenido = (char*)malloc(len+1); if(nodo->contenido){ memcpy(nodo->contenido,texto,len+1); }
    nodo->size = len;
    printf("Escrito (%d bytes)\n", nodo->size);
}

void fs_rm(const char *ruta, Nodo *actual, Nodo *root) {
    Nodo *nodo = resolverRuta(ruta, actual, root);
    if (!nodo) { printf("rm: no existe '%s'\n", ruta); return; }
    if (!nodo->padre)  { printf("rm: no se puede eliminar root\n"); return; }

    Nodo *padre = nodo->padre;
    quitarHijo(padre, nodo);
    eliminarRecursivo(nodo);
    printf("Eliminado: %s\n", ruta);
}

Nodo *fs_cd(const char *ruta, Nodo *actual, Nodo *root,
            Usuario *usuario, Nodo *userDir) {
    Nodo *destino = resolverRuta(ruta, actual, root);
    if (!destino || destino->tipo != DIRECTORIO) {
        printf("cd: no existe o no es directorio: '%s'\n", ruta);
        return actual;
    }
    if (!tienePermiso(destino, userDir, usuario)) {
        printf("cd: permiso denegado\n");
        return actual;
    }
    return destino;
}
