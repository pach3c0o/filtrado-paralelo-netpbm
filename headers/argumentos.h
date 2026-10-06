#ifndef ARGUMENTOS_H
#define ARGUMENTOS_H

#include <cstdio>
#include <cstring>
#include <iostream>
#include "filtros.h"

using namespace std;

const int MAX_RUTA = 256;

void mostrarUso(const char* programa) {
    cout << "uso: " << programa << " entrada.pgm salida.pgm [--f filtro]..." << endl;
    cout << "filtros: blur gauss sharpen laplace sobel" << endl;
    cout << "sin --f se aplican blur, laplace y sharpen" << endl;
}

const Filtro* buscarFiltro(const char* nombre) {
    for (int i = 0; i < NUM_FILTROS; i++)
        if (strcmp(FILTROS[i].nombre, nombre) == 0)
            return &FILTROS[i];
    return nullptr;
}

// Lee los filtros pedidos con "--f nombre" (se puede repetir).
// Devuelve cuántos filtros quedaron en `elegidos`, o 0 si los argumentos no son válidos.
int elegirFiltros(int argc, char* argv[], const Filtro* elegidos[]) {
    if (argc < 3) return 0;

    int cantidad = 0;
    for (int i = 3; i < argc; i += 2) {
        if (strcmp(argv[i], "--f") != 0 || i + 1 >= argc || cantidad == NUM_FILTROS) return 0;
        elegidos[cantidad] = buscarFiltro(argv[i + 1]);
        if (elegidos[cantidad] == nullptr) {
            cout << "Filtro desconocido: " << argv[i + 1] << endl;
            return 0;
        }
        cantidad++;
    }

    // Sin --f: los tres filtros del enunciado
    if (cantidad == 0) {
        elegidos[0] = buscarFiltro("blur");
        elegidos[1] = buscarFiltro("laplace");
        elegidos[2] = buscarFiltro("sharpen");
        cantidad = 3;
    }
    return cantidad;
}

// Con un solo filtro la salida se usa tal cual; con varios, cada filtro va a
// su propio archivo: salida.pgm -> salida_blur.pgm, salida_laplace.pgm, ...
void nombreSalida(const char* salida, const char* filtro, int cantidad, char* resultado) {
    if (cantidad == 1) {
        strcpy(resultado, salida);
        return;
    }
    const char* punto = strrchr(salida, '.');
    int largoBase = (punto != NULL) ? (int)(punto - salida) : (int) strlen(salida);
    snprintf(resultado, MAX_RUTA, "%.*s_%s%s", largoBase, salida, filtro, (punto != NULL) ? punto : "");
}

#endif
