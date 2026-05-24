#ifndef AUTH_H
#define AUTH_H

#include "usuario.h"

Usuario crearUsuario(void);

int loginUsuario(char* username, char* password);

void guardarUsuario(Usuario u);

int credencialExiste(char* prefijo);

#endif
