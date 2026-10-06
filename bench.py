#!/usr/bin/env python3
# Corre todos los diseños sobre las imágenes de prueba y escribe resultados.md.
# Cada medición es la mediana de 5 ejecuciones (el filtrado dura milisegundos y varía).
# Antes: make  y  cd docker && docker compose up -d
import os, re, statistics, subprocess

EJECUCIONES = 5
IMAGENES = ["lena.pgm", "lena.ppm", "fruit.pgm", "fruit.ppm", "puj.pgm", "puj.ppm",
            "sulfur.pgm", "sulfur.ppm", "damma.pgm", "damma.ppm"]
NUM = r"([\d.]+)"


def correr(comando, hilos=None):
    entorno = dict(os.environ)
    if hilos:
        entorno["OMP_NUM_THREADS"] = str(hilos)
    return subprocess.run(comando, capture_output=True, text=True, check=True, env=entorno).stdout


def memoria_compartida(programa, imagen, hilos=None):
    """Mediana de (filtrado real, filtrado CPU, programa completo) en segundos."""
    medidas = []
    for _ in range(EJECUCIONES):
        salida = correr([f"./{programa}", f"images/{imagen}", f"images/out/{programa}_{imagen}"], hilos)
        real, cpu = re.search(rf"filtrado\s+tiempo real = {NUM} s \| tiempo CPU = {NUM}", salida).groups()
        total = re.search(rf"programa completo .* = {NUM}", salida).group(1)
        medidas.append((float(real), float(cpu), float(total)))
    return [statistics.median(m) for m in zip(*medidas)]


def mpi(imagen, nodos):
    """Mediana de (filtrado real, CPU, comunicación) del nodo más lento, leer+enviar y programa completo."""
    maquinas = ",".join(f"node{i + 1}" for i in range(nodos))
    medidas = []
    for _ in range(EJECUCIONES):
        salida = correr(["docker", "compose", "-f", "docker/docker-compose.yml", "exec", "-u", "mpi", "node1",
                         "mpirun", "-np", str(nodos), "--host", maquinas, "--bind-to", "none",
                         "./mpi_filterer", f"images/{imagen}", f"images/out/mpi_{imagen}"])
        # Sumar los tres filtros de cada nodo y quedarse con el nodo más lento
        por_nodo = {}
        for nodo, real, cpu, comm in re.findall(
                rf"Nodo (\d+) .*tiempo real = {NUM} s \| tiempo CPU = {NUM} s \| comunicacion = {NUM}", salida):
            suma = por_nodo.get(nodo, [0, 0, 0])
            por_nodo[nodo] = [suma[0] + float(real), suma[1] + float(cpu), suma[2] + float(comm)]
        lento = max(por_nodo.values(), key=lambda t: t[0])
        lectura, total = re.search(rf"leer y enviar la imagen = {NUM} s \| programa completo = {NUM}", salida).groups()
        medidas.append((*lento, float(lectura), float(total)))
    return [statistics.median(m) for m in zip(*medidas)]


def ms(segundos):
    return f"{segundos * 1000:.2f}"


os.makedirs("images/out", exist_ok=True)
lineas = [f"# Resultados (mediana de {EJECUCIONES} ejecuciones, filtros blur + laplace + sharpen, tiempos en ms)\n"]

secuencial = {}
lineas += ["## Diseño 2: secuencial\n",
           "| Imagen | Valores (píxeles × canales) | Filtrado real | Filtrado CPU | Programa completo | % filtrado |",
           "|---|---|---|---|---|---|"]
for imagen in IMAGENES:
    ancho, alto = map(int, open(f"images/{imagen}").read(64).split()[1:3])
    valores = ancho * alto * (3 if imagen.endswith("ppm") else 1)
    secuencial[imagen] = memoria_compartida("filterer", imagen)
    real, cpu, total = secuencial[imagen]
    lineas.append(f"| {imagen} | {valores:,} | {ms(real)} | {ms(cpu)} | {ms(total)} | {100 * real / total:.1f} % |")
    print("secuencial", imagen, flush=True)

lineas += ["\n## Diseño 3: hilos (4 regiones) y OpenMP (1, 2, 4 y 8 hilos)\n",
           "Speedup S = T_secuencial / T_paralelo y eficiencia E = S / p, con el tiempo real de filtrado.\n",
           "| Imagen | Diseño | p | Filtrado real | Filtrado CPU | Speedup | Eficiencia | Programa completo |",
           "|---|---|---|---|---|---|---|---|"]
for imagen in IMAGENES:
    pruebas = [("Hilos", "th_filterer", 4, None)] + [("OpenMP", "omp_filterer", p, p) for p in (1, 2, 4, 8)]
    for diseno, programa, p, hilos in pruebas:
        real, cpu, total = memoria_compartida(programa, imagen, hilos)
        s = secuencial[imagen][0] / real
        lineas.append(f"| {imagen} | {diseno} | {p} | {ms(real)} | {ms(cpu)} | {s:.2f} | {s / p:.2f} | {ms(total)} |")
    print("hilos/openmp", imagen, flush=True)

lineas += ["\n## Diseño 4: MPI entre contenedores Docker (un proceso por nodo)\n",
           "Tiempos del nodo más lento. Speedup contra 1 nodo (misma máquina virtual de Docker).\n",
           "| Imagen | Nodos | Filtrado real | Filtrado CPU | Comunicación | Leer y enviar | Programa completo | Speedup filtrado |",
           "|---|---|---|---|---|---|---|---|"]
for imagen in ["sulfur.pgm", "sulfur.ppm", "damma.pgm", "damma.ppm"]:
    for nodos in (1, 2, 3):
        real, cpu, comm, lectura, total = mpi(imagen, nodos)
        if nodos == 1:
            base = real
        lineas.append(f"| {imagen} | {nodos} | {ms(real)} | {ms(cpu)} | {ms(comm)} | {ms(lectura)} | "
                      f"{ms(total)} | {base / real:.2f} |")
    print("mpi", imagen, flush=True)

open("resultados.md", "w").write("\n".join(lineas) + "\n")
print("escrito resultados.md")
