#ifndef USUARIO_H
#define USUARIO_H

typedef enum {
    ADMIN,
    COMUN
} TipoUsuario;

typedef struct {

    char username[20];

    char password[20];

    TipoUsuario tipo;

} Usuario;

#endif
