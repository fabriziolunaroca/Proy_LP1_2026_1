#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "nodo.h"
#include "usuario.h"

Nodo *resolverRuta(const char *ruta, Nodo *actual, Nodo *root);
void  separarRuta(const char *ruta, char *padreRuta, char *nombre);

void  fs_mkdir(const char *ruta, Nodo *actual, Nodo *root);
void  fs_touch(const char *ruta, Nodo *actual, Nodo *root);
void  fs_ls   (const char *ruta, Nodo *actual, Nodo *root);
void  fs_cat  (const char *ruta, Nodo *actual, Nodo *root);
void  fs_write(const char *ruta, const char *texto, Nodo *actual, Nodo *root);
void  fs_rm   (const char *ruta, Nodo *actual, Nodo *root);
Nodo *fs_cd   (const char *ruta, Nodo *actual, Nodo *root,
                Usuario *usuario, Nodo *userDir);

void  construirRutaAbsoluta(Nodo *nodo, Nodo *root, char *buf, int bufSize);

#endif
