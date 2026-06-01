#include <stdio.h>
#include <string.h>
#include "../include/usuario.h"

Usuario crearUsuario(const char *id, const char *usuario,
                     const char *password, RolUsuario rol) {
    Usuario u;
    strncpy(u.id,       id,       MAX_ID       - 1); u.id[MAX_ID - 1]             = '\0';
    strncpy(u.usuario,  usuario,  MAX_USUARIO  - 1); u.usuario[MAX_USUARIO - 1]   = '\0';
    strncpy(u.password, password, MAX_PASSWORD - 1); u.password[MAX_PASSWORD - 1] = '\0';
    u.rol = rol;
    return u;
}

void agregarUsuario(ListaUsuarios *lista, Usuario u) {
    if (lista->numUsuarios >= MAX_USUARIOS) {
        printf("Error: lista de usuarios llena.\n");
        return;
    }
    lista->lista[lista->numUsuarios++] = u;
}

Usuario *buscarUsuario(ListaUsuarios *lista, const char *usuario) {
    for (int i = 0; i < lista->numUsuarios; i++) {
        if (strcmp(lista->lista[i].usuario, usuario) == 0)
            return &lista->lista[i];
    }
    return NULL;
}
