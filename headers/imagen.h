#ifndef IMAGEN_H
#define IMAGEN_H

#include <cctype>
#include <cstdio>
#include <cstring>

// Imagen PGM (P2, escala de grises) o PPM (P3, color) en texto plano.
// Los píxeles se guardan en un arreglo, fila por fila. En color cada píxel ocupa 3 posiciones (R, G, B).
class Imagen {
public:
    char tipo[3];   // "P2" o "P3"
    int ancho;
    int alto;
    int maximo;     // valor máximo de color (normalmente 255)
    int canales;    // 1 en P2, 3 en P3
    int* pixeles;

    Imagen() {
        tipo[0] = '\0';
        ancho = alto = maximo = canales = 0;
        pixeles = nullptr;
    }

    ~Imagen() {
        delete[] pixeles;
    }

    int totalValores() const {
        return ancho * alto * canales;
    }

    // Posición en el arreglo del canal c del píxel (x, y)
    int indice(int x, int y, int c) const {
        return (y * ancho + x) * canales + c;
    }

    // Reserva memoria para una imagen del tamaño indicado
    void crear(int nuevoAncho, int nuevoAlto, int nuevoMaximo, int nuevosCanales) {
        ancho = nuevoAncho;
        alto = nuevoAlto;
        maximo = nuevoMaximo;
        canales = nuevosCanales;
        strcpy(tipo, canales == 3 ? "P3" : "P2");
        delete[] pixeles;
        pixeles = new int[totalValores()];
    }

    void crearComo(const Imagen& otra) {
        crear(otra.ancho, otra.alto, otra.maximo, otra.canales);
    }

    // Lee el archivo; la ruta "-" lee desde la entrada estándar
    bool leer(const char* ruta) {
        FILE* archivo = (strcmp(ruta, "-") == 0) ? stdin : fopen(ruta, "r");
        if (archivo == NULL) return false;

        char leido[3];
        int nuevoAncho, nuevoAlto, nuevoMaximo;
        bool ok = fscanf(archivo, "%2s", leido) == 1 &&
                  (strcmp(leido, "P2") == 0 || strcmp(leido, "P3") == 0) &&
                  leerNumero(archivo, &nuevoAncho) &&
                  leerNumero(archivo, &nuevoAlto) &&
                  leerNumero(archivo, &nuevoMaximo);

        if (ok) {
            crear(nuevoAncho, nuevoAlto, nuevoMaximo, strcmp(leido, "P3") == 0 ? 3 : 1);
            for (int i = 0; ok && i < totalValores(); i++)
                ok = fscanf(archivo, "%d", &pixeles[i]) == 1;
        }

        if (archivo != stdin) fclose(archivo);
        return ok;
    }

    bool guardar(const char* ruta) const {
        FILE* archivo = fopen(ruta, "w");
        if (archivo == NULL) return false;

        fprintf(archivo, "%s\n%d %d\n%d\n", tipo, ancho, alto, maximo);
        // Una fila de la imagen por línea
        for (int i = 0; i < totalValores(); i++) {
            bool finDeFila = (i + 1) % (ancho * canales) == 0;
            fprintf(archivo, finDeFila ? "%d\n" : "%d ", pixeles[i]);
        }

        fclose(archivo);
        return true;
    }

private:
    // Lee un número del encabezado, saltando espacios y comentarios (líneas que empiezan con #)
    static bool leerNumero(FILE* archivo, int* numero) {
        int c = fgetc(archivo);
        while (c == '#' || isspace(c)) {
            if (c == '#')
                while (c != '\n' && c != EOF) c = fgetc(archivo);
            c = fgetc(archivo);
        }
        ungetc(c, archivo);
        return fscanf(archivo, "%d", numero) == 1;
    }
};

#endif
