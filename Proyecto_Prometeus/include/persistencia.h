#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include "usuario.h"
#include "nodo.h"

#define RUTA_USUARIOS   "data/usuarios.dat"
#define RUTA_FILESYSTEM "data/filesystem.dat"

void guardarUsuarios(ListaUsuarios *lista);
void cargarUsuarios(ListaUsuarios *lista);

void guardarFilesystem(Nodo *root);
Nodo *cargarFilesystem(void);

#endif
