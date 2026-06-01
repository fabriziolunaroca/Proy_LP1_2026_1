#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/nodo.h"
#include "include/usuario.h"
#include "include/auth.h"
#include "include/filesystem.h"
#include "include/permisos.h"
#include "include/utils.h"
#include "include/persistencia.h"

#define MAX_CMD 512

static void imprimirMenu(void) {
    printf("\n");
    printf("+==============================+\n");
    printf("|     SISTEMA DE ARCHIVOS v1.0   |\n");
    printf("+==============================+\n");
    printf("|  1. Iniciar sesion               |\n");
    printf("|  2. Crear cuenta                 |\n");
    printf("|  3. Exit                         |\n");
    printf("+==============================+\n");
    printf("\n");
}

static void imprimirPrompt(Usuario *u, Nodo *actual, Nodo *root) {
    char ruta[512];
    construirRutaAbsoluta(actual, root, ruta, sizeof(ruta));
    printf("%s-%s/%s:%s$ ",
           u->id, u->usuario,
           (u->rol == ROL_ADMIN ? "admin" : "comun"),
           ruta);
    fflush(stdout);
}

static void guardarTodo(ListaUsuarios *lista, Nodo *root) {
    guardarUsuarios(lista);
    guardarFilesystem(root);
}

static void bucleFS(Usuario *usuario, Nodo *root, Nodo *userDir,
                    ListaUsuarios *lista) {
    Nodo *actual = userDir;
    char  linea[MAX_CMD];
    char  cmd[64];
    char  arg1[512];

    printf("\nBienvenido, %s [%s]\n",
           usuario->usuario,
           usuario->rol == ROL_ADMIN ? "admin" : "comun");
    printf("Comandos: ls  cd  mkdir  touch  write  cat  rm  pwd  whoami  clear  help  logout\n\n");

    while (1) {
        imprimirPrompt(usuario, actual, root);

        if (!fgets(linea, sizeof(linea), stdin)) break;
        linea[strcspn(linea, "\n")] = '\0';
        if (strlen(linea) == 0) continue;

        cmd[0] = arg1[0] = '\0';
        sscanf(linea, "%63s", cmd);

        if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "logout") == 0) {
            guardarTodo(lista, root);
            printf("\n[guardado] Sesion cerrada. Adios, %s.\n\n",
                   usuario->usuario);
            break;

        } else if (strcmp(cmd, "pwd") == 0) {
            char ruta[512];
            construirRutaAbsoluta(actual, root, ruta, sizeof(ruta));
            printf("%s\n", ruta);

        } else if (strcmp(cmd, "whoami") == 0) {
            printf("  usuario : %s\n", usuario->usuario);
            printf("  id      : %s\n", usuario->id);
            printf("  rol     : %s\n",
                   usuario->rol == ROL_ADMIN ? "admin" : "comun");

        } else if (strcmp(cmd, "ls") == 0) {
            char ruta[512];
            construirRutaAbsoluta(actual, root, ruta, sizeof(ruta));
            if (sscanf(linea, "%*s %511s", arg1) == 1)
                fs_ls(arg1, actual, root);
            else
                fs_ls(ruta, actual, root);

        } else if (strcmp(cmd, "cd") == 0) {
            if (sscanf(linea, "%*s %511s", arg1) != 1)
                printf("Uso: cd <ruta>\n");
            else
                actual = fs_cd(arg1, actual, root, usuario, userDir);

        } else if (strcmp(cmd, "mkdir") == 0) {
            if (sscanf(linea, "%*s %511s", arg1) != 1)
                printf("Uso: mkdir <nombre>\n");
            else {
                char padreRuta[512], nombre[MAX_NOMBRE];
                separarRuta(arg1, padreRuta, nombre);
                Nodo *destPadre = resolverRuta(padreRuta, actual, root);
                if (!destPadre || !tienePermiso(destPadre, userDir, usuario))
                    printf("Permiso denegado.\n");
                else {
                    fs_mkdir(arg1, actual, root);
                    guardarTodo(lista, root);
                }
            }

        } else if (strcmp(cmd, "touch") == 0) {
            if (sscanf(linea, "%*s %511s", arg1) != 1)
                printf("Uso: touch <nombre>\n");
            else {
                char padreRuta[512], nombre[MAX_NOMBRE];
                separarRuta(arg1, padreRuta, nombre);
                Nodo *destPadre = resolverRuta(padreRuta, actual, root);
                if (!destPadre || !tienePermiso(destPadre, userDir, usuario))
                    printf("Permiso denegado.\n");
                else {
                    fs_touch(arg1, actual, root);
                    guardarTodo(lista, root);
                }
            }

        } else if (strcmp(cmd, "cat") == 0) {
            if (sscanf(linea, "%*s %511s", arg1) != 1)
                printf("Uso: cat <archivo>\n");
            else {
                Nodo *dest = resolverRuta(arg1, actual, root);
                if (!dest || !tienePermiso(dest, userDir, usuario))
                    printf("Permiso denegado.\n");
                else
                    fs_cat(arg1, actual, root);
            }

        } else if (strcmp(cmd, "write") == 0) {
            char archivo[512];
            char texto[MAX_CONTENIDO];
            if (sscanf(linea, "%*s %511s \"%4095[^\"]\"", archivo, texto) != 2)
                printf("Uso: write <archivo> \"texto\"\n");
            else {
                Nodo *dest = resolverRuta(archivo, actual, root);
                if (!dest || !tienePermiso(dest, userDir, usuario))
                    printf("Permiso denegado.\n");
                else {
                    fs_write(archivo, texto, actual, root);
                    guardarTodo(lista, root);
                }
            }

        } else if (strcmp(cmd, "rm") == 0) {
            if (sscanf(linea, "%*s %511s", arg1) != 1)
                printf("Uso: rm <ruta>\n");
            else {
                Nodo *dest = resolverRuta(arg1, actual, root);
                if (!dest || !tienePermiso(dest, userDir, usuario))
                    printf("Permiso denegado.\n");
                else {
                    fs_rm(arg1, actual, root);
                    guardarTodo(lista, root);
                }
            }

        } else if (strcmp(cmd, "clear") == 0) {
            limpiarPantalla();

        } else if (strcmp(cmd, "help") == 0) {
            printf("\nComandos disponibles:\n");
            printf("  ls [ruta]            listar contenido\n");
            printf("  cd <ruta>            cambiar directorio\n");
            printf("  mkdir <nombre>       crear directorio\n");
            printf("  touch <nombre>       crear archivo vacio\n");
            printf("  write <arch> \"texto\" escribir en archivo\n");
            printf("  cat <archivo>        leer archivo\n");
            printf("  rm <ruta>            eliminar\n");
            printf("  pwd                  ruta actual\n");
            printf("  whoami               info del usuario\n");
            printf("  clear                limpiar pantalla\n");
            printf("  logout / exit        cerrar sesion\n\n");

        } else {
            printf("%s: comando no encontrado\n", cmd);
        }
    }
}

