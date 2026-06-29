#include <stdio.h>
#include <string.h>
#include "../include/auth.h"
#include "../include/utils.h"

int idEnCredenciales(const char *id) {
    FILE *f = fopen(RUTA_CREDENCIALES, "r");
    if (!f) { printf("Error: no se pudo abrir credenciales.txt\n"); return 0; }

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

    printf("\n-- Crear cuenta --\n");

    leerLinea("  ID (4 digitos)     : ", id, sizeof(id));
    if (strlen(id) != 4) {
        printf("Error: el ID debe tener exactamente 4 digitos.\n");
        return 0;
    }
    for (int i = 0; i < 4; i++) {
        if (id[i] < '0' || id[i] > '9') {
            printf("Error: el ID solo puede contener numeros.\n");
            return 0;
        }
    }
    if (!idEnCredenciales(id)) {
        printf("Error: ID '%s' no encontrado en credenciales.txt\n", id);
        return 0;
    }
    for (int i = 0; i < lista->numUsuarios; i++) {
        if (strcmp(lista->lista[i].id, id) == 0) {
            printf("Error: ese ID ya fue registrado.\n");
            return 0;
        }
    }

    leerLinea("  Nombre de usuario  : ", nombre, sizeof(nombre));
    if (strlen(nombre) < 3) {
        printf("Error: nombre muy corto (minimo 3 caracteres).\n");
        return 0;
    }
    if (buscarUsuario(lista, nombre)) {
        printf("Error: nombre de usuario ya existe.\n");
        return 0;
    }

    leerPassword("  Password           : ", password, sizeof(password));
    if (strlen(password) < 4) {
        printf("Error: password muy corta (minimo 4 caracteres).\n");
        return 0;
    }
    leerPassword("  Confirmar password : ", confirm,  sizeof(confirm));
    if (strcmp(password, confirm) != 0) {
        printf("Error: las passwords no coinciden.\n");
        return 0;
    }

    leerPassword("  Key de acceso      : ", key, sizeof(key));
    RolUsuario rol;
    if (strcmp(key, KEY_ADMIN) == 0) {
        rol = ROL_ADMIN;
    } else if (strcmp(key, KEY_COMUN) == 0) {
        rol = ROL_COMUN;
    } else {
        printf("Error: key invalida.\n");
        return 0;
    }

    Usuario u = crearUsuario(id, nombre, password, rol);
    agregarUsuario(lista, u);

    printf("\nCuenta creada exitosamente.\n");
    printf("  Usuario : %s\n", nombre);
    printf("  ID      : %s\n", id);
    printf("  Tipo    : %s\n",
           rol == ROL_ADMIN ? "admin" : "comun");
    return 1;
}
