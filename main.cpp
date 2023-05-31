#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
#include <map>
#include <cmath>
#include <ctime>


using namespace std;

// Definimos las constantes que usaremos a lo largo del programa.
int num_nodos_recorridos = 0;
const int TAM_TABLERO = 3;
const int PROF_MAX = 120;
const int INFINITO = INT_MAX;

// Estructura para representar el estado del rompecabezas.
struct Estado {
    std::vector<std::vector<int>> tablero;
    int posXBlanco;
    int posYBlanco;

    Estado();

    bool operator==(const Estado& otro) const;

    void imprimir() const;
};

int heuristicaManhattan(const Estado& estado);

std::vector<Estado> obtenerSucesores(const Estado& estado);

void busquedaProfundidadLimitada(Estado estado, int g, int limite,
                                 int (*Manhattan)(const Estado&),
                                 std::vector<Estado>& solucion, bool& solucionEncontrada);

std::vector<Estado> IDAStar(const Estado& inicio);

void resolverRompecabezas(Estado estado);

int main();

// Definición de las funciones
Estado::Estado() {
    // Constructor: inicializamos el tablero en orden y colocamos el blanco en la esquina inferior derecha.
    tablero.resize(TAM_TABLERO, std::vector<int>(TAM_TABLERO));
    int valor = 1;
    for (int i = 0; i < TAM_TABLERO; i++) {
        for (int j = 0; j < TAM_TABLERO; j++) {
            tablero[i][j] = valor;
            valor += 1;
        }
    }
    tablero[TAM_TABLERO - 1][TAM_TABLERO - 1] = 0;  // La posición del blanco se representa con 0.
    posXBlanco = TAM_TABLERO - 1;
    posYBlanco = TAM_TABLERO - 1;
}

bool Estado::operator==(const Estado& otro) const {
    // Comparamos si los tableros y la posición del blanco son iguales.
    return tablero == otro.tablero && posXBlanco == otro.posXBlanco && posYBlanco == otro.posYBlanco;
}

void Estado::imprimir() const {
    // Función para imprimir el tablero.
    for (int i = 0; i < TAM_TABLERO; i++) {
        for (int j = 0; j < TAM_TABLERO; j++) {
            std::cout << tablero[i][j] << "   ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int heuristicaManhattan(const Estado& estado) {
    // Función heurística que calcula la suma de las distancias de Manhattan.
    int distancia = 0;
    for (int i = 0; i < TAM_TABLERO; i++) {
        for (int j = 0; j < TAM_TABLERO; j++) {
            int valor = estado.tablero[i][j];
            if (valor != 0) {
                int objetivoX = (valor - 1) / TAM_TABLERO;
                int objetivoY = (valor - 1) % TAM_TABLERO;
                distancia += std::abs(i - objetivoX) + std::abs(j - objetivoY);
            }
        }
    }
        return distancia;
}

std::vector<Estado> obtenerSucesores(const Estado& estado) {
    // Genera los sucesores del estado actual moviendo el blanco en cada una de las 4 direcciones posibles.
    std::vector<Estado> sucesores;
    int x = estado.posXBlanco;
    int y = estado.posYBlanco;
    if (x > 0) {
        Estado s = estado;
        std::swap(s.tablero[x][y], s.tablero[x-1][y]);
        s.posXBlanco = x-1;
        sucesores.push_back(s);
    }
    if (x < TAM_TABLERO-1) {
        Estado s = estado;
        std::swap(s.tablero[x][y], s.tablero[x+1][y]);
        s.posXBlanco = x+1;
        sucesores.push_back(s);
    }
    if (y > 0) {
        Estado s = estado;
        std::swap(s.tablero[x][y], s.tablero[x][y-1]);
        s.posYBlanco = y-1;
        sucesores.push_back(s);
    }
    if (y < TAM_TABLERO-1) {
        Estado s = estado;
        std::swap(s.tablero[x][y], s.tablero[x][y+1]);
        s.posYBlanco = y+1;
        sucesores.push_back(s);
    }
    return sucesores;
}

void busquedaProfundidadLimitada(Estado estado, int g, int limite, int (*Manhattan)(const Estado&),
                                 std::vector<Estado>& solucion, bool& solucionEncontrada) {
                                     num_nodos_recorridos++;
    // Realiza la búsqueda con una profundidad limitada.
    int f = g + Manhattan(estado);
    if (f > limite || g >= PROF_MAX) {
        return;
    }
    if (Manhattan(estado) == 0) {
        solucionEncontrada = true;
        solucion.push_back(estado);
        return;
    }
    int min_t = INFINITO;
    std::vector<Estado> sucesores = obtenerSucesores(estado);
    for (Estado& s : sucesores) {
        if (s == estado) {
            continue;
        }
        int t = g + Manhattan(s);
        if (t <= limite) {
            busquedaProfundidadLimitada(s, g + 1, limite, Manhattan, solucion, solucionEncontrada);
            if (solucionEncontrada) {
                solucion.push_back(estado);
                return;
            }
        }
        min_t = std::min(min_t, t);
    }
    limite = min_t;
}

std::vector<Estado> IDAStar(const Estado& inicio) {
    int limite = heuristicaManhattan(inicio);
    while (true) {
        std::vector<Estado> solucion;
        bool solucionEncontrada = false;
        busquedaProfundidadLimitada(inicio, 0, limite, heuristicaManhattan, solucion, solucionEncontrada);
        if (solucionEncontrada) {
            std::reverse(solucion.begin(), solucion.end());
            return solucion;
        } else if (limite == INFINITO) {
            return solucion;
        }
        limite++;
    }
}

void resolverRompecabezas(Estado estado) {
    // Resuelve el rompecabezas del 15 utilizando el algoritmo IDA*.
    std::cout << "Tablero inicial:" << std::endl;
    estado.imprimir();
    std::vector<Estado> solucion = IDAStar(estado);
    if (solucion.empty()) {
        std::cout << "No se encontró solución" << std::endl;
    } else {
        std::cout << "Solución encontrada con costo " << solucion.size()-1 << std::endl;
        std::cout << " " << std::endl;
        Estado estadoFinal = solucion.back();
        std::cout << "Tablero final:" << std::endl;
        estadoFinal.imprimir();
    }
}

int main() {
    // Crea un estado inicial y mezcla el tablero antes de intentar resolverlo.
    Estado estado;
    srand(time(nullptr));

    for (int i = 0; i < 100; i++) {
        std::vector<Estado> sucesores = obtenerSucesores(estado);
        int indiceAleatorio = rand() % sucesores.size();
        estado = sucesores[indiceAleatorio];
    }

    resolverRompecabezas(estado);

    std::cout << "Número de nodos recorridos: " << num_nodos_recorridos << "\n";

    return 0;
}
