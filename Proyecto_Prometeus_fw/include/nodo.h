#ifndef NODO_H
#define NODO_H

#define MAX_NOMBRE 64
#define MAX_CONTENIDO 4096

typedef enum {
    DIRECTORIO,
    ARCHIVO
} TipoNodo;

typedef struct Nodo {
    char nombre[MAX_NOMBRE];
    TipoNodo tipo;
    struct Nodo *padre;
    struct Nodo **hijos;
    int numHijos;
    char *contenido;
    int size;
} Nodo;

Nodo *crearNodo(const char *nombre, TipoNodo tipo);
void  agregarHijo(Nodo *padre, Nodo *hijo);
Nodo *buscarHijo(Nodo *padre, const char *nombre);
void  eliminarRecursivo(Nodo *nodo);
void  quitarHijo(Nodo *padre, Nodo *hijo);

#endif
