// Diseño 1: aplicación base. Lee una imagen PGM/PPM y la vuelve a escribir.
// Uso: ./processor lena.ppm lena2.ppm
//      ./processor - lena2.ppm < lena.ppm      (lectura desde la entrada estándar)
#include <iostream>
#include "imagen.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "uso: " << argv[0] << " entrada.ppm salida.ppm   (entrada \"-\" = stdin)" << endl;
        return 1;
    }

    Imagen imagen;
    if (!imagen.leer(argv[1])) {
        cout << "Error leyendo " << argv[1] << endl;
        return 1;
    }
    if (!imagen.guardar(argv[2])) {
        cout << "Error escribiendo " << argv[2] << endl;
        return 1;
    }

    cout << imagen.tipo << " " << imagen.ancho << "x" << imagen.alto
         << ", " << imagen.canales << " canal(es) -> " << argv[2] << endl;
    return 0;
}
