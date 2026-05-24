#ifndef AUTH_H
#define AUTH_H

#include "usuario.h"

#define KEY_ADMIN "234600389"
#define KEY_COMUN "456263990"
#define RUTA_CREDENCIALES "data/credenciales.txt"

int idEnCredenciales(const char *id);
int loginUsuario(ListaUsuarios *lista, const char *usuario,
                 const char *password, Usuario **out);
int registrarUsuario(ListaUsuarios *lista);

#endif