int main(void) {
    ListaUsuarios lista = { .numUsuarios = 0 };

    /* -- Cargar datos guardados -- */
    Nodo *root = cargarFilesystem();
    if (!root) {
        /* Primera vez: crear arbol inicial */
        root = crearNodo("/", DIRECTORIO);
        Nodo *papeleria = crearNodo("papeleria", DIRECTORIO);
        Nodo *userDir   = crearNodo("user",      DIRECTORIO);
        agregarHijo(root, papeleria);
        agregarHijo(root, userDir);
        printf("Sistema iniciado por primera vez.\n");
    } else {
        printf("Datos cargados correctamente.\n");
    }
    cargarUsuarios(&lista);

    Nodo *userDir = buscarHijo(root, "user");
    if (!userDir) {
        userDir = crearNodo("user", DIRECTORIO);
        agregarHijo(root, userDir);
    }

    char opcion[8];

    while (1) {
        imprimirMenu();
        leerLinea("  Selecciona [1-3]: ", opcion, sizeof(opcion));

        if (strcmp(opcion, "1") == 0) {
            char nombre[MAX_USUARIO];
            char password[MAX_PASSWORD];
            Usuario *u = NULL;

            printf("\n-- Iniciar sesion --\n");
            leerLinea   ("  Usuario  : ", nombre,   sizeof(nombre));
            leerPassword("  Password : ", password, sizeof(password));

            if (loginUsuario(&lista, nombre, password, &u)) {
                Nodo *suDir = buscarHijo(userDir, u->usuario);
                if (!suDir) {
                    suDir = crearNodo(u->usuario, DIRECTORIO);
                    agregarHijo(userDir, suDir);
                    guardarTodo(&lista, root);
                }
                bucleFS(u, root, suDir, &lista);
            } else {
                printf("\nError: usuario o password incorrectos.\n");
            }

        } else if (strcmp(opcion, "2") == 0) {
            int ok = registrarUsuario(&lista);
            if (ok) {
                const char *nuevoNombre = lista.lista[lista.numUsuarios - 1].usuario;
                if (!buscarHijo(userDir, nuevoNombre)) {
                    Nodo *nd = crearNodo(nuevoNombre, DIRECTORIO);
                    agregarHijo(userDir, nd);
                }
                guardarTodo(&lista, root);
                printf("[guardado]\n");
            }

        } else if (strcmp(opcion, "3") == 0) {
            guardarTodo(&lista, root);
            printf("\n[guardado] Saliendo del sistema. Hasta luego.\n\n");
            break;

        } else {
            printf("\nOpcion invalida. Intenta de nuevo.\n");
        }
    }

    eliminarRecursivo(root);
    return 0;
}
