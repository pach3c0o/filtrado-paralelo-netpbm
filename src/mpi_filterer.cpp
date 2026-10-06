// Diseño 4 (MPI): cada nodo filtra un bloque de filas de la imagen.
//   1. El nodo 0 lee la imagen y envía a cada nodo solo su bloque de filas,
//      más una fila vecina arriba y otra abajo (la convolución 3x3 las necesita).
//   2. Cada nodo filtra sus filas.
//   3. Cada nodo devuelve sus filas filtradas al nodo 0, que guarda el resultado.
// Uso: mpirun -np 3 --host node1,node2,node3 --bind-to none ./mpi_filterer sulfur.pgm sulfur_mpi.pgm
#include <chrono>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mpi.h>
#include "imagen.h"
#include "filtros.h"
#include "argumentos.h"

using namespace std;

// Filas [inicio, fin) que filtra un nodo; el último nodo se queda con las filas sobrantes
void filasDelNodo(int nodo, int nodos, int alto, int& inicio, int& fin) {
    int filasPorNodo = alto / nodos;
    inicio = nodo * filasPorNodo;
    fin = (nodo == nodos - 1) ? alto : inicio + filasPorNodo;
}

// Filas que el nodo necesita recibir: las suyas más una vecina arriba y otra abajo (si existen)
void filasConVecinas(int nodo, int nodos, int alto, int& inicio, int& fin) {
    filasDelNodo(nodo, nodos, alto, inicio, fin);
    if (inicio > 0) inicio--;
    if (fin < alto) fin++;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    auto inicioPrograma = chrono::high_resolution_clock::now();

    int id, nodos;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &nodos);

    char maquina[MPI_MAX_PROCESSOR_NAME];
    int largo;
    MPI_Get_processor_name(maquina, &largo);
    cout << fixed << setprecision(6);

    // Todos los nodos reciben los mismos argumentos
    const Filtro* filtros[NUM_FILTROS];
    int cantidad = elegirFiltros(argc, argv, filtros);
    if (cantidad == 0) {
        if (id == 0) mostrarUso(argv[0]);
        MPI_Finalize();
        return 1;
    }

    // 1. El nodo 0 lee la imagen y avisa a todos su tamaño
    Imagen imagen;
    int tamano[4] = {0, 0, 0, 0};   // ancho, alto, máximo, canales
    if (id == 0 && imagen.leer(argv[1])) {
        tamano[0] = imagen.ancho;
        tamano[1] = imagen.alto;
        tamano[2] = imagen.maximo;
        tamano[3] = imagen.canales;
    }
    MPI_Bcast(tamano, 4, MPI_INT, 0, MPI_COMM_WORLD);
    if (tamano[0] == 0) {
        if (id == 0) cout << "Error leyendo " << argv[1] << endl;
        MPI_Finalize();
        return 1;
    }
    int ancho = tamano[0], alto = tamano[1];
    int valoresPorFila = ancho * tamano[3];

    // Filas propias de este nodo y filas que recibe (con las vecinas)
    int propiaInicio, propiaFin, recibeInicio, recibeFin;
    filasDelNodo(id, nodos, alto, propiaInicio, propiaFin);
    filasConVecinas(id, nodos, alto, recibeInicio, recibeFin);

    // El bloque es una imagen pequeña: solo las filas que este nodo recibe
    Imagen bloque;
    bloque.crear(ancho, recibeFin - recibeInicio, tamano[2], tamano[3]);

    if (id == 0) {
        // El nodo 0 copia su propio bloque y envía el suyo a cada nodo
        memcpy(bloque.pixeles, imagen.pixeles + recibeInicio * valoresPorFila,
               bloque.totalValores() * sizeof(int));
        for (int n = 1; n < nodos; n++) {
            int inicio, fin;
            filasConVecinas(n, nodos, alto, inicio, fin);
            MPI_Send(imagen.pixeles + inicio * valoresPorFila, (fin - inicio) * valoresPorFila,
                     MPI_INT, n, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(bloque.pixeles, bloque.totalValores(), MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    double tiempoLectura = chrono::duration<double>(chrono::high_resolution_clock::now() - inicioPrograma).count();

    // Dentro del bloque, las filas propias empiezan después de la fila vecina de arriba (si la hay)
    int filaLocalInicio = propiaInicio - recibeInicio;
    int filaLocalFin = propiaFin - recibeInicio;
    Imagen bloqueFiltrado;
    bloqueFiltrado.crearComo(bloque);

    // El nodo 0 arma una imagen completa por filtro con las filas de todos
    Imagen salidas[NUM_FILTROS];
    if (id == 0)
        for (int f = 0; f < cantidad; f++)
            salidas[f].crear(ancho, alto, tamano[2], tamano[3]);

    for (int f = 0; f < cantidad; f++) {
        // 2. Filtrar las filas propias (las filas vecinas solo se leen)
        auto inicioReal = chrono::high_resolution_clock::now();
        clock_t inicioCPU = clock();

        filtros[f]->aplicar(bloque, bloqueFiltrado, 0, filaLocalInicio, ancho, filaLocalFin);

        clock_t finCPU = clock();
        auto finReal = chrono::high_resolution_clock::now();
        double tiempoReal = chrono::duration<double>(finReal - inicioReal).count();
        double tiempoCPU = double(finCPU - inicioCPU) / CLOCKS_PER_SEC;

        // 3. Devolver las filas filtradas al nodo 0
        auto inicioComunicacion = chrono::high_resolution_clock::now();
        int* filasFiltradas = bloqueFiltrado.pixeles + filaLocalInicio * valoresPorFila;
        if (id == 0) {
            memcpy(salidas[f].pixeles + propiaInicio * valoresPorFila, filasFiltradas,
                   (propiaFin - propiaInicio) * valoresPorFila * sizeof(int));
            for (int n = 1; n < nodos; n++) {
                int inicio, fin;
                filasDelNodo(n, nodos, alto, inicio, fin);
                MPI_Recv(salidas[f].pixeles + inicio * valoresPorFila, (fin - inicio) * valoresPorFila,
                         MPI_INT, n, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        } else {
            MPI_Send(filasFiltradas, (propiaFin - propiaInicio) * valoresPorFila, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
        double tiempoComunicacion = chrono::duration<double>(chrono::high_resolution_clock::now() - inicioComunicacion).count();

        cout << "Nodo " << id << " (" << maquina << ") filas " << propiaInicio << "-" << propiaFin
             << " | " << setw(8) << left << filtros[f]->nombre << right
             << " tiempo real = " << tiempoReal << " s | tiempo CPU = " << tiempoCPU
             << " s | comunicacion = " << tiempoComunicacion << " s" << endl;
    }

    // El nodo 0 guarda los resultados al final, así los demás nodos no lo esperan entre filtros
    if (id == 0) {
        for (int f = 0; f < cantidad; f++) {
            char ruta[MAX_RUTA];
            nombreSalida(argv[2], filtros[f]->nombre, cantidad, ruta);
            salidas[f].guardar(ruta);
        }
        double tiempoTotal = chrono::duration<double>(chrono::high_resolution_clock::now() - inicioPrograma).count();
        cout << "Nodo 0: leer y enviar la imagen = " << tiempoLectura
             << " s | programa completo = " << tiempoTotal << " s" << endl;
    }

    MPI_Finalize();
    return 0;
}
