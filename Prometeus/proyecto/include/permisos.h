#ifndef PERMISOS_H
#define PERMISOS_H

#include "nodo.h"
#include "usuario.h"

int tienePermiso(Nodo *destino, Nodo *userDir, Usuario *usuario);

#endif
