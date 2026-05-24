#ifndef USUARIO_H
#define USUARIO_H

#define MAX_USUARIO  32
#define MAX_PASSWORD 64
#define MAX_ID        5
#define MAX_USUARIOS 64

typedef enum {
    ROL_COMUN,
    ROL_ADMIN
} RolUsuario;

typedef struct {
    char      id[MAX_ID];
    char      usuario[MAX_USUARIO];
    char      password[MAX_PASSWORD];
    RolUsuario rol;
} Usuario;

typedef struct {
    Usuario lista[MAX_USUARIOS];
    int     numUsuarios;
} ListaUsuarios;

Usuario     crearUsuario(const char *id, const char *usuario,
                         const char *password, RolUsuario rol);
void        agregarUsuario(ListaUsuarios *lista, Usuario u);
Usuario    *buscarUsuario(ListaUsuarios *lista, const char *usuario);

#endif
