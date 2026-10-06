#ifndef FILTROS_H
#define FILTROS_H

#include <cmath>
#include "imagen.h"

class Filtro {
public:
    const char* nombre;
    int kernel[3][3];
    float factor;

    // Convolución 2D sobre la región [x0, x1) x [y0, y1) de la imagen:
    //     I'(x, y) = factor * suma de K(i, j) * I(x + i, y + j)   con i, j en -1..1
    // Solo lee `entrada` y solo escribe su región de `salida`, por eso varios hilos
    // pueden filtrar regiones distintas al mismo tiempo sin usar mutex.
    void aplicar(const Imagen& entrada, Imagen& salida, int x0, int y0, int x1, int y1) const {
        for (int y = y0; y < y1; y++) {
            for (int x = x0; x < x1; x++) {
                for (int c = 0; c < entrada.canales; c++) {
                    int pos = entrada.indice(x, y, c);

                    // Los píxeles del borde no tienen todos sus vecinos: se copian sin filtrar
                    if (x == 0 || y == 0 || x == entrada.ancho - 1 || y == entrada.alto - 1) {
                        salida.pixeles[pos] = entrada.pixeles[pos];
                        continue;
                    }

                    int suma = 0;
                    for (int j = -1; j <= 1; j++)
                        for (int i = -1; i <= 1; i++)
                            suma += kernel[j + 1][i + 1] * entrada.pixeles[entrada.indice(x + i, y + j, c)];

                    // Redondear y mantener el valor dentro del rango [0, maximo]
                    int valor = (int) round(suma * factor);
                    if (valor < 0) valor = 0;
                    if (valor > entrada.maximo) valor = entrada.maximo;
                    salida.pixeles[pos] = valor;
                }
            }
        }
    }
};

// Filtros vistos en clase (CL9a). Para agregar un filtro basta con agregar una línea.
const int NUM_FILTROS = 5;
const Filtro FILTROS[NUM_FILTROS] = {
    {"blur",    {{ 1,  1,  1}, { 1, 1,  1}, { 1,  1,  1}}, 1.0f / 9},   // suavizado
    {"gauss",   {{ 1,  2,  1}, { 2, 4,  2}, { 1,  2,  1}}, 1.0f / 16},  // desenfoque ponderado
    {"sharpen", {{ 0, -1,  0}, {-1, 5, -1}, { 0, -1,  0}}, 1},          // realce
    {"laplace", {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}}, 1},          // detección de bordes
    {"sobel",   {{-1, -2, -1}, { 0, 0,  0}, { 1,  2,  1}}, 1},          // bordes horizontales
};

#endif
