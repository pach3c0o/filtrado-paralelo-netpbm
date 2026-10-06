// Diseño 3 (OpenMP): las filas de la imagen se reparten entre los hilos.
// El número de hilos se elige con la variable OMP_NUM_THREADS.
// Uso: ./omp_filterer sulfur.pgm sulfur_N.pgm
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <omp.h>
#include "imagen.h"
#include "filtros.h"
#include "argumentos.h"

using namespace std;

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

    cout << fixed << setprecision(6);
    cout << "OpenMP x" << omp_get_max_threads() << " | " << argv[1] << " " << entrada.ancho << "x"
         << entrada.alto << ", " << entrada.canales << " canal(es)" << endl;

    double sumaReal = 0, sumaCPU = 0;
    for (int f = 0; f < cantidad; f++) {
        auto inicioReal = chrono::high_resolution_clock::now();
        clock_t inicioCPU = clock();

        // Cada iteración escribe una fila distinta, así que los hilos no se interfieren
        #pragma omp parallel for
        for (int y = 0; y < entrada.alto; y++)
            filtros[f]->aplicar(entrada, salida, 0, y, entrada.ancho, y + 1);

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
