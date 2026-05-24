#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../include/utils.h"

int validarFormatoUsuario(char* user) {

    int len = strlen(user);

    if(len < 6)
        return 0;

    for(int i = 0; i < 4; i++) {
        if(!isdigit(user[i]))
            return 0;
    }

    if(user[4] != '-')
        return 0;

    return 1;
}

int credencialExiste(char* prefijo) {

    FILE* f = fopen("data/credenciales.txt", "r");

    if(f == NULL)
        return 0;

    char line[100];
    char fileUser[20];

    while(fgets(line, sizeof(line), f)) {

        sscanf(line, "%[^:]", fileUser);

        // comparar solo prefijo (4 primeros dígitos)
        if(strncmp(fileUser, prefijo, 4) == 0) {

            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}
int extensionValida(char* nombre) {

    char* ext = strrchr(nombre,'.');

    if(ext == NULL)
        return 0;

    if(strcmp(ext,".txt")==0)
        return 1;

    if(strcmp(ext,".docx")==0)
        return 1;

    if(strcmp(ext,".xlsx")==0)
        return 1;

    return 0;
}
