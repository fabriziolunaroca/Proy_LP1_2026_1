#ifndef NODO_H
#define NODO_H

typedef enum {
    ARCHIVO,
    DIRECTORIO
} TipoNodo;

typedef struct Nodo {

    char nombre[100];

    TipoNodo tipo;

    struct Nodo* padre;

    struct Nodo** hijos;

    int numHijos;

    char* contenido;

    int size;

} Nodo;

Nodo* crearNodo(char* nombre, TipoNodo tipo);

void agregarHijo(Nodo* padre, Nodo* hijo);

Nodo* buscarHijo(Nodo* padre, char* nombre);

void eliminarNodo(Nodo* nodo);

#endif
