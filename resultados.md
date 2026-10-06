# Resultados (mediana de 5 ejecuciones, filtros blur + laplace + sharpen, tiempos en ms)

## Diseño 2: secuencial

| Imagen | Valores (píxeles × canales) | Filtrado real | Filtrado CPU | Programa completo | % filtrado |
|---|---|---|---|---|---|
| lena.pgm | 262,144 | 6.68 | 6.67 | 91.73 | 7.3 % |
| lena.ppm | 49,152 | 1.26 | 1.26 | 18.44 | 6.8 % |
| fruit.pgm | 405,000 | 10.61 | 10.61 | 140.18 | 7.6 % |
| fruit.ppm | 1,215,000 | 31.88 | 31.87 | 416.80 | 7.6 % |
| puj.pgm | 1,152,000 | 29.68 | 29.67 | 405.68 | 7.3 % |
| puj.ppm | 3,456,000 | 90.55 | 90.53 | 1190.64 | 7.6 % |
| sulfur.pgm | 823,000 | 21.34 | 21.34 | 269.73 | 7.9 % |
| sulfur.ppm | 2,469,000 | 64.47 | 64.45 | 806.94 | 8.0 % |
| damma.pgm | 1,278,000 | 33.35 | 33.35 | 422.03 | 7.9 % |
| damma.ppm | 3,834,000 | 100.52 | 100.51 | 1272.29 | 7.9 % |

## Diseño 3: hilos (4 regiones) y OpenMP (1, 2, 4 y 8 hilos)

Speedup S = T_secuencial / T_paralelo y eficiencia E = S / p, con el tiempo real de filtrado.

| Imagen | Diseño | p | Filtrado real | Filtrado CPU | Speedup | Eficiencia | Programa completo |
|---|---|---|---|---|---|---|---|
| lena.pgm | Hilos | 4 | 1.98 | 7.28 | 3.37 | 0.84 | 86.73 |
| lena.pgm | OpenMP | 1 | 6.72 | 6.72 | 0.99 | 0.99 | 92.05 |
| lena.pgm | OpenMP | 2 | 3.47 | 6.82 | 1.93 | 0.96 | 88.16 |
| lena.pgm | OpenMP | 4 | 2.01 | 7.10 | 3.32 | 0.83 | 86.42 |
| lena.pgm | OpenMP | 8 | 1.61 | 8.44 | 4.15 | 0.52 | 85.46 |
| lena.ppm | Hilos | 4 | 0.62 | 1.67 | 2.02 | 0.51 | 17.38 |
| lena.ppm | OpenMP | 1 | 1.26 | 1.25 | 1.00 | 1.00 | 18.48 |
| lena.ppm | OpenMP | 2 | 0.68 | 1.31 | 1.84 | 0.92 | 18.26 |
| lena.ppm | OpenMP | 4 | 0.50 | 1.47 | 2.54 | 0.63 | 17.72 |
| lena.ppm | OpenMP | 8 | 0.52 | 2.02 | 2.42 | 0.30 | 18.12 |
| fruit.pgm | Hilos | 4 | 2.91 | 10.94 | 3.64 | 0.91 | 132.03 |
| fruit.pgm | OpenMP | 1 | 10.38 | 10.38 | 1.02 | 1.02 | 138.55 |
| fruit.pgm | OpenMP | 2 | 5.34 | 10.54 | 1.99 | 0.99 | 130.71 |
| fruit.pgm | OpenMP | 4 | 3.07 | 10.92 | 3.46 | 0.87 | 129.23 |
| fruit.pgm | OpenMP | 8 | 2.41 | 12.34 | 4.41 | 0.55 | 128.28 |
| fruit.ppm | Hilos | 4 | 8.65 | 33.23 | 3.68 | 0.92 | 397.08 |
| fruit.ppm | OpenMP | 1 | 31.75 | 31.75 | 1.00 | 1.00 | 417.00 |
| fruit.ppm | OpenMP | 2 | 16.24 | 32.22 | 1.96 | 0.98 | 395.03 |
| fruit.ppm | OpenMP | 4 | 8.46 | 32.24 | 3.77 | 0.94 | 386.71 |
| fruit.ppm | OpenMP | 8 | 7.27 | 36.20 | 4.38 | 0.55 | 386.33 |
| puj.pgm | Hilos | 4 | 8.04 | 31.18 | 3.69 | 0.92 | 378.12 |
| puj.pgm | OpenMP | 1 | 29.72 | 29.72 | 1.00 | 1.00 | 400.97 |
| puj.pgm | OpenMP | 2 | 15.09 | 29.99 | 1.97 | 0.98 | 381.69 |
| puj.pgm | OpenMP | 4 | 7.83 | 30.13 | 3.79 | 0.95 | 372.91 |
| puj.pgm | OpenMP | 8 | 6.50 | 34.19 | 4.57 | 0.57 | 369.53 |
| puj.ppm | Hilos | 4 | 23.68 | 93.12 | 3.82 | 0.96 | 1105.89 |
| puj.ppm | OpenMP | 1 | 90.61 | 90.60 | 1.00 | 1.00 | 1181.36 |
| puj.ppm | OpenMP | 2 | 45.74 | 90.97 | 1.98 | 0.99 | 1147.52 |
| puj.ppm | OpenMP | 4 | 23.61 | 92.06 | 3.83 | 0.96 | 1112.15 |
| puj.ppm | OpenMP | 8 | 17.86 | 103.42 | 5.07 | 0.63 | 1116.34 |
| sulfur.pgm | Hilos | 4 | 5.92 | 22.69 | 3.61 | 0.90 | 253.54 |
| sulfur.pgm | OpenMP | 1 | 21.31 | 21.31 | 1.00 | 1.00 | 271.15 |
| sulfur.pgm | OpenMP | 2 | 10.80 | 21.41 | 1.98 | 0.99 | 253.92 |
| sulfur.pgm | OpenMP | 4 | 5.67 | 21.73 | 3.76 | 0.94 | 249.18 |
| sulfur.pgm | OpenMP | 8 | 4.86 | 24.82 | 4.39 | 0.55 | 248.50 |
| sulfur.ppm | Hilos | 4 | 17.28 | 67.76 | 3.73 | 0.93 | 768.29 |
| sulfur.ppm | OpenMP | 1 | 63.79 | 63.79 | 1.01 | 1.01 | 789.76 |
| sulfur.ppm | OpenMP | 2 | 32.72 | 65.00 | 1.97 | 0.99 | 764.56 |
| sulfur.ppm | OpenMP | 4 | 18.81 | 67.24 | 3.43 | 0.86 | 897.12 |
| sulfur.ppm | OpenMP | 8 | 15.79 | 72.43 | 4.08 | 0.51 | 800.52 |
| damma.pgm | Hilos | 4 | 9.79 | 36.01 | 3.41 | 0.85 | 402.32 |
| damma.pgm | OpenMP | 1 | 33.60 | 33.51 | 0.99 | 0.99 | 437.04 |
| damma.pgm | OpenMP | 2 | 17.10 | 33.48 | 1.95 | 0.98 | 403.75 |
| damma.pgm | OpenMP | 4 | 9.56 | 35.06 | 3.49 | 0.87 | 402.01 |
| damma.pgm | OpenMP | 8 | 7.65 | 38.07 | 4.36 | 0.54 | 401.80 |
| damma.ppm | Hilos | 4 | 31.35 | 107.28 | 3.21 | 0.80 | 1224.11 |
| damma.ppm | OpenMP | 1 | 114.97 | 100.82 | 0.87 | 0.87 | 1335.10 |
| damma.ppm | OpenMP | 2 | 50.55 | 100.61 | 1.99 | 0.99 | 1218.37 |
| damma.ppm | OpenMP | 4 | 26.26 | 101.96 | 3.83 | 0.96 | 1188.85 |
| damma.ppm | OpenMP | 8 | 19.02 | 112.73 | 5.29 | 0.66 | 1182.58 |

