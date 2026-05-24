#ifndef PERMISOS_H
#define PERMISOS_H

#include "usuario.h"
#include "nodo.h"

int tienePermiso(Usuario user, Nodo* actual, Nodo* destino);

#endif
