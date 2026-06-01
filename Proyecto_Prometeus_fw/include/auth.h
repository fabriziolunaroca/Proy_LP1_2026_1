//! @file auth.h
//! @brief Autenticación y registro de usuarios

#ifndef AUTH_H
#define AUTH_H

#include "usuario.h"

//! @name Claves de acceso
//! @{

//! @brief Clave para obtener rol de administrador
#define KEY_ADMIN "234600389"

//! @brief Clave para obtener rol de usuario común
#define KEY_COMUN "456263990"

//! @}

//! @name Rutas de archivos
//! @{

//! @brief Ruta del archivo de credenciales válidas
#define RUTA_CREDENCIALES "data/credenciales.txt"

//! @}

//! @name Funciones de autenticación
//! @{

//! @brief Verifica si un ID existe en el archivo de credenciales
//! @param id ID de 4 dígitos a verificar
//! @return 1 si el ID existe, 0 en caso contrario
int idEnCredenciales(const char *id);

//! @brief Inicia sesión con usuario y contraseña
//! @param lista Lista de usuarios registrados
//! @param usuario Nombre de usuario
//! @param password Contraseña
//! @param out Puntero donde se guardará el usuario encontrado
//! @return 1 si las credenciales son correctas, 0 en caso contrario
int loginUsuario(ListaUsuarios *lista, const char *usuario,
                 const char *password, Usuario **out);

//! @brief Registra un nuevo usuario en el sistema
//! @param lista Lista de usuarios donde se agregará
//! @return 1 si el registro fue exitoso, 0 en caso contrario
int registrarUsuario(ListaUsuarios *lista);

//! @}

#endif
