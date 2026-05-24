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
    printf("\033[1;37m╔══════════════════════════════╗\033[0m\n");
    printf("\033[1;37m║\033[0m  \033[1;36m   SISTEMA DE ARCHIVOS v1.0  \033[0m \033[1;37m║\033[0m\n");
    printf("\033[1;37m╠══════════════════════════════╣\033[0m\n");
    printf("\033[1;37m║\033[0m  \033[1;32m1.\033[0m Iniciar sesion               \033[1;37m║\033[0m\n");
    printf("\033[1;37m║\033[0m  \033[1;32m2.\033[0m Crear cuenta                 \033[1;37m║\033[0m\n");
    printf("\033[1;37m║\033[0m  \033[1;31m3.\033[0m Exit                         \033[1;37m║\033[0m\n");
    printf("\033[1;37m╚══════════════════════════════╝\033[0m\n");
    printf("\n");
}

static void imprimirPrompt(Usuario *u, Nodo *actual, Nodo *root) {
    char ruta[512];
    construirRutaAbsoluta(actual, root, ruta, sizeof(ruta));
    const char *colorRol = (u->rol == ROL_ADMIN) ? "\033[1;31m" : "\033[1;36m";
    printf("\033[1;32m%s-%s\033[0m\033[1;37m/\033[0m%s%s\033[0m\033[1;37m:\033[0m\033[1;34m%s\033[0m$ ",
           u->id, u->usuario,
           colorRol, (u->rol == ROL_ADMIN ? "admin" : "comun"),
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

    printf("\n\033[1;32mBienvenido, %s \033[0m[\033[1;%sm%s\033[0m]\033[0m\n",
           usuario->usuario,
           usuario->rol == ROL_ADMIN ? "31" : "36",
           usuario->rol == ROL_ADMIN ? "admin" : "comun");
    printf("\033[2mComandos: ls  cd  mkdir  touch  write  cat  rm  pwd  whoami  clear  help  logout\033[0m\n\n");

    while (1) {
        imprimirPrompt(usuario, actual, root);

        if (!fgets(linea, sizeof(linea), stdin)) break;
        linea[strcspn(linea, "\n")] = '\0';
        if (strlen(linea) == 0) continue;

        cmd[0] = arg1[0] = '\0';
        sscanf(linea, "%63s", cmd);

        if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "logout") == 0) {
            guardarTodo(lista, root);
            printf("\n\033[1;32m[guardado]\033[0m \033[1;33mSesion cerrada. Adios, %s.\033[0m\n\n",
                   usuario->usuario);
            break;

        } else if (strcmp(cmd, "pwd") == 0) {
            char ruta[512];
            construirRutaAbsoluta(actual, root, ruta, sizeof(ruta));
            printf("\033[1;34m%s\033[0m\n", ruta);

        } else if (strcmp(cmd, "whoami") == 0) {
            printf("  usuario : \033[1;37m%s\033[0m\n", usuario->usuario);
            printf("  id      : \033[1;33m%s\033[0m\n", usuario->id);
            printf("  rol     : \033[1;%sm%s\033[0m\n",
                   usuario->rol == ROL_ADMIN ? "31" : "36",
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
                printf("\033[1;31mUso: cd <ruta>\033[0m\n");
            else
                actual = fs_cd(arg1, actual, root, usuario, userDir);

        } else if (strcmp(cmd, "mkdir") == 0) {
            if (sscanf(linea, "%*s %511s", arg1) != 1)
                printf("\033[1;31mUso: mkdir <nombre>\033[0m\n");
            else {
                char padreRuta[512], nombre[MAX_NOMBRE];
                separarRuta(arg1, padreRuta, nombre);
                Nodo *destPadre = resolverRuta(padreRuta, actual, root);
                if (!destPadre || !tienePermiso(destPadre, userDir, usuario))
                    printf("\033[1;31mPermiso denegado.\033[0m\n");
                else {
                    fs_mkdir(arg1, actual, root);
                    guardarTodo(lista, root);
                }
            }

        } else if (strcmp(cmd, "touch") == 0) {
            if (sscanf(linea, "%*s %511s", arg1) != 1)
                printf("\033[1;31mUso: touch <nombre>\033[0m\n");
            else {
                char padreRuta[512], nombre[MAX_NOMBRE];
                separarRuta(arg1, padreRuta, nombre);
                Nodo *destPadre = resolverRuta(padreRuta, actual, root);
                if (!destPadre || !tienePermiso(destPadre, userDir, usuario))
                    printf("\033[1;31mPermiso denegado.\033[0m\n");
                else {
                    fs_touch(arg1, actual, root);
                    guardarTodo(lista, root);
                }
            }

        } else if (strcmp(cmd, "cat") == 0) {
            if (sscanf(linea, "%*s %511s", arg1) != 1)
                printf("\033[1;31mUso: cat <archivo>\033[0m\n");
            else {
                Nodo *dest = resolverRuta(arg1, actual, root);
                if (!dest || !tienePermiso(dest, userDir, usuario))
                    printf("\033[1;31mPermiso denegado.\033[0m\n");
                else
                    fs_cat(arg1, actual, root);
            }

        } else if (strcmp(cmd, "write") == 0) {
            char archivo[512];
            char texto[MAX_CONTENIDO];
            if (sscanf(linea, "%*s %511s \"%4095[^\"]\"", archivo, texto) != 2)
                printf("\033[1;31mUso: write <archivo> \"texto\"\033[0m\n");
            else {
                Nodo *dest = resolverRuta(archivo, actual, root);
                if (!dest || !tienePermiso(dest, userDir, usuario))
                    printf("\033[1;31mPermiso denegado.\033[0m\n");
                else {
                    fs_write(archivo, texto, actual, root);
                    guardarTodo(lista, root);
                }
            }

        } else if (strcmp(cmd, "rm") == 0) {
            if (sscanf(linea, "%*s %511s", arg1) != 1)
                printf("\033[1;31mUso: rm <ruta>\033[0m\n");
            else {
                Nodo *dest = resolverRuta(arg1, actual, root);
                if (!dest || !tienePermiso(dest, userDir, usuario))
                    printf("\033[1;31mPermiso denegado.\033[0m\n");
                else {
                    fs_rm(arg1, actual, root);
                    guardarTodo(lista, root);
                }
            }

        } else if (strcmp(cmd, "clear") == 0) {
            limpiarPantalla();

        } else if (strcmp(cmd, "help") == 0) {
            printf("\n\033[1;36mComandos disponibles:\033[0m\n");
            printf("  \033[1;32mls\033[0m [ruta]            listar contenido\n");
            printf("  \033[1;32mcd\033[0m <ruta>            cambiar directorio\n");
            printf("  \033[1;32mmkdir\033[0m <nombre>       crear directorio\n");
            printf("  \033[1;32mtouch\033[0m <nombre>       crear archivo vacio\n");
            printf("  \033[1;32mwrite\033[0m <arch> \"texto\" escribir en archivo\n");
            printf("  \033[1;32mcat\033[0m <archivo>        leer archivo\n");
            printf("  \033[1;32mrm\033[0m <ruta>            eliminar\n");
            printf("  \033[1;32mpwd\033[0m                  ruta actual\n");
            printf("  \033[1;32mwhoami\033[0m               info del usuario\n");
            printf("  \033[1;32mclear\033[0m                limpiar pantalla\n");
            printf("  \033[1;32mlogout\033[0m / exit        cerrar sesion\n\n");

        } else {
            printf("\033[1;31m%s\033[0m\033[2m: comando no encontrado\033[0m\n", cmd);
        }
    }
}

