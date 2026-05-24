#include <stdio.h>
#include <string.h>
#include "../include/auth.h"
#include "../include/utils.h"

int idEnCredenciales(const char *id) {
    FILE *f = fopen(RUTA_CREDENCIALES, "r");
    if (!f) { printf("\033[1;31mError: no se pudo abrir credenciales.txt\033[0m\n"); return 0; }

    char linea[16];
    while (fgets(linea, sizeof(linea), f)) {
        linea[strcspn(linea, "\n\r")] = '\0';
        if (strcmp(linea, id) == 0) { fclose(f); return 1; }
    }
    fclose(f);
    return 0;
}

int loginUsuario(ListaUsuarios *lista, const char *usuario,
                 const char *password, Usuario **out) {
    for (int i = 0; i < lista->numUsuarios; i++) {
        if (strcmp(lista->lista[i].usuario,  usuario)  == 0 &&
            strcmp(lista->lista[i].password, password) == 0) {
            *out = &lista->lista[i];
            return 1;
        }
    }
    return 0;
}

int registrarUsuario(ListaUsuarios *lista) {
    char id[MAX_ID];
    char nombre[MAX_USUARIO];
    char password[MAX_PASSWORD];
    char confirm[MAX_PASSWORD];
    char key[32];

    printf("\n\033[1;36m-- Crear cuenta --\033[0m\n");

    leerLinea("  ID (4 digitos)     : ", id, sizeof(id));
    if (strlen(id) != 4) {
        printf("\033[1;31mError: el ID debe tener exactamente 4 digitos.\033[0m\n");
        return 0;
    }
    for (int i = 0; i < 4; i++) {
        if (id[i] < '0' || id[i] > '9') {
            printf("\033[1;31mError: el ID solo puede contener numeros.\033[0m\n");
            return 0;
        }
    }
    if (!idEnCredenciales(id)) {
        printf("\033[1;31mError: ID '%s' no encontrado en credenciales.txt\033[0m\n", id);
        return 0;
    }
    for (int i = 0; i < lista->numUsuarios; i++) {
        if (strcmp(lista->lista[i].id, id) == 0) {
            printf("\033[1;31mError: ese ID ya fue registrado.\033[0m\n");
            return 0;
        }
    }

    leerLinea("  Nombre de usuario  : ", nombre, sizeof(nombre));
    if (strlen(nombre) < 3) {
        printf("\033[1;31mError: nombre muy corto (minimo 3 caracteres).\033[0m\n");
        return 0;
    }
    if (buscarUsuario(lista, nombre)) {
        printf("\033[1;31mError: nombre de usuario ya existe.\033[0m\n");
        return 0;
    }

    leerPassword("  Password           : ", password, sizeof(password));
    if (strlen(password) < 4) {
        printf("\033[1;31mError: password muy corta (minimo 4 caracteres).\033[0m\n");
        return 0;
    }
    leerPassword("  Confirmar password : ", confirm,  sizeof(confirm));
    if (strcmp(password, confirm) != 0) {
        printf("\033[1;31mError: las passwords no coinciden.\033[0m\n");
        return 0;
    }

    leerPassword("  Key de acceso      : ", key, sizeof(key));
    RolUsuario rol;
    if (strcmp(key, KEY_ADMIN) == 0) {
        rol = ROL_ADMIN;
    } else if (strcmp(key, KEY_COMUN) == 0) {
        rol = ROL_COMUN;
    } else {
        printf("\033[1;31mError: key invalida.\033[0m\n");
        return 0;
    }

    Usuario u = crearUsuario(id, nombre, password, rol);
    agregarUsuario(lista, u);

    printf("\n\033[1;32mCuenta creada exitosamente.\033[0m\n");
    printf("  Usuario : \033[1;37m%s\033[0m\n", nombre);
    printf("  ID      : \033[1;33m%s\033[0m\n", id);
    printf("  Tipo    : \033[1;%sm%s\033[0m\n",
           rol == ROL_ADMIN ? "31" : "36",
           rol == ROL_ADMIN ? "admin" : "comun");
    return 1;
}
