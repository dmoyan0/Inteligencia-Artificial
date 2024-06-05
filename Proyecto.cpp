//Bibliotecas
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <set>
#include <string>

using namespace std;

//Funciones
// Función para crear la matriz de conflictos
vector<vector<int>> crearMatrizConflictos(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo." << endl;
        exit(1);
    }

    unordered_map<int, set<int>> examenesPorEstudiante;
    int idEstudiante, idExamen;
    
    // Leer el archivo y almacenar en el map
    while (archivo >> idEstudiante >> idExamen) {
        examenesPorEstudiante[idEstudiante].insert(idExamen);
    }
    archivo.close();

    // Encontrar el número máximo de examen para definir el tamaño de la matriz
    int maxExamen = 0;
    for (const auto& entry : examenesPorEstudiante) {
        for (int examen : entry.second) {
            if (examen > maxExamen) {
                maxExamen = examen;
            }
        }
    }

    // Crear una matriz cuadrada de tamaño (maxExamen+1) x (maxExamen+1) inicializada en 0
    vector<vector<int>> matrizConflictos(maxExamen + 1, vector<int>(maxExamen + 1, 0));

    // Llenar la matriz de conflictos
    for (const auto& entry : examenesPorEstudiante) {
        const set<int>& examenes = entry.second;
        for (auto it1 = examenes.begin(); it1 != examenes.end(); ++it1) {
            for (auto it2 = next(it1); it2 != examenes.end(); ++it2) {
                matrizConflictos[*it1][*it2] = 1;
                matrizConflictos[*it2][*it1] = 1;
            }
        }
    }

    return matrizConflictos;
}



//Main
int main() {
    string nombreArchivo = "./Carleton91.stu";
    //Constantes
    //Matriz de conflictos
    vector<vector<int>> matrizConflictos = crearMatrizConflictos(nombreArchivo);

    // Imprimir la matriz de conflictos
    cout << "Matriz de Conflictos:" << endl;
    for (size_t i = 0; i < matrizConflictos.size(); ++i) {
        for (size_t j = 0; j < matrizConflictos[i].size(); ++j) {
            cout << matrizConflictos[i][j] << " ";
        }
        cout << endl;
    }

    return 0;

}