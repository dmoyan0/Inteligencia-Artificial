//Bibliotecas
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <set>
#include <string>
#include <map>
#include <algorithm>

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
//Funcion para leer los examenes
map<int, int> cantExamenes(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    map<int, int> examenes;

    if (archivo.is_open()) {
        int idExamen, cant;
        while (archivo >> idExamen >> cant) {
            examenes[idExamen] = cant;
        }
        archivo.close();
    } else {
        cerr << "Error al abrir el archivo " << nombreArchivo << endl;
    }

    return examenes;
}
//GREEDY
map<int, int> greedyScheduler(const vector<vector<int>>& matrizConflictos, const map<int, int>& examenes) {
    vector<int> exámenesOrdenados;
    for (const auto& [idExamen, _] : examenes) {
        exámenesOrdenados.push_back(idExamen);
    }

    // Ordenar los exámenes por el número de conflictos (grado)
    sort(exámenesOrdenados.begin(), exámenesOrdenados.end(), [&matrizConflictos](int a, int b) {
        int gradoA = count(matrizConflictos[a].begin(), matrizConflictos[a].end(), 1);
        int gradoB = count(matrizConflictos[b].begin(), matrizConflictos[b].end(), 1);
        return gradoA > gradoB;
    });

    map<int, int> asignacion; // examen -> time-slot
    int timeSlot = 0;

    for (int examen : exámenesOrdenados) {
        bool asignado = false;
        while (!asignado) {
            bool conflicto = false;
            for (const auto& [examenAsignado, ts] : asignacion) {
                if (ts == timeSlot && matrizConflictos[examen][examenAsignado] == 1) {
                    conflicto = true;
                    break;
                }
            }
            if (!conflicto) {
                asignacion[examen] = timeSlot;
                asignado = true;
            } else {
                timeSlot++;
            }
        }
    }

    return asignacion;
}
//Main
int main() {
    string archivoEstudiantes = "./Carleton91.stu";
    string archivoExamenes = "./Carleton91.exm";

    //Constantes
    //Matriz de conflictos
    vector<vector<int>> matrizConflictos = crearMatrizConflictos(archivoEstudiantes);//C 
    set<string> A = leerEstudiantes(archivoEstudiantes);//Conjunto de nombres de los alumnos
    set<string> E = leerExamenes(archivoEstudiantes);//Conjunto de nombres de los Examenes
    map<int, int> examenes = cantExamenes(archivoExamenes);//Leer los examenes y sus cantidades
    string T;//Cantidad de bloques en los que se deben realizar los exámenes
    string D;//Bloques por dia
    vector<int> W = {16, 8, 4, 2, 1, 0};
    int S = 30;//Capacidad de las salas

    //Variables
    map<string, vector<Sala>> examSala = countExams(archivoEstudiantes);//Salas y capacidad que utilizan los examen

    map<int, int> asignacion = greedyScheduler(matrizConflictos, examenes);
    // for (const auto& pair : examSala) {
    //     string examen = pair.first;
    //     vector<Sala> salas = pair.second;
    //     cout << "Examen " << examen << ": " << endl;
    //     for (const auto& sala : salas) {
    //         cout << "Sala " << sala.idSala << " con capacidad " << sala.capacidad  << " y ocupacion total de: " << sala.ocupacion << endl;
    //     }
    // }

    //for (const auto& estudiante : E) {
    //    cout << estudiante << endl;
    //}
    // // Imprimir la matriz de conflictos
    // cout << "Matriz de Conflictos:" << endl;
    // for (size_t i = 0; i < matrizConflictos.size(); ++i) {
    //     for (size_t j = 0; j < matrizConflictos[i].size(); ++j) {
    //         cout << matrizConflictos[i][j] << " ";
    //     }
    //     cout << endl;
    // }

    ofstream archivo("./Carleton91.sol");
    if (archivo.is_open()){
        for (const auto& [examen, timeSlot] : asignacion){
            archivo << examen << " " << timeSlot << endl;
        }
        archivo.close();
    } else {
        cerr << "Error al abrir el archivo " << "Carleton91.sol" << endl;
    }

    return 0;

}