//Bibliotecas
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <set>
#include <string>
#include <map>

using namespace std;

struct Sala {
    int idSala;

    int capacidad;

    int ocupacion;

};
//Funciones
// Función para crear la matriz de conflictos
vector<vector<int>> crearMatrizConflictos(const string& nombreArchivo) {
    ifstream archivo;
    archivo.open(nombreArchivo,ios::in);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo." << endl;
        exit(1);
    }

    unordered_map<string, set<int>> examenesPorEstudiante;
    string idEstudiante; 
    int idExamen;
    
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
//Funcion para extraer todos los estudiantes
set<string> leerEstudiantes(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);

    set<string> estudiantes;

    if (archivo.is_open()) {
        string idEstudiante, idExamen;
        while (archivo >> idEstudiante >> idExamen) {
            estudiantes.insert(idEstudiante);
        }
        archivo.close();
    } else {
        cerr << "Error al abrir el archivo " << nombreArchivo << endl;
    }

    return estudiantes;
}
//Funcion para extraer todos los examenes
set<string> leerExamenes(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);

    set<string> examenes;

    if (archivo.is_open()) {
        string idEstudiante, idExamen;
        while (archivo >> idEstudiante >> idExamen) {
            examenes.insert(idExamen);
        }
        archivo.close();
    } else {
        cerr << "Error al abrir el archivo " << nombreArchivo << endl;
    }

    return examenes;
}
//Funcion que reparte los estudiantes entre las salas necesarias
map<string, vector<Sala>> countExams(const string& nombreArchivo) {
    map<string, int> examCounts;
    ifstream file(nombreArchivo);
    string estudiante, examen;

    while (file >> estudiante >> examen) {
        if (examCounts.find(examen)!= examCounts.end()) {
            examCounts[examen]++;
        } else {
            examCounts[examen] = 1;
        }
    }
    file.close();
    
    map<string, vector<Sala>> examSala;

    int salaid = 0;
    for (const auto& pair : examCounts) {
        string examen = pair.first;
        int cantidadEstudiantes = pair.second;
        int salasNecesarias = (cantidadEstudiantes + 29) / 30; // Redondeo hacia arriba
        vector<Sala> salas;

        for (int i = 0; i < salasNecesarias; i++) {
            Sala sala;
            sala.idSala = salaid;
            sala.capacidad = 30;
            int ocupacion;
            if (cantidadEstudiantes >= 30) {
                ocupacion = 30;
                cantidadEstudiantes = cantidadEstudiantes - 30;
            } else {
                ocupacion = cantidadEstudiantes;
            }
            sala.ocupacion = ocupacion;
            salas.push_back(sala);
            salaid++;
        }
        examSala[examen] = salas;
    }

    return examSala;
}

//Main
int main() {
    string nombreArchivo = "./Carleton91.stu";
    
    //Constantes
    //Matriz de conflictos
    vector<vector<int>> matrizConflictos = crearMatrizConflictos(nombreArchivo);//C 
    //int N = UINTMAX_MAX;//Numero muy grande
    set<string> A = leerEstudiantes(nombreArchivo);//Conjunto de nombres de los alumnos
    set<string> E = leerExamenes(nombreArchivo);//Conjunto de nombres de los alumnos, cada indice corresponde a la sala del examen
    string T;//Cantidad de bloques en los que se deben realizar los exámenes
    string D;//Bloques por dia
    vector<int> W = {16, 8, 4, 2, 1, 0};
    int S = 30;//Capacidad de las salas

    //Variables
    map<string, vector<Sala>> examSala = countExams(nombreArchivo);//Salas y capacidad que utilizan los examen

    // for (const auto& pair : examSala) {
    //     string examen = pair.first;
    //     vector<Sala> salas = pair.second;
    //     cout << "Examen " << examen << ": " << endl;
    //     for (const auto& sala : salas) {
    //         cout << "Sala " << sala.idSala << " con capacidad " << sala.capacidad  << " y ocupacion total de: " << sala.ocupacion << endl;
    //     }
    // }

    // for (const auto& estudiante : E) {
    //     cout << estudiante << endl;
    // }
    // // Imprimir la matriz de conflictos
    // cout << "Matriz de Conflictos:" << endl;
    // for (size_t i = 0; i < matrizConflictos.size(); ++i) {
    //     for (size_t j = 0; j < matrizConflictos[i].size(); ++j) {
    //         cout << matrizConflictos[i][j] << " ";
    //     }
    //     cout << endl;
    // }

    return 0;

}