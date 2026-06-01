//! @file usuario.c
//! @brief Implementación de funciones para la gestión de usuarios

#include <stdio.h>
#include <string.h>
#include "../include/usuario.h"

//! @name Funciones de gestión de usuarios
//! @{

//! @brief Crea una nueva estructura Usuario
//! @param id Identificador único del usuario
//! @param usuario Nombre de usuario
//! @param password Contraseña
//! @param rol Rol del usuario (ROL_COMUN o ROL_ADMIN)
//! @return Estructura Usuario inicializada
Usuario crearUsuario(const char *id, const char *usuario,
                     const char *password, RolUsuario rol) {
    Usuario u;
    strncpy(u.id,       id,       MAX_ID       - 1); u.id[MAX_ID - 1]             = '\0';
    strncpy(u.usuario,  usuario,  MAX_USUARIO  - 1); u.usuario[MAX_USUARIO - 1]   = '\0';
    strncpy(u.password, password, MAX_PASSWORD - 1); u.password[MAX_PASSWORD - 1] = '\0';
    u.rol = rol;
    return u;
}

//! @brief Agrega un usuario a la lista
//! @param lista Lista de usuarios
//! @param u Usuario a agregar
void agregarUsuario(ListaUsuarios *lista, Usuario u) {
    if (lista->numUsuarios >= MAX_USUARIOS) {
        printf("Error: lista de usuarios llena.\n");
        return;
    }
    lista->lista[lista->numUsuarios++] = u;
}

//! @brief Busca un usuario por su nombre de usuario
//! @param lista Lista de usuarios
//! @param usuario Nombre de usuario a buscar
//! @return Puntero al usuario encontrado, o NULL si no existe
Usuario *buscarUsuario(ListaUsuarios *lista, const char *usuario) {
    for (int i = 0; i < lista->numUsuarios; i++) {
        if (strcmp(lista->lista[i].usuario, usuario) == 0)
            return &lista->lista[i];
    }
    return NULL;
}

//! @}
