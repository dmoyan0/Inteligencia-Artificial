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

//Funcion de evaluacion
int funcionEvaluacion(int penalizacion, int cantidadBloques) {
    return 0.5*penalizacion + cantidadBloques;
}

// Función greedy para asignar exámenes a time-slots
map<string, int> greedyScheduler(const vector<vector<int>>& matrizConflictos, const map<string, int>& examenes, const vector<string>& examenesList, int D) {
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
    int currentDay = 0;

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
            if (!conflicto && (timeSlot / D == currentDay)) {
                asignacion[examen] = timeSlot;
                asignado = true;
            } else {
                timeSlot++;
                if (timeSlot % D == 0) {
                    currentDay++;
                }
            }
        }
    }

    return asignacion;
}

// Función para calcular la penalización total
int calcularPenalizacion(const map<string, int>& solucion, const map<string, set<string>>& examenesPorEstudiante, const vector<int>& W) {
    int penalizacionTotal = 0;

    // Calcular la penalización para cada estudiante
    for (const auto& entry : examenesPorEstudiante) {
        vector<int> horarios;
        for (const auto& examen : entry.second) {
            horarios.push_back(solucion.at(examen));
        }
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


string encontrarExamenPorIndice(const map<string, int>& solucion, int indice) {
    auto it = solucion.begin();
    advance(it, indice);
    return it->first;
}

//Swap
void swapTimeSlots(map<string, int>& asignacion, int i, int j) {
    string Examen1 = encontrarExamenPorIndice(asignacion, i);
    string Examen2 = encontrarExamenPorIndice(asignacion, j);

    swap(asignacion[Examen1], asignacion[Examen2]);
}

// Función para verificar si una solución es válida según la matriz de conflictos
bool esSolucionValida(const map<string, int>& solucion, const vector<vector<int>>& matrizConflictos, const vector<string>& examenesList) {
    for (const auto& [examen1, timeSlot1] : solucion) {
        for (const auto& [examen2, timeSlot2] : solucion) {
            if (timeSlot1 == timeSlot2) {
                int idxExamen1 = find(examenesList.begin(), examenesList.end(), examen1) - examenesList.begin();
                int idxExamen2 = find(examenesList.begin(), examenesList.end(), examen2) - examenesList.begin();
                if (matrizConflictos[idxExamen1][idxExamen2] == 1) {
                    return false;
                }
            }
        }
    }
    return true;
}

// HillClimbing
map<string, int> hillClimbingFirstImprovement(map<string, int> solucionInicial, const map<string, set<string>>& examenesPorEstudiante, const vector<int>& W, const vector<vector<int>>& matrizConflictos, const vector<string>& examenesList) {
    map<string, int> mejorSolucion = solucionInicial;
    int penalizacion = calcularPenalizacion(mejorSolucion, examenesPorEstudiante, W); 
    int CalidadInicial = funcionEvaluacion(penalizacion, mejorSolucion.size());
    cout << "mejor calidad inicial: " << CalidadInicial << endl;

    bool mejoraEncontrada = true;

    while (mejoraEncontrada) {
        mejoraEncontrada = false;

        for (size_t i = 0; i < mejorSolucion.size(); ++i) {
            for (size_t j = i + 1; j < mejorSolucion.size(); ++j) {
                map<string, int> nuevaSolucion = mejorSolucion;
                swapTimeSlots(nuevaSolucion, i, j);

                // Verificar si la nueva solución es válida
                if (!esSolucionValida(nuevaSolucion, matrizConflictos, examenesList)) {
                    continue;
                }

                int nuevaPenalizacion = calcularPenalizacion(nuevaSolucion, examenesPorEstudiante, W);
                int nuevaCalidad = funcionEvaluacion(nuevaPenalizacion, nuevaSolucion.size());
                cout << "Nueva calidad: " << nuevaCalidad << endl;
                if (nuevaCalidad < CalidadInicial) {
                    mejorSolucion = nuevaSolucion;
                    mejoraEncontrada = true;
                    cout << "ENCONTRE" << endl;
                    return mejorSolucion;
                }
            }
            if (mejoraEncontrada) break;
        }
    }

    return mejorSolucion;
}

//Main
int main() {
    string archivoEstudiantes = "./Carleton91.stu";
    string archivoExamenes = "./Carleton91.exm";
    ifstream archivoStu(archivoEstudiantes);

    //Constantes
    //Matriz de conflictos
    vector<string> examenesList;
    vector<vector<int>> matrizConflictos = crearMatrizConflictos(archivoEstudiantes, examenesList);//C 
    set<string> A = leerEstudiantes(archivoEstudiantes);//Conjunto de nombres de los alumnos
    set<string> E = leerExamenes(archivoEstudiantes);//Conjunto de nombres de los Examenes
    map<string, int> examenes = cantExamenes(archivoExamenes);//Leer los examenes y sus cantidades
    string T;//Cantidad de bloques en los que se deben realizar los exámenes
    int D = 10;//Bloques por dia
    vector<int> W = {16, 8, 4, 2, 1, 0};
    map<string, set<string>> examenesPorEstudiante;
    string idEstudiante, idExamen;
    while (archivoStu >> idEstudiante >> idExamen)
    {
        examenesPorEstudiante[idEstudiante].insert(idExamen);
    }
    archivoStu.close();


    //Variables
    map<string, vector<Sala>> examSala = countExams(archivoEstudiantes);//Salas y capacidad que utilizan los examen
    map<string, int> asignacion = greedyScheduler(matrizConflictos, examenes, examenesList, D);
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

    int penalizacionTotal = calcularPenalizacion(asignacion, examenesPorEstudiante, W);

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
    int calidad = funcionEvaluacion(penalizacionTotal, asignacion.size());
    cout << "Calidad Greedy: " << calidad << endl;  
    
    //Falta definir iteraciones del HCAM
    map<string, int> mejorSolucion = hillClimbingFirstImprovement(asignacion, examenesPorEstudiante, W, matrizConflictos, examenesList);
    int mejorPenalizacion = calcularPenalizacion(mejorSolucion, examenesPorEstudiante, W);
    int mejorCalidad = funcionEvaluacion(mejorPenalizacion, mejorSolucion.size());


    cout << "Calidad Hill Climbing: " << mejorCalidad << endl;
    return 0;

}