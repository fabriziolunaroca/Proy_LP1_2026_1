"""
Visor Prometeus — Sistema de Archivos Virtual (interfaz Tkinter)
==================================================================

Lee el estado del sistema de archivos desde data/snapshot.txt,
generado automaticamente por el nucleo en C cada vez que invoca
guardarTodo() (Arquitectura D).

La funcion cargar_snapshot() reconstruye el arbol jerarquico en
memoria a partir del formato de texto plano:

    D|nivel|nombre
    F|nivel|nombre|size
    contenido (cero o mas lineas)
    ~~~

Si snapshot.txt no existe o esta vacio, carga un arbol raiz vacio
y muestra un mensaje en la barra de estado, sin lanzar excepciones.

Las operaciones de escritura (mkdir, touch, write, rm) y la
actualizacion automatica via after() seran implementadas en la
siguiente iteracion.

Requiere unicamente la biblioteca estandar de Python (tkinter + os),
sin dependencias externas.
"""

import tkinter as tk
from tkinter import ttk, messagebox
import os


# ──────────────────────────────────────────────────────────────────
# RUTA DEL SNAPSHOT  (debe coincidir con RUTA_SNAPSHOT en persistencia.h)
# ──────────────────────────────────────────────────────────────────
# Se resuelve relativa al directorio desde el que se lanza el visor.
# Si el visor se ejecuta desde la raiz del proyecto C
# (Proyecto_Prometeus_fw/), la ruta "data/snapshot.txt" apunta al
# mismo archivo que genera exportarSnapshot() en C.
RUTA_SNAPSHOT = "data/snapshot.txt"


# ──────────────────────────────────────────────────────────────────
# CARGA DEL SNAPSHOT
# ──────────────────────────────────────────────────────────────────

def cargar_snapshot(ruta=RUTA_SNAPSHOT):
    """Lee data/snapshot.txt y devuelve el arbol jerarquico como un
    diccionario anidado con la misma forma que antes tenia ARBOL_SIMULADO:

        {
            "nombre": str,
            "tipo":   "dir" | "file",
            "hijos":  [...]   # solo si tipo == "dir"
            "contenido": str  # solo si tipo == "file"
        }

    Algoritmo de reconstruccion con pila
    ─────────────────────────────────────
    El snapshot esta escrito en pre-orden: primero el padre, luego sus
    hijos, luego los hijos de los hijos. Cada linea lleva su nivel de
    profundidad explicitamente, lo que permite reconstruir la jerarquia
    en una unica pasada lineal usando una pila indexada por nivel:

      pila[nivel] = nodo directorio "abierto" (aun puede recibir hijos)

    Al procesar un nodo de nivel N:
      1. Truncar la pila a longitud N  (cierra todos los directorios
         de nivel >= N que ya estan completos).
      2. Crear el diccionario del nuevo nodo.
      3. Si N > 0: agregarlo como hijo de pila[N-1].
      4. Si es directorio: empujarlo en pila[N] (puede recibir hijos).

    Devuelve una raiz minima valida si el archivo no existe o esta vacio,
    para que la GUI pueda abrirse siempre sin lanzar excepciones.
    """
    raiz_vacia = {"nombre": "/", "tipo": "dir", "hijos": []}

    # ── intentar abrir el archivo ────────────────────────────────
    try:
        f = open(ruta, encoding="utf-8")
    except FileNotFoundError:
        return raiz_vacia, "snapshot.txt no encontrado en '{}'.".format(ruta)
    except OSError as e:
        return raiz_vacia, "No se pudo abrir snapshot.txt: {}".format(e)

    with f:
        lineas = f.readlines()

    if not lineas:
        return raiz_vacia, "snapshot.txt esta vacio."

    # ── reconstruccion con pila ──────────────────────────────────
    pila = []          # pila[nivel] = nodo directorio abierto en ese nivel
    raiz = None
    i = 0
    n = len(lineas)

    while i < n:
        linea = lineas[i].rstrip("\n\r")
        i += 1

        if not linea:
            continue

        partes = linea.split("|")

        # ── directorio: D|nivel|nombre ───────────────────────────
        if partes[0] == "D" and len(partes) >= 3:
            nivel = int(partes[1])
            nombre = partes[2]

            nodo = {"nombre": nombre, "tipo": "dir", "hijos": []}

            # truncar pila: cierra directorios de nivel >= nivel actual
            pila = pila[:nivel]

            if nivel == 0:
                raiz = nodo          # este es el nodo raiz ("/")
            else:
                pila[nivel - 1]["hijos"].append(nodo)

            pila.append(nodo)        # pila[nivel] = este directorio

        # ── archivo: F|nivel|nombre|size ─────────────────────────
        elif partes[0] == "F" and len(partes) >= 4:
            nivel  = int(partes[1])
            nombre = partes[2]
            size   = int(partes[3])

            # leer contenido hasta encontrar la linea "~~~"
            lineas_contenido = []
            while i < n:
                linea_cont = lineas[i].rstrip("\n\r")
                i += 1
                if linea_cont == "~~~":
                    break
                lineas_contenido.append(linea_cont)

            # reconstruir el contenido como string
            # (si size == 0 el bloque estara vacio, pero ~~~
            #  siempre aparece, asi que lineas_contenido = [])
            contenido = "\n".join(lineas_contenido) if size > 0 else ""

            nodo = {"nombre": nombre, "tipo": "file", "contenido": contenido}

            # truncar pila: los archivos no se apilan (no tienen hijos),
            # pero si un archivo sigue a otro del mismo nivel, la pila
            # ya tiene la longitud correcta; solo nos aseguramos de ello.
            pila = pila[:nivel]

            if pila:
                pila[nivel - 1]["hijos"].append(nodo)
            # si nivel == 0 (archivo en la raiz, caso raro pero valido):
            # no hay pila[nivel-1], el archivo queda suelto; en la practica
            # el nucleo en C siempre tiene "/" como raiz de nivel 0.

    if raiz is None:
        return raiz_vacia, "snapshot.txt no contiene ningun nodo valido."

    return raiz, None   # None = sin mensaje de error

