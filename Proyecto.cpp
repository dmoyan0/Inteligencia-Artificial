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
vector<vector<int>> crearMatrizConflictos(const string& nombreArchivo, vector<string>& examenesList) {
    ifstream archivo;
    archivo.open(nombreArchivo, ios::in);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo." << endl;
        exit(1);
    }

    unordered_map<string, set<string>> examenesPorEstudiante;
    string idEstudiante, idExamen;
    
    // Leer el archivo y almacenar en el map
    while (archivo >> idEstudiante >> idExamen) {
        examenesPorEstudiante[idEstudiante].insert(idExamen);
    }
    archivo.close();

    // Encontrar todos los exámenes únicos
    set<string> examenesUnicos;
    for (const auto& entry : examenesPorEstudiante) {
        for (const auto& examen : entry.second) {
            examenesUnicos.insert(examen);
        }
    }

    // Crear lista de exámenes
    examenesList.assign(examenesUnicos.begin(), examenesUnicos.end());

    // Mapear cada examen a un índice
    unordered_map<string, int> examenToIndex;
    for (size_t i = 0; i < examenesList.size(); ++i) {
        examenToIndex[examenesList[i]] = i;
    }

    // Crear una matriz cuadrada de tamaño examenesList.size() x examenesList.size() inicializada en 0
    vector<vector<int>> matrizConflictos(examenesList.size(), vector<int>(examenesList.size(), 0));
 
    // Llenar la matriz de conflictos
    for (const auto& entry : examenesPorEstudiante) {
        const set<string>& examenes = entry.second;
        for (auto it1 = examenes.begin(); it1 != examenes.end(); ++it1) {
            for (auto it2 = next(it1); it2 != examenes.end(); ++it2) {
                int idx1 = examenToIndex[*it1];
                int idx2 = examenToIndex[*it2];
                matrizConflictos[idx1][idx2] = 1;
                matrizConflictos[idx2][idx1] = 1;
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
map<string, int> cantExamenes(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    map<string, int> examenes;

    if (archivo.is_open()) {
        string idExamen;
        int cant;
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
map<string, int> greedyScheduler(const vector<vector<int>>& matrizConflictos, const map<string, int>& examenes, const vector<string>& examenesList) {
    vector<string> exámenesOrdenados = examenesList;

    // Ordenar los exámenes por el número de conflictos (grado)
    sort(exámenesOrdenados.begin(), exámenesOrdenados.end(), [&matrizConflictos, &examenesList](const string& a, const string& b) {
        int idxA = find(examenesList.begin(), examenesList.end(), a) - examenesList.begin();
        int idxB = find(examenesList.begin(), examenesList.end(), b) - examenesList.begin();
        int gradoA = count(matrizConflictos[idxA].begin(), matrizConflictos[idxA].end(), 1);
        int gradoB = count(matrizConflictos[idxB].begin(), matrizConflictos[idxB].end(), 1);
        return gradoA > gradoB;
    });

    map<string, int> asignacion; // examen -> time-slot
    int timeSlot = 0;

    for (const string& examen : exámenesOrdenados) {
        bool asignado = false;
        while (!asignado) {
            bool conflicto = false;
            for (const auto& [examenAsignado, ts] : asignacion) {
                int idxExamen = find(examenesList.begin(), examenesList.end(), examen) - examenesList.begin();
                int idxExamenAsignado = find(examenesList.begin(), examenesList.end(), examenAsignado) - examenesList.begin();
                if (ts == timeSlot && matrizConflictos[idxExamen][idxExamenAsignado] == 1) {
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
// Función para calcular la penalización total
int calcularPenalizacionTotal(const string& archivoEstudiantes, const map<string, int>& asignacion, const vector<int>& W) {
    ifstream archivo(archivoEstudiantes);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo de estudiantes." << endl;
        exit(1);
    }

    unordered_map<string, vector<int>> examenesPorEstudiante;
    string idEstudiante, idExamen;

    // Leer el archivo y almacenar en el map
    while (archivo >> idEstudiante >> idExamen) {
        examenesPorEstudiante[idEstudiante].push_back(asignacion.at(idExamen));
    }
    archivo.close();

    int penalizacionTotal = 0;

    // Calcular la penalización para cada estudiante
    for (const auto& entry : examenesPorEstudiante) {
        vector<int> horarios = entry.second;
        sort(horarios.begin(), horarios.end());

        for (size_t i = 0; i < horarios.size(); ++i) {
            for (size_t j = i + 1; j < horarios.size(); ++j) {
                int diferencia = horarios[j] - horarios[i];
                if (diferencia <= 5) {
                    penalizacionTotal += W[diferencia - 1];
                } else {
                    break;
                }
            }
        }
    }

    return penalizacionTotal;
}
//Funcion de evaluacion
int funcionEvaluacion(int penalizacion, int cantidadBloques) {
    return penalizacion * cantidadBloques;
}
//Main
int main() {
    string archivoEstudiantes = "./Carleton91.stu";
    string archivoExamenes = "./Carleton91.exm";

    //Constantes
    //Matriz de conflictos
    vector<string> examenesList;
    vector<vector<int>> matrizConflictos = crearMatrizConflictos(archivoEstudiantes, examenesList);//C 
    set<string> A = leerEstudiantes(archivoEstudiantes);//Conjunto de nombres de los alumnos
    set<string> E = leerExamenes(archivoEstudiantes);//Conjunto de nombres de los Examenes
    map<string, int> examenes = cantExamenes(archivoExamenes);//Leer los examenes y sus cantidades
    string T;//Cantidad de bloques en los que se deben realizar los exámenes
    string D;//Bloques por dia
    vector<int> W = {16, 8, 4, 2, 1, 0};

    //Variables
    map<string, vector<Sala>> examSala = countExams(archivoEstudiantes);//Salas y capacidad que utilizan los examen

    map<string, int> asignacion = greedyScheduler(matrizConflictos, examenes, examenesList);
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
    int timeSlotsReq = 0;
    ofstream archivo("./Carleton91.sol");
    if (archivo.is_open()){
        for (const auto& [examen, timeSlot] : asignacion){
            archivo << examen << " " << timeSlot << endl;
            if (timeSlot > timeSlotsReq)
            {
                timeSlotsReq = timeSlot;
            }
        }
        archivo.close();
    } else {
        cerr << "Error al abrir el archivo " << "Carleton91.sol" << endl;
    }

    int penalizacionTotal = calcularPenalizacionTotal(archivoEstudiantes, asignacion, W);

    ofstream file("./Carleton91.pen");
    if (file.is_open()){
        file << penalizacionTotal << endl;
        file.close();
    } else {
        cerr << "Error al abrir el archivo " << "Carleton91.pen" << endl;
    }

    ofstream res("./Carleton91.res");
    if (res.is_open()){
        res << timeSlotsReq << endl;
        res.close();
    } else {
        cerr << "Error al abrir el archivo " << "Carleton91.res" << endl;
    }
    int calidad = funcionEvaluacion(penalizacionTotal, timeSlotsReq);
    cout << "Calidad: " << calidad << endl;
    return 0;

}