int main(void) {
    ListaUsuarios lista = { .numUsuarios = 0 };

    /* ── Cargar datos guardados ── */
    Nodo *root = cargarFilesystem();
    if (!root) {
        /* Primera vez: crear árbol inicial */
        root = crearNodo("/", DIRECTORIO);
        Nodo *papeleria = crearNodo("papeleria", DIRECTORIO);
        Nodo *userDir   = crearNodo("user",      DIRECTORIO);
        agregarHijo(root, papeleria);
        agregarHijo(root, userDir);
        printf("\033[2mSistema iniciado por primera vez.\033[0m\n");
    } else {
        printf("\033[2mDatos cargados correctamente.\033[0m\n");
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

            printf("\n\033[1;36m-- Iniciar sesion --\033[0m\n");
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
                printf("\n\033[1;31mError: usuario o password incorrectos.\033[0m\n");
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
                printf("\033[1;32m[guardado]\033[0m\n");
            }

        } else if (strcmp(opcion, "3") == 0) {
            guardarTodo(&lista, root);
            printf("\n\033[1;32m[guardado]\033[0m \033[1;33mSaliendo del sistema. Hasta luego.\033[0m\n\n");
            break;

        } else {
            printf("\n\033[1;31mOpcion invalida. Intenta de nuevo.\033[0m\n");
        }
    }

    eliminarRecursivo(root);
    return 0;
}
