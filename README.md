# Filtrado paralelo de imágenes PPM/PGM

Micro-proyecto de Programación Paralela: aplicar filtros de convolución 3×3 a imágenes PGM (P2, escala de grises)
y PPM (P3, color) en cuatro versiones, y comparar sus tiempos.

| Diseño | Programa | Cómo reparte el trabajo |
|---|---|---|
| 1. Base | `processor` | Lee la imagen (archivo o entrada estándar) y la vuelve a escribir. |
| 2. Secuencial | `filterer` | Un solo hilo filtra toda la imagen. |
| 3. Hilos | `th_filterer` | 4 `std::thread`, uno por cuadrante de la imagen. |
| 3. OpenMP | `omp_filterer` | `#pragma omp parallel for` reparte las filas entre los hilos. |
| 4. MPI | `mpi_filterer` | Cada nodo (contenedor Docker) recibe su bloque de filas más una fila vecina arriba y abajo, lo filtra y lo devuelve al nodo 0. |

Video explicativo: https://drive.google.com/file/d/1BhZiywPJMaX7eIsfAV7kDg3VrmYhOmbQ/view?usp=sharing

## Filtros

Todos se aplican con la misma convolución: `I'(x, y) = factor · Σ K(i, j) · I(x + i, y + j)`, con `i, j = -1, 0, 1`.

| Filtro | Matriz 3×3 | Efecto |
|---|---|---|
| `blur` | 1/9 · [1 1 1 / 1 1 1 / 1 1 1] | suavizado |
| `gauss` | 1/16 · [1 2 1 / 2 4 2 / 1 2 1] | desenfoque ponderado |
| `sharpen` | [0 -1 0 / -1 5 -1 / 0 -1 0] | realce |
| `laplace` | [-1 -1 -1 / -1 8 -1 / -1 -1 -1] | detección de bordes |
| `sobel` | [-1 -2 -1 / 0 0 0 / 1 2 1] | bordes horizontales |

Los píxeles del borde se copian sin filtrar. `Filtro::aplicar` solo lee la imagen de entrada y solo escribe su
región de la salida, por eso las versiones paralelas no necesitan `mutex`. Agregar un filtro es agregar una línea
al arreglo `FILTROS` en `headers/filtros.h`.

## Estructura

```txt
headers/imagen.h      clase Imagen: lectura y escritura de P2/P3
headers/filtros.h     clase Filtro y los cinco filtros
headers/argumentos.h  lectura de --f y nombres de los archivos de salida
src/                  un programa por diseño
docker/               imagen y docker-compose con 3 nodos MPI
bench.py              corre todas las mediciones y genera resultados.md
images/               imágenes de prueba
```

## Compilar y ejecutar

```bash
make            # processor, filterer, th_filterer, omp_filterer
make check      # verifica que hilos y OpenMP dan la misma imagen que la versión secuencial
mkdir -p images/out
```

En macOS, OpenMP requiere `brew install libomp`.

```bash
./processor images/lena.ppm images/out/lena2.ppm
./processor - images/out/lena2.ppm < images/lena.ppm          # desde la entrada estándar
./filterer images/fruit.ppm images/out/fruit_blur.ppm --f blur
./th_filterer images/fruit.pgm images/out/fruit_blur2.pgm --f blur
OMP_NUM_THREADS=4 ./omp_filterer images/sulfur.pgm images/out/sulfur_N.pgm
```

Con un `--f` la salida se usa tal cual. Con varios `--f`, o sin `--f` (se aplican blur, laplace y sharpen), cada
filtro va a su propio archivo: `sulfur_N.pgm` → `sulfur_N_blur.pgm`, `sulfur_N_laplace.pgm`, …

Cada programa imprime el tiempo real (`std::chrono`) y el tiempo de CPU (`clock()`, suma de todos los hilos) de
cada filtro.

### MPI con Docker

```bash
cd docker
docker compose build node1 && docker compose up -d
docker compose exec -u mpi node1 mpirun -np 3 --host node1,node2,node3 --bind-to none \
    ./mpi_filterer images/sulfur.pgm images/out/sulfur_mpi.pgm
docker compose down
```

`--bind-to none` es necesario: los contenedores comparten la misma máquina virtual y, sin esa opción, OpenMPI
fija los procesos al mismo núcleo.

## Resultados

Speedup del filtrado frente a la versión secuencial (Apple M3, 4 núcleos de rendimiento + 4 de eficiencia;
mediana de 5 ejecuciones). Tablas completas en [resultados.md](resultados.md); se regeneran con
`python3 bench.py` (requiere `make` y los nodos MPI encendidos).

| Imagen | Secuencial | Hilos (4) | OpenMP (4) | OpenMP (8) | MPI (3 nodos) |
|---|---|---|---|---|---|
| sulfur.pgm | 21.34 ms | 3.61× | 3.76× | 4.39× | 2.91× |
| damma.pgm | 33.35 ms | 3.41× | 3.49× | 4.36× | 2.93× |
| damma.ppm | 100.52 ms | 3.21× | 3.83× | 5.29× | 2.86× |

Con 8 hilos la eficiencia baja a entre 0.5 y 0.7, y en imágenes pequeñas (lena.ppm, 128×128) la ganancia es
menor. En MPI, devolver los resultados al nodo 0 cuesta entre 3 y 11 ms con 3 nodos.

## Referencias

- [Especificación PPM](http://netpbm.sourceforge.net/doc/ppm.html) · [Especificación PGM](http://netpbm.sourceforge.net/doc/pgm.html)
- [Kernels de convolución](https://en.wikipedia.org/wiki/Kernel_(image_processing)) · [Operador Sobel](https://en.wikipedia.org/wiki/Sobel_operator)
- Repositorio base: https://github.com/japeto/netpbm_filters
