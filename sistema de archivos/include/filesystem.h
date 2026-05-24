#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "nodo.h"
#include "usuario.h"

void mkdirFS(char* nombre, Nodo* actual, Usuario user);

void touchFS(char* nombre, Nodo* actual, Usuario user);

void lsFS(Nodo* actual);

Nodo* cdFS(Nodo* actual, char* ruta, Usuario user);

void writeFS(char* nombre, char* texto, Nodo* actual, Usuario user);

void catFS(char* nombre, Nodo* actual);

void rmFS(char* nombre, Nodo* actual, Nodo* papelera, Usuario user);

#endif
