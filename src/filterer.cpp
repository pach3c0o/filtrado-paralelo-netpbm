// Diseño 2: versión secuencial. Un solo hilo filtra toda la imagen.
// Uso: ./filterer fruit.ppm fruit_blur.ppm --f blur
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
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
    cout << "Secuencial | " << argv[1] << " " << entrada.ancho << "x" << entrada.alto
         << ", " << entrada.canales << " canal(es)" << endl;

    double sumaReal = 0, sumaCPU = 0;
    for (int f = 0; f < cantidad; f++) {
        auto inicioReal = chrono::high_resolution_clock::now();
        clock_t inicioCPU = clock();

        filtros[f]->aplicar(entrada, salida, 0, 0, entrada.ancho, entrada.alto);

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
