#include <stdio.h>
#include <string.h>

#include "../include/auth.h"
#include "../include/utils.h"

#define KEY_ADMIN "234600389"
#define KEY_COMUN "456263990"

void hashPassword(char* pass) {

    for(int i = 0; pass[i] != '\0'; i++) {
        pass[i] = pass[i] + 3;
    }
}

int usuarioExiste(char* username) {

    FILE* f = fopen("data/credenciales.txt", "r");
    if(!f) return 0;

    char line[100];
    char fileUser[20];

    while(fgets(line, sizeof(line), f)) {

        sscanf(line, "%[^:]", fileUser);

        if(strcmp(username, fileUser) == 0) {
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}


Usuario crearUsuario() {

    Usuario u;
    char key[20];

    printf("Usuario (1234-5678): ");
    scanf("%s", u.username);

    if(!validarFormatoUsuario(u.username)) {
        printf("Formato invalido\n");
        strcpy(u.username, "");
        return u;
    }

    char prefijo[5];
    strncpy(prefijo, u.username, 4);
    prefijo[4] = '\0';

    if(!credencialExiste(prefijo)) {
        printf("Credencial invalida\n");
        strcpy(u.username, "");
        return u;
    }

    if(usuarioExiste(u.username)) {
        printf("Usuario ya existe\n");
        strcpy(u.username, "");
        return u;
    }

    printf("Password: ");
    scanf("%s", u.password);

    printf("Key: ");
    scanf("%s", key);

    if(strcmp(key, KEY_ADMIN) == 0) {
        u.tipo = ADMIN;
    }
    else if(strcmp(key, KEY_COMUN) == 0) {
        u.tipo = COMUN;
    }
    else {
        printf("Key incorrecta\n");
        strcpy(u.username, "");
        return u;
    }

    return u;
}


void guardarUsuario(Usuario u) {

    FILE* f = fopen("data/credenciales.txt", "a");
    if(!f) return;

    hashPassword(u.password);

    fprintf(f, "%s:%s:%d\n", u.username, u.password, u.tipo);

    fclose(f);
}


int loginUsuario(char* username, char* password) {

    FILE* f = fopen("data/credenciales.txt", "r");
    if(!f) return 0;

    char line[100];
    char fileUser[20];
    char filePass[20];
    int fileTipo;

    char passEnc[20];
    strcpy(passEnc, password);
    hashPassword(passEnc);

    while(fgets(line, sizeof(line), f)) {

        sscanf(line, "%[^:]:%[^:]:%d",
               fileUser, filePass, &fileTipo);

        if(strcmp(username, fileUser) == 0 &&
           strcmp(passEnc, filePass) == 0) {

            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}