## Diseño 4: MPI entre contenedores Docker (un proceso por nodo)

Tiempos del nodo más lento. Speedup contra 1 nodo (misma máquina virtual de Docker).

| Imagen | Nodos | Filtrado real | Filtrado CPU | Comunicación | Leer y enviar | Programa completo | Speedup filtrado |
|---|---|---|---|---|---|---|---|
| sulfur.pgm | 1 | 21.76 | 21.75 | 2.01 | 45.22 | 270.14 | 1.00 |
| sulfur.pgm | 2 | 11.12 | 11.12 | 2.49 | 51.01 | 264.79 | 1.96 |
| sulfur.pgm | 3 | 7.48 | 7.47 | 2.91 | 61.23 | 269.69 | 2.91 |
| sulfur.ppm | 1 | 63.87 | 63.86 | 3.48 | 134.77 | 801.93 | 1.00 |
| sulfur.ppm | 2 | 32.69 | 32.68 | 5.88 | 142.48 | 772.30 | 1.95 |
| sulfur.ppm | 3 | 22.33 | 22.32 | 7.49 | 152.17 | 767.57 | 2.86 |
| damma.pgm | 1 | 33.69 | 33.69 | 2.04 | 70.71 | 420.30 | 1.00 |
| damma.pgm | 2 | 17.22 | 17.21 | 3.70 | 71.37 | 402.64 | 1.96 |
| damma.pgm | 3 | 11.51 | 11.51 | 4.45 | 81.56 | 414.75 | 2.93 |
| damma.ppm | 1 | 98.82 | 98.81 | 5.60 | 218.29 | 1250.77 | 1.00 |
| damma.ppm | 2 | 49.42 | 49.41 | 9.45 | 223.82 | 1208.77 | 2.00 |
| damma.ppm | 3 | 34.55 | 34.55 | 11.31 | 234.24 | 1211.98 | 2.86 |
