//! @file usuario.h
//! @brief Estructuras y funciones para la gestión de usuarios

#ifndef USUARIO_H
#define USUARIO_H

//! @name Constantes
//! @{

//! @brief Tamaño máximo del nombre de usuario
#define MAX_USUARIO  32

//! @brief Tamaño máximo de la contraseña
#define MAX_PASSWORD 64

//! @brief Tamaño máximo del ID (4 dígitos + terminador nulo)
#define MAX_ID        5

//! @brief Número máximo de usuarios que puede almacenar el sistema
#define MAX_USUARIOS 64

//! @}

//! @name Tipos de usuario
//! @{

//! @brief Roles posibles para un usuario del sistema
typedef enum {
    ROL_COMUN,   //!< Usuario común: acceso restringido
    ROL_ADMIN    //!< Administrador: acceso total
} RolUsuario;

//! @}

//! @name Estructuras principales
//! @{

//! @brief Estructura que representa un usuario del sistema
typedef struct {
    char      id[MAX_ID];           //!< Identificador único (4 dígitos)
    char      usuario[MAX_USUARIO]; //!< Nombre de usuario para login
    char      password[MAX_PASSWORD]; //!< Contraseña del usuario
    RolUsuario rol;                 //!< Nivel de privilegio
} Usuario;

//! @brief Estructura que contiene una lista estática de usuarios
typedef struct {
    Usuario lista[MAX_USUARIOS];  //!< Arreglo estático de usuarios
    int     numUsuarios;          //!< Número actual de usuarios
} ListaUsuarios;

//! @}

//! @name Funciones de gestión de usuarios
//! @{

//! @brief Crea una nueva estructura Usuario
//! @param id Identificador único del usuario
//! @param usuario Nombre de usuario
//! @param password Contraseña
//! @param rol Rol del usuario (ROL_COMUN o ROL_ADMIN)
//! @return Estructura Usuario inicializada
Usuario crearUsuario(const char *id, const char *usuario,
                     const char *password, RolUsuario rol);

//! @brief Agrega un usuario a la lista
//! @param lista Lista de usuarios
//! @param u Usuario a agregar
void agregarUsuario(ListaUsuarios *lista, Usuario u);

//! @brief Busca un usuario por su nombre de usuario
//! @param lista Lista de usuarios
//! @param usuario Nombre de usuario a buscar
//! @return Puntero al usuario encontrado, o NULL si no existe
Usuario *buscarUsuario(ListaUsuarios *lista, const char *usuario);

//! @}

#endif
