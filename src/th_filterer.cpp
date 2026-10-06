// Diseño 3 (hilos): la imagen se divide en cuatro regiones y cada hilo filtra una.
// Uso: ./th_filterer fruit.pgm fruit_blur2.pgm --f blur
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <thread>
#include "imagen.h"
#include "filtros.h"
#include "argumentos.h"

using namespace std;

const int NUM_HILOS = 4;

// Trabajo de cada hilo: aplicar el filtro a su región
void filtrarRegion(const Filtro& filtro, const Imagen& entrada, Imagen& salida,
                   int x0, int y0, int x1, int y1) {
    filtro.aplicar(entrada, salida, x0, y0, x1, y1);
}

int main(int argc, char* argv[]) {
    auto inicioPrograma = chrono::high_resolution_clock::now();

    const Filtro* filtros[NUM_FILTROS];
    int cantidad = elegirFiltros(argc, argv, filtros);
    if (cantidad == 0) {
        mostrarUso(argv[0]);
        return 1;
    }

    Imagen entrada;
    if (!entrada.leer(argv[1])) {
        cout << "Error leyendo " << argv[1] << endl;
        return 1;
    }
    Imagen salida;
    salida.crearComo(entrada);

    // Cuatro regiones {x0, y0, x1, y1}
    int mitadX = entrada.ancho / 2;
    int mitadY = entrada.alto / 2;
    int regiones[NUM_HILOS][4] = {
        {0,      0,      mitadX,        mitadY},         // arriba-izquierda
        {mitadX, 0,      entrada.ancho, mitadY},         // arriba-derecha
        {0,      mitadY, mitadX,        entrada.alto},   // abajo-izquierda
        {mitadX, mitadY, entrada.ancho, entrada.alto}    // abajo-derecha
    };

    cout << fixed << setprecision(6);
    cout << "Hilos x" << NUM_HILOS << " | " << argv[1] << " " << entrada.ancho << "x" << entrada.alto
         << ", " << entrada.canales << " canal(es)" << endl;

    double sumaReal = 0, sumaCPU = 0;
    for (int f = 0; f < cantidad; f++) {
        auto inicioReal = chrono::high_resolution_clock::now();
        clock_t inicioCPU = clock();

        thread hilos[NUM_HILOS];
        for (int h = 0; h < NUM_HILOS; h++)
            hilos[h] = thread(filtrarRegion, ref(*filtros[f]), ref(entrada), ref(salida),
                              regiones[h][0], regiones[h][1], regiones[h][2], regiones[h][3]);

        // Esperar a que los cuatro terminen antes de guardar
        for (int h = 0; h < NUM_HILOS; h++)
            hilos[h].join();

        clock_t finCPU = clock();
        auto finReal = chrono::high_resolution_clock::now();
        double tiempoReal = chrono::duration<double>(finReal - inicioReal).count();
        double tiempoCPU = double(finCPU - inicioCPU) / CLOCKS_PER_SEC;
        sumaReal += tiempoReal;
        sumaCPU += tiempoCPU;

        char ruta[MAX_RUTA];
        nombreSalida(argv[2], filtros[f]->nombre, cantidad, ruta);
        salida.guardar(ruta);

        cout << "  " << setw(8) << left << filtros[f]->nombre << right
             << " tiempo real = " << tiempoReal << " s | tiempo CPU = " << tiempoCPU << " s" << endl;
    }

    double tiempoTotal = chrono::duration<double>(chrono::high_resolution_clock::now() - inicioPrograma).count();
    cout << "  filtrado  tiempo real = " << sumaReal << " s | tiempo CPU = " << sumaCPU << " s" << endl;
    cout << "  programa completo (leer + filtrar + escribir) = " << tiempoTotal << " s" << endl;
    return 0;
}