# Usuario simulado actualmente "logueado". Mas adelante este dato
# vendra de la pantalla de login (fuera del alcance de este archivo).
USUARIO_ACTUAL = {
    "usuario": "fabrizio",
    "id": "1234",
    "rol": "comun",  # "comun" o "admin"
}


# ──────────────────────────────────────────────────────────────────
# VENTANA PRINCIPAL
# ──────────────────────────────────────────────────────────────────

class VisorPrometeus(tk.Tk):
    """Ventana principal del visor. Construye los cuatro paneles
    definidos en el diseño conceptual: barra de menu, arbol lateral,
    panel de contenido y barra de estado inferior."""

    def __init__(self):
        super().__init__()

        self.title("Visor Prometeus — Sistema de Archivos Virtual")
        self.geometry("900x560")
        self.minsize(700, 420)

        # Nodo raiz actual devuelto por cargar_snapshot().
        # Se guarda como atributo para que _ruta_de_item() pueda
        # usarlo como centinela de identidad (nodo is self._raiz_arbol)
        # sin depender de una constante global.
        self._raiz_arbol, _error_carga = cargar_snapshot()

        # Nodo actualmente seleccionado en el arbol, usado por el
        # panel de contenido y los menus contextuales.
        self.nodo_seleccionado = None

        # Mapea cada item_id del Treeview a su nodo de datos, para
        # poder recuperar el diccionario original al hacer clic.
        self.item_a_nodo = {}

        # Indica si el panel de contenido tiene cambios sin guardar
        # (solo relevante cuando se entra en modo edicion con doble clic).
        self.hay_cambios_sin_guardar = False

        self._construir_barra_menu()
        self._construir_cuerpo()
        self._construir_barra_estado()

        self._poblar_arbol()

        if _error_carga:
            self._mostrar_estado(
                "⚠  {} Ejecuta el sistema en C primero para generar snapshot.txt.".format(
                    _error_carga
                ),
                tipo="advertencia",
            )
        else:
            self._mostrar_estado(
                "Árbol cargado desde {}.".format(RUTA_SNAPSHOT), tipo="exito"
            )

    # ── BARRA DE MENU SUPERIOR ──────────────────────────────────

    def _construir_barra_menu(self):
        barra = tk.Frame(self, bd=1, relief=tk.RAISED)
        barra.pack(side=tk.TOP, fill=tk.X)

        # Menu "Archivo"
        menu_archivo_btn = tk.Menubutton(barra, text="Archivo", relief=tk.FLAT)
        menu_archivo = tk.Menu(menu_archivo_btn, tearoff=0)
        menu_archivo.add_command(label="Actualizar", command=self.accion_actualizar)
        menu_archivo.add_separator()
        menu_archivo.add_command(label="Cerrar sesion", command=self.accion_cerrar_sesion)
        menu_archivo_btn.config(menu=menu_archivo)
        menu_archivo_btn.pack(side=tk.LEFT, padx=2, pady=2)

        # Menu "Editar"
        menu_editar_btn = tk.Menubutton(barra, text="Editar", relief=tk.FLAT)
        menu_editar = tk.Menu(menu_editar_btn, tearoff=0)
        menu_editar.add_command(label="Nueva carpeta", command=self.accion_nueva_carpeta)
        menu_editar.add_command(label="Nuevo archivo", command=self.accion_nuevo_archivo)
        menu_editar.add_separator()
        menu_editar.add_command(label="Eliminar", command=self.accion_eliminar)
        menu_editar_btn.config(menu=menu_editar)
        menu_editar_btn.pack(side=tk.LEFT, padx=2, pady=2)

        # Menu "Ver"
        menu_ver_btn = tk.Menubutton(barra, text="Ver", relief=tk.FLAT)
        menu_ver = tk.Menu(menu_ver_btn, tearoff=0)
        menu_ver.add_command(label="Expandir todo", command=self.accion_expandir_todo)
        menu_ver.add_command(label="Colapsar todo", command=self.accion_colapsar_todo)
        menu_ver_btn.config(menu=menu_ver)
        menu_ver_btn.pack(side=tk.LEFT, padx=2, pady=2)

        # Indicador de usuario / rol, alineado a la derecha
        texto_usuario = "{}  [{}]".format(
            USUARIO_ACTUAL["usuario"], USUARIO_ACTUAL["rol"]
        )
        etiqueta_usuario = tk.Label(barra, text=texto_usuario, fg="#444")
        etiqueta_usuario.pack(side=tk.RIGHT, padx=10)

        # Campo de busqueda
        tk.Label(barra, text="Buscar:").pack(side=tk.RIGHT, padx=(10, 2))
        self.var_busqueda = tk.StringVar()
        entrada_busqueda = tk.Entry(barra, textvariable=self.var_busqueda, width=18)
        entrada_busqueda.pack(side=tk.RIGHT, padx=2)
        entrada_busqueda.bind("<Return>", lambda e: self.accion_buscar())

    # ── CUERPO: ARBOL (IZQUIERDA) + CONTENIDO (CENTRO) ──────────

    def _construir_cuerpo(self):
        cuerpo = tk.PanedWindow(self, orient=tk.HORIZONTAL, sashwidth=4)
        cuerpo.pack(side=tk.TOP, fill=tk.BOTH, expand=True)

        # --- Panel izquierdo: arbol de directorios ---
        panel_izquierdo = tk.Frame(cuerpo)
        tk.Label(
            panel_izquierdo, text="ÁRBOL DE DIRECTORIOS",
            anchor="w", font=("TkDefaultFont", 9, "bold")
        ).pack(side=tk.TOP, fill=tk.X, padx=4, pady=(4, 2))

        self.arbol = ttk.Treeview(panel_izquierdo, show="tree")
        self.arbol.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=(4, 0), pady=(0, 4))

        scroll_arbol = ttk.Scrollbar(
            panel_izquierdo, orient=tk.VERTICAL, command=self.arbol.yview
        )
        self.arbol.configure(yscrollcommand=scroll_arbol.set)
        scroll_arbol.pack(side=tk.LEFT, fill=tk.Y, pady=(0, 4))

        self.arbol.bind("<<TreeviewSelect>>", self._al_seleccionar_nodo)
        self.arbol.bind("<Double-1>", self._al_doble_clic)
        self.arbol.bind("<Button-3>", self._al_clic_derecho)  # Linux/Windows
        self.arbol.bind("<Button-2>", self._al_clic_derecho)  # macOS (algunos casos)

        cuerpo.add(panel_izquierdo, minsize=220)

        # --- Panel central: contenido / propiedades ---
        panel_central = tk.Frame(cuerpo)
        tk.Label(
            panel_central, text="CONTENIDO",
            anchor="w", font=("TkDefaultFont", 9, "bold")
        ).pack(side=tk.TOP, fill=tk.X, padx=4, pady=(4, 2))

        # Ficha de propiedades (nombre, ruta, tipo, tamaño/elementos)
        self.frame_propiedades = tk.Frame(panel_central, bd=1, relief=tk.GROOVE)
        self.frame_propiedades.pack(side=tk.TOP, fill=tk.X, padx=4, pady=(0, 4))

        self.lbl_icono_nombre = tk.Label(
            self.frame_propiedades, text="", anchor="w",
            font=("TkDefaultFont", 10, "bold")
        )
        self.lbl_icono_nombre.pack(side=tk.TOP, fill=tk.X, padx=6, pady=(4, 0))

        self.lbl_prop_ruta = tk.Label(self.frame_propiedades, text="", anchor="w", fg="#555")
        self.lbl_prop_ruta.pack(side=tk.TOP, fill=tk.X, padx=6)

        self.lbl_prop_tipo = tk.Label(self.frame_propiedades, text="", anchor="w", fg="#555")
        self.lbl_prop_tipo.pack(side=tk.TOP, fill=tk.X, padx=6)

        self.lbl_prop_extra = tk.Label(self.frame_propiedades, text="", anchor="w", fg="#555")
        self.lbl_prop_extra.pack(side=tk.TOP, fill=tk.X, padx=6, pady=(0, 4))

        # Area de texto para contenido de archivo
        frame_texto = tk.Frame(panel_central)
        frame_texto.pack(side=tk.TOP, fill=tk.BOTH, expand=True, padx=4)

        self.texto_contenido = tk.Text(
            frame_texto, wrap="word", state="disabled",
            bd=1, relief=tk.SUNKEN, padx=6, pady=6
        )
        self.texto_contenido.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        scroll_texto = ttk.Scrollbar(
            frame_texto, orient=tk.VERTICAL, command=self.texto_contenido.yview
        )
        self.texto_contenido.configure(yscrollcommand=scroll_texto.set)
        scroll_texto.pack(side=tk.LEFT, fill=tk.Y)

        self.texto_contenido.bind("<<Modified>>", self._al_modificar_texto)

        # Botones de guardado (solo visibles/activos en modo edicion)
        frame_botones_guardado = tk.Frame(panel_central)
        frame_botones_guardado.pack(side=tk.TOP, fill=tk.X, padx=4, pady=4)

        self.btn_guardar = tk.Button(
            frame_botones_guardado, text="Guardar cambios",
            command=self.accion_guardar_cambios, state="disabled"
        )
        self.btn_guardar.pack(side=tk.LEFT)

        self.btn_descartar = tk.Button(
            frame_botones_guardado, text="Cerrar sin guardar",
            command=self.accion_descartar_cambios, state="disabled"
        )
        self.btn_descartar.pack(side=tk.LEFT, padx=(6, 0))

        cuerpo.add(panel_central)

    # ── BARRA DE ESTADO INFERIOR ─────────────────────────────────

    def _construir_barra_estado(self):
        barra = tk.Frame(self, bd=1, relief=tk.SUNKEN)
        barra.pack(side=tk.BOTTOM, fill=tk.X)

        self.lbl_estado_ruta = tk.Label(barra, text="Ruta: /", anchor="w")
        self.lbl_estado_ruta.pack(side=tk.LEFT, padx=6, pady=2)

        self.lbl_estado_rol = tk.Label(
            barra, text="Rol: {}".format(USUARIO_ACTUAL["rol"]), anchor="w"
        )
        self.lbl_estado_rol.pack(side=tk.LEFT, padx=6, pady=2)

        self.lbl_estado_conteo = tk.Label(barra, text="", anchor="w")
        self.lbl_estado_conteo.pack(side=tk.LEFT, padx=6, pady=2)

        self.lbl_estado_mensaje = tk.Label(barra, text="", anchor="e", fg="#1a7f37")
        self.lbl_estado_mensaje.pack(side=tk.RIGHT, padx=6, pady=2)

    # ── CONSTRUCCION DEL ARBOL A PARTIR DE DATOS SIMULADOS ───────

    def _poblar_arbol(self):
        """Limpia y vuelve a construir el Treeview a partir de
        self._raiz_arbol, que contiene el arbol devuelto por la
        ultima llamada a cargar_snapshot()."""
        self.arbol.delete(*self.arbol.get_children())
        self.item_a_nodo.clear()

        raiz_id = self.arbol.insert(
            "", "end", text=" 🏠  /", open=True
        )
        self.item_a_nodo[raiz_id] = self._raiz_arbol
        self._insertar_hijos(raiz_id, self._raiz_arbol)

    def _insertar_hijos(self, item_padre, nodo_padre):
        hijos = nodo_padre.get("hijos", [])
        # Carpetas primero, despues archivos; cada grupo alfabetico.
        carpetas = sorted(
            [h for h in hijos if h["tipo"] == "dir"], key=lambda n: n["nombre"]
        )
        archivos = sorted(
            [h for h in hijos if h["tipo"] == "file"], key=lambda n: n["nombre"]
        )

        for hijo in carpetas + archivos:
            etiqueta = self._etiqueta_para_nodo(hijo)
            item_id = self.arbol.insert(item_padre, "end", text=etiqueta, open=False)
            self.item_a_nodo[item_id] = hijo
            if hijo["tipo"] == "dir":
                self._insertar_hijos(item_id, hijo)

    @staticmethod
    def _etiqueta_para_nodo(nodo):
        if nodo["tipo"] == "dir":
            return " 📁  " + nodo["nombre"]
        return " 📄  " + nodo["nombre"]

    def _ruta_de_item(self, item_id):
        """Reconstruye la ruta absoluta de un item recorriendo sus
        ancestros en el Treeview (equivalente visual de
        construirRutaAbsoluta() en filesystem.c).
        Se detiene al encontrar el nodo raiz (self._raiz_arbol)."""
        partes = []
        actual = item_id
        while actual:
            nodo = self.item_a_nodo.get(actual)
            if nodo is None:
                break
            if nodo is self._raiz_arbol:
                break
            partes.append(nodo["nombre"])
            actual = self.arbol.parent(actual)
        if not partes:
            return "/"
        return "/" + "/".join(reversed(partes))

    # ── EVENTOS DEL ARBOL ─────────────────────────────────────────

    def _al_seleccionar_nodo(self, event):
        seleccion = self.arbol.selection()
        if not seleccion:
            return
        item_id = seleccion[0]
        nodo = self.item_a_nodo.get(item_id)
        if nodo is None:
            return

        self.nodo_seleccionado = nodo
        self._mostrar_propiedades(nodo, item_id)
        self._mostrar_contenido(nodo, modo_edicion=False)

    def _al_doble_clic(self, event):
        seleccion = self.arbol.selection()
        if not seleccion:
            return
        item_id = seleccion[0]
        nodo = self.item_a_nodo.get(item_id)
        if nodo is None or nodo["tipo"] != "file":
            return  # doble clic en carpeta solo expande/colapsa (comportamiento nativo)
        self._mostrar_contenido(nodo, modo_edicion=True)

    def _al_clic_derecho(self, event):
        item_id = self.arbol.identify_row(event.y)
        if item_id:
            self.arbol.selection_set(item_id)
            nodo = self.item_a_nodo.get(item_id)
        else:
            nodo = None  # clic en area vacia del arbol

        self._mostrar_menu_contextual(event, nodo)

    # ── PANEL DE PROPIEDADES Y CONTENIDO ─────────────────────────

    def _mostrar_propiedades(self, nodo, item_id):
        ruta = self._ruta_de_item(item_id)

        if nodo["tipo"] == "dir":
            icono_nombre = "📁  " + nodo["nombre"]
            tipo_texto = "Tipo:     Directorio"
            num_archivos = sum(1 for h in nodo.get("hijos", []) if h["tipo"] == "file")
            num_carpetas = sum(1 for h in nodo.get("hijos", []) if h["tipo"] == "dir")
            extra_texto = "Elementos: {} ({} archivos, {} carpetas)".format(
                num_archivos + num_carpetas, num_archivos, num_carpetas
            )
        else:
            icono_nombre = "📄  " + nodo["nombre"]
            tipo_texto = "Tipo:     Archivo"
            tam = len(nodo.get("contenido", ""))
            extra_texto = "Tamaño:   {} bytes".format(tam)

        self.lbl_icono_nombre.config(text=icono_nombre)
        self.lbl_prop_ruta.config(text="Ruta:     " + ruta)
        self.lbl_prop_tipo.config(text=tipo_texto)
        self.lbl_prop_extra.config(text=extra_texto)

        self.lbl_estado_ruta.config(text="Ruta: " + ruta)
        if nodo["tipo"] == "dir":
            total = len(nodo.get("hijos", []))
            self.lbl_estado_conteo.config(text="{} elementos".format(total))
        else:
            self.lbl_estado_conteo.config(text="")

    def _mostrar_contenido(self, nodo, modo_edicion):
        self.texto_contenido.config(state="normal")
        self.texto_contenido.delete("1.0", tk.END)

        if nodo["tipo"] == "dir":
            self.texto_contenido.insert(
                "1.0",
                "(Esta es una carpeta. Selecciona un archivo para ver su contenido.)"
            )
            self.texto_contenido.config(state="disabled")
            self._fijar_estado_botones_guardado(activos=False)
            return

        contenido = nodo.get("contenido", "")
        if contenido == "":
            self.texto_contenido.insert("1.0", "(archivo vacío)")
        else:
            self.texto_contenido.insert("1.0", contenido)

        # Evita que la insercion programatica dispare <<Modified>>
        self.texto_contenido.edit_modified(False)
        self.hay_cambios_sin_guardar = False

        if modo_edicion:
            self.texto_contenido.config(state="normal")
        else:
            self.texto_contenido.config(state="disabled")

        self._fijar_estado_botones_guardado(activos=False)

    def _al_modificar_texto(self, event):
        # <<Modified>> se dispara tanto por ediciones del usuario como
        # por inserciones programaticas; edit_modified() lo distingue.
        if self.texto_contenido.edit_modified():
            if self.texto_contenido.cget("state") == "normal":
                self.hay_cambios_sin_guardar = True
                self._fijar_estado_botones_guardado(activos=True)
            self.texto_contenido.edit_modified(False)

    def _fijar_estado_botones_guardado(self, activos):
        estado = "normal" if activos else "disabled"
        self.btn_guardar.config(state=estado)
        self.btn_descartar.config(state=estado)

    # ── MENU CONTEXTUAL ───────────────────────────────────────────

    def _mostrar_menu_contextual(self, event, nodo):
        menu = tk.Menu(self, tearoff=0)

        if nodo is None:
            # Clic en area vacia del arbol
            menu.add_command(label="📁 Nueva carpeta aquí", command=self.accion_nueva_carpeta)
            menu.add_command(label="📄 Nuevo archivo aquí", command=self.accion_nuevo_archivo)
            menu.add_separator()
            menu.add_command(label="🔄 Actualizar", command=self.accion_actualizar)
        elif nodo["tipo"] == "dir":
            menu.add_command(label="📁 Nueva carpeta", command=self.accion_nueva_carpeta)
            menu.add_command(label="📄 Nuevo archivo", command=self.accion_nuevo_archivo)
            menu.add_separator()
            menu.add_command(label="🗑️  Eliminar carpeta", command=self.accion_eliminar)
            menu.add_separator()
            menu.add_command(label="ℹ️  Propiedades", command=self.accion_propiedades)
        else:
            menu.add_command(label="👁️  Ver contenido", command=self.accion_ver_contenido)
            menu.add_separator()
            menu.add_command(label="🗑️  Eliminar archivo", command=self.accion_eliminar)
            menu.add_separator()
            menu.add_command(label="ℹ️  Propiedades", command=self.accion_propiedades)

        menu.tk_popup(event.x_root, event.y_root)
        menu.grab_release()

    # ── ACCIONES (PLACEHOLDERS — SIN BACKEND TODAVÍA) ────────────
    #
    # Estas funciones todavia no leen snapshot.txt ni escriben datos
    # reales. Solo simulan el resultado para validar el flujo visual
    # completo (mensajes de estado, dialogos de confirmacion, etc.)
    # Cuando se conecte la Arquitectura D, cada una llamara a la
    # operacion correspondiente sobre el snapshot real.

    def accion_nueva_carpeta(self):
        self._mostrar_estado(
            "[Simulación] Se crearía una nueva carpeta aquí (mkdir). "
            "Pendiente de conectar con el backend.", tipo="info"
        )

    def accion_nuevo_archivo(self):
        self._mostrar_estado(
            "[Simulación] Se crearía un nuevo archivo aquí (touch). "
            "Pendiente de conectar con el backend.", tipo="info"
        )

    def accion_eliminar(self):
        if self.nodo_seleccionado is None:
            messagebox.showinfo("Eliminar", "Selecciona primero un elemento del árbol.")
            return

        nombre = self.nodo_seleccionado["nombre"]
        if self.nodo_seleccionado["tipo"] == "dir":
            num_elementos = len(self.nodo_seleccionado.get("hijos", []))
            mensaje = (
                "¿Eliminar \"{}\" y todo su contenido ({} elementos)?\n"
                "Esta acción no se puede deshacer."
            ).format(nombre, num_elementos)
        else:
            mensaje = "¿Eliminar el archivo \"{}\"?\nEsta acción no se puede deshacer.".format(nombre)

        confirmar = messagebox.askyesno("Confirmar eliminación", mensaje, icon="warning")
        if confirmar:
            self._mostrar_estado(
                "[Simulación] \"{}\" se eliminaría aquí (rm). "
                "Pendiente de conectar con el backend.".format(nombre),
                tipo="info",
            )

    def accion_ver_contenido(self):
        if self.nodo_seleccionado and self.nodo_seleccionado["tipo"] == "file":
            self._mostrar_contenido(self.nodo_seleccionado, modo_edicion=False)

    def accion_propiedades(self):
        seleccion = self.arbol.selection()
        if not seleccion or self.nodo_seleccionado is None:
            return
        self._mostrar_propiedades(self.nodo_seleccionado, seleccion[0])

    def accion_guardar_cambios(self):
        # Simulacion: en la version final esto llamaria a la operacion
        # equivalente a fs_write() sobre el snapshot/backend real.
        self.texto_contenido.edit_modified(False)
        self.hay_cambios_sin_guardar = False
        self._fijar_estado_botones_guardado(activos=False)
        self._mostrar_estado(
            "[Simulación] Cambios \"guardados\" en memoria local. "
            "Aún no se escribe en el backend en C.", tipo="exito"
        )

    def accion_descartar_cambios(self):
        if self.nodo_seleccionado:
            self._mostrar_contenido(self.nodo_seleccionado, modo_edicion=True)
        self._mostrar_estado("Cambios descartados.", tipo="info")

    def accion_actualizar(self):
        self._raiz_arbol, error = cargar_snapshot()
        self._poblar_arbol()
        if error:
            self._mostrar_estado(
                "⚠  {}".format(error), tipo="advertencia"
            )
        else:
            self._mostrar_estado(
                "Árbol recargado desde {}.".format(RUTA_SNAPSHOT), tipo="exito"
            )

    def accion_expandir_todo(self):
        def expandir(item):
            self.arbol.item(item, open=True)
            for hijo in self.arbol.get_children(item):
                expandir(hijo)
        for raiz in self.arbol.get_children():
            expandir(raiz)

    def accion_colapsar_todo(self):
        def colapsar(item):
            for hijo in self.arbol.get_children(item):
                colapsar(hijo)
            self.arbol.item(item, open=False)
        for raiz in self.arbol.get_children():
            colapsar(raiz)

    def accion_buscar(self):
        consulta = self.var_busqueda.get().strip().lower()
        if not consulta:
            return

        encontrado = None
        for item_id, nodo in self.item_a_nodo.items():
            if consulta in nodo["nombre"].lower():
                encontrado = item_id
                break

        if encontrado:
            # Expande todos los ancestros para que el item sea visible
            actual = self.arbol.parent(encontrado)
            while actual:
                self.arbol.item(actual, open=True)
                actual = self.arbol.parent(actual)
            self.arbol.selection_set(encontrado)
            self.arbol.see(encontrado)
            self._mostrar_estado("Coincidencia encontrada para \"{}\".".format(consulta), tipo="exito")
        else:
            self._mostrar_estado("Sin coincidencias para \"{}\".".format(consulta), tipo="advertencia")

    def accion_cerrar_sesion(self):
        confirmar = messagebox.askyesno(
            "Cerrar sesión", "¿Cerrar sesión y salir del visor?"
        )
        if confirmar:
            self.destroy()

    # ── UTILIDAD: MENSAJES EN LA BARRA DE ESTADO ─────────────────

    def _mostrar_estado(self, mensaje, tipo="info"):
        colores = {
            "info": "#1a73e8",
            "exito": "#1a7f37",
            "advertencia": "#b35900",
            "error": "#c0392b",
        }
        self.lbl_estado_mensaje.config(
            text=mensaje, fg=colores.get(tipo, "#1a73e8")
        )


def main():
    app = VisorPrometeus()
    app.mainloop()


if __name__ == "__main__":
    main()
