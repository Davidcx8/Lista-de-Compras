# Lista de Compras

Proyecto de consola en C++ que implementa una lista enlazada simple para gestionar un listado de compras.

**Caracteristicas principales**
- Anadir articulos con nombre y cantidad (detecta duplicados por nombre).
- Listar articulos con indices y resumen de totales.
- Editar, eliminar y ordenar articulos por nombre o cantidad.
- Historial de cambios con deshacer y rehacer.
- Exportar e importar CSV y JSON.
- Persistencia local automatica (carga al iniciar y guarda al modificar).
- Entrada robusta: validacion basica para evitar estados invalidos de `cin`.

**Estructura del repositorio**
- `Elemento.h` / `Elemento.cpp`: nodo de la lista con `nombre`, `cantidad` y puntero al siguiente.
- `Lista.h` / `Lista.cpp`: implementación de la lista enlazada (agregar, eliminar, acceso al primer nodo).
- `main.cpp`: interfaz de consola (menu, interacción y flujo principal).
- `Makefile`: tareas de compilación y un objetivo alternativo `corre` para el corrector.
- `output/`: carpeta de salida para el ejecutable (por defecto requerida por la tarea).

## Requisitos
- Compilador C++ compatible con C++11 o superior (ej.: `g++` en MinGW/Windows, GCC o Clang en Linux/macOS).
- `make` (opcional, se puede compilar manualmente con `g++`).

## Compilación (sistema original: Windows / MinGW)

1. Desde VS Code: usar `Ctrl+Shift+B` y elegir la tarea `build` o `build-and-run`.
2. Línea de comandos (ejemplo):

```bash
# Compilar todos los .cpp y colocar binario en output/
g++ -Wall -Wextra -g3 *.cpp -o output/main.exe

# Ejecutar
./output/main.exe
```

Si el directorio `output/` no existe, créalo antes de enlazar, o modifica la ruta de salida.

## Quickstart

```bash
# 1) Compilar
g++ -Wall -Wextra -g3 *.cpp -o output/main.exe

# 2) Ejecutar
./output/main.exe
```

## Uso

Al ejecutar el binario veras un menu con opciones extendidas:

- `1` Listar articulos: muestra todos los elementos y el resumen.
- `2` Introducir articulo: escribe el nombre (soporta espacios) y la cantidad; si existe, suma la cantidad.
- `3` Eliminar articulo: ingresa el indice mostrado en la lista.
- `4` Editar articulo: cambia nombre y/o cantidad.
- `5` Ordenar lista: por nombre o cantidad.
- `6` Deshacer: revierte el ultimo cambio.
- `7` Rehacer: reaplica el ultimo cambio revertido.
- `8` Exportar: CSV o JSON.
- `9` Importar: CSV o JSON.
- `10` Guardar y salir.

Comportamiento importante:
- Si intentas eliminar un indice invalido, el programa avisa y vuelve al menu.
- Las entradas numericas invalidas se detectan y se solicita reingreso.
- Los cambios se guardan automaticamente en el archivo de datos local.
- Importar permite combinar o reemplazar la lista actual.
- Exportar e importar permiten definir rutas personalizadas (ENTER usa la ruta por defecto).

## Ejemplo de uso (sesión corta)

```
1- Listar articulos
2- Introducir articulo
3- Eliminar articulo
4- Editar articulo
5- Ordenar lista
6- Deshacer
7- Rehacer
8- Exportar
9- Importar
10- Guardar y salir
Elija una opcion => 2
Introducir el nombre del articulo: leche entera
Introduzca la cantidad del nuevo articulo: 2
-- Articulo agregado. Presione ENTER para regresar al menu principal--

Elija una opcion => 1

1- leche entera 2

Total articulos: 1 | Total unidades: 2

Presione ENTER para continuar...
```

## Problemas comunes

- **No se genera el ejecutable**: asegurate de que exista `output/` o ajusta la ruta del binario.
- **El menu se repite sin parar**: suele ocurrir por entradas invalidas; este proyecto limpia el estado de `cin` y pide reintentar.
- **No se conservan los datos**: verifica permisos de escritura en la carpeta del proyecto.
- **No se ven los elementos al listar**: se muestra una pausa al final del listado; presiona ENTER.

## Datos generados

El proyecto guarda y exporta archivos locales en la carpeta de trabajo:

- `lista_compras.txt` (persistencia interna)
- `lista_compras.csv`
- `lista_compras.json`

Los nombres se pueden ajustar en el codigo si lo necesitas, o usar rutas personalizadas al exportar/importar.

## Buenas practicas y extensiones sugeridas
- Exportar e importar desde UI con rutas personalizadas.
- Historial con limites configurables y etiquetas.
- Interfaz: GUI (Qt / wxWidgets) o una API REST ligera.
- Tests: anadir unit tests para `Lista` y `Elemento`.

## Contribución

Si quieres proponer mejoras:
1. Crea una rama (`feat/...` o `fix/...`).
2. Mantén los cambios acotados y documenta el comportamiento nuevo.
3. Abre un PR con un resumen breve y pasos para reproducir.

git push -u origin main
