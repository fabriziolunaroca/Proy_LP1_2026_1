#include <string.h>

#include "../include/permisos.h"

int tienePermiso(
    Usuario user,
    Nodo* actual,
    Nodo* destino
) {

    if(user.tipo == ADMIN)
        return 1;

    if(strcmp(destino->nombre,"sistema")==0)
        return 0;

    return 1;
}
