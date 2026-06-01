#include <stddef.h>
#include "../include/permisos.h"

int tienePermiso(Nodo *destino, Nodo *userDir, Usuario *usuario) {
    if (usuario->rol == ROL_ADMIN) return 1;

    Nodo *cursor = destino;
    while (cursor != NULL) {
        if (cursor == userDir) return 1;
        cursor = cursor->padre;
    }
    return 0;
}
