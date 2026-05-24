#include <stdio.h>
#include <string.h>

#include "include/nodo.h"
#include "include/auth.h"
#include "include/filesystem.h"

int main() {

    Nodo* root = crearNodo("/", DIRECTORIO);
    Nodo* home = crearNodo("home", DIRECTORIO);
    Nodo* papelera = crearNodo("papelera", DIRECTORIO);

    agregarHijo(root, home);
    agregarHijo(root, papelera);

    Usuario user;
    int logged = 0;

    while(!logged) {

        int op;

        printf("\n1. Crear usuario\n2. Login\n3. Exit\n");
        scanf("%d", &op);

        if(op == 1) {

            user = crearUsuario();

            if(strlen(user.username) > 0) {
                guardarUsuario(user);
                printf("Usuario creado\n");
            }

        } else if(op == 2) {

            char u[20], p[20];

            printf("Usuario: ");
            scanf("%s", u);

            printf("Password: ");
            scanf("%s", p);

            if(loginUsuario(u, p)) {
                strcpy(user.username, u);
                logged = 1;
                printf("Login OK\n");
            } else {
                printf("Login FAIL\n");
            }

        } else {
            return 0;
        }
    }

    Nodo* userDir = crearNodo(user.username, DIRECTORIO);
    agregarHijo(home, userDir);

    Nodo* actual = userDir;

    char cmd[50];

    while(1) {

        printf("[%s]$ ", actual->nombre);
        scanf("%s", cmd);

        if(strcmp(cmd, "mkdir") == 0) {

            char n[50];
            scanf("%s", n);
            mkdirFS(n, actual);

        } else if(strcmp(cmd, "touch") == 0) {

            char n[50];
            scanf("%s", n);
            touchFS(n, actual);

        } else if(strcmp(cmd, "ls") == 0) {

            lsFS(actual);

        } else if(strcmp(cmd, "cd") == 0) {

            char r[50];
            scanf("%s", r);
            actual = cdFS(actual, r, user);

        } else if(strcmp(cmd, "write") == 0) {

            char n[50], t[200];

            scanf("%s", n);
            scanf(" %[^\n]", t);

            writeFS(n, t, actual);

        } else if(strcmp(cmd, "cat") == 0) {

            char n[50];
            scanf("%s", n);
            catFS(n, actual);

        } else if(strcmp(cmd, "rm") == 0) {

            char n[50];
            scanf("%s", n);
            rmFS(n, actual, papelera);

        } else if(strcmp(cmd, "exit") == 0) {
            break;
        }
    }

    eliminarNodo(root);
    return 0;
}
