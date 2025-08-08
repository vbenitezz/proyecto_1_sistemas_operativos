#include <iostream>
#include <cstring> //char[]
#include <fstream> //std::ifstream para leer archivos
#include <sstream> //usar streams de texto en memoria como si fueran archivos
#include <vector> //arreglo dinámico
#include <string>

#include "../include/Proceso.h"

using namespace std; //usar elementos del espacio de nombres std sin escribir std::

Proceso leerProceso(const string& proceso) {
    Proceso proc;

    proc.pc = 0;
    proc.ax = 0;
    proc.bx = 0;
    proc.cx = 0;
    proc.quantum = 1;

    strcpy(proc.estado, "Listo");

    string bloque;
    stringstream ss_proc(proceso);

    while (getline(ss_proc, bloque, ',')) {
        if (bloque.find("PID:") != string::npos) {
            proc.pid = stoi(bloque.substr(4));
        } else if (bloque.find("AX=") != string::npos) {
            proc.ax = stoi(bloque.substr(3));
        } else if (bloque.find("BX=") != string::npos) {
            proc.bx = stoi(bloque.substr(3));
        } else if (bloque.find("CX=") != string::npos) {
            proc.cx = stoi(bloque.substr(3));
        } else if (bloque.find("Quantum=") != string::npos) {
            proc.quantum = stoi(bloque.substr(8));
        }
    }
    return proc;
}

void leerInstrucciones(Proceso &proceso) {
    string nombreArchivo = to_string(proceso.pid) + ".txt";
    ifstream archivo(nombreArchivo);
    string ins;

    while(getline(archivo, ins)) {
        if(!ins.empty()) {
            proceso.instrucciones.push_back(ins);
        }
    }

    archivo.close();
}

void ejecutarIns(Proceso& proceso) {
    if(proceso.pc >= proceso.instrucciones.size()) {
        strcpy(proceso.estado, "Terminado");
        return;
    }

    string ins = proceso.instrucciones[proceso.pc];
    stringstream ss(ins);
    string oper, arg1, arg2;
    ss >> oper >> arg1 >> arg2;

    // cout << ss << endl;
    // cout << oper << endl;
    // cout << arg1 << endl;
    // cout << arg2 << endl;

    if(oper == "NOP") {
        proceso.pc++;
    } else if(oper == "INC") {
        if(arg1 == "AX") proceso.ax++;
        else if(arg1 == "BX") proceso.bx++;
        else if(arg1 == "CX") proceso.cx++;
        else cout << "Error en INC" << endl;
        proceso.pc++;
    } else if(oper == "JMP") {
        int salto = stoi(arg1);
        if(salto >= 0 && salto < proceso.instrucciones.size()) {
            proceso.pc = salto;
        } else {
            cout << "Error en JMP" << endl;
            proceso.pc = proceso.instrucciones.size() - 1;
            strcpy(proceso.estado, "Terminado");
        }
    } else if(oper == "ADD" || oper == "SUB" || oper == "MUL") {
        if(!arg1.empty() && arg1.back() == ',') {
            arg1.pop_back();
        }
        int* ar1 = nullptr;
        int ar2;

        if(arg1 == "AX") ar1 = &proceso.ax;
        else if(arg1 == "BX") ar1 = &proceso.bx;
        else if(arg1 == "CX") ar1 = &proceso.cx;

        if(arg2 == "AX") ar2 = proceso.ax;
        else if(arg2 == "BX") ar2 = proceso.bx;
        else if(arg2 == "CX") ar2 = proceso.cx;
        else ar2 = stoi(arg2);

        if(oper == "ADD") *ar1 += ar2;
        else if(oper == "SUB") *ar1 -= ar2;
        else if(oper == "MUL") *ar1 *= ar2;

        proceso.pc++;
    }
}

// void simularRoundRobin(vector<Proceso> &procesos) {
//     int numProcesos = procesos.size();
//     int actual = 0;
//     bool todosTerminados = false;
    
//     while(!todosTerminados) {
//         todosTerminados = true;
//         Proceso &procesoActual = procesos[actual];

//         if(strcmp(procesoActual.estado, "Terminado") != 0) {
//             todosTerminados = false;

//             cout << "[Cambio de contexto]" << endl;
//             cout << "Cargando estado de Proceso " << procesoActual.pid
//                  << ": PC=" << procesoActual.pc
//                  << ", AX=" << procesoActual.ax
//                  << ", BX=" << procesoActual.bx 
//                  << ", CX=" << procesoActual.cx << endl;
            
//             for(int i = 0; i < procesoActual.quantum; i++) {
//                 ejecutarIns(procesoActual);
//                 if(strcmp(procesoActual.estado, "Terminado") == 0) break;
//             }

//             cout << "Guardando estado de Proceso " << procesoActual.pid
//                  << ": PC=" << procesoActual.pc
//                  << ", AX=" << procesoActual.ax
//                  << ", BX=" << procesoActual.bx 
//                  << ", CX=" << procesoActual.cx << endl;
//             cout << endl;

//             actual = (actual + 1) % numProcesos;
//         } else {
//             for(int i = 0; i < numProcesos; i++) {
//                 if(strcmp(procesos[i].estado, "Terminado") != 0) {
//                     todosTerminados = false;
//                     actual = i;
//                     break;
//                 }
//             }
//         }
//     }
// }

// void simularRoundRobin(vector<Proceso> &procesos) {
//     int numProcesos = procesos.size();
//     int procesosTerminados = 0;
//     int indiceActual = 0;
//     int ultimoProceso = -1; // para detectar el cambio de contexto

//     while (procesosTerminados < numProcesos) {
//         Proceso &p = procesos[indiceActual];

//         if (strcmp(p.estado, "Terminado") == 0) {
//             indiceActual = (indiceActual + 1) % numProcesos;
//             continue;
//         }

//         // Cambio de contexto
//         if (ultimoProceso != -1 && ultimoProceso != p.pid) {
//             Proceso &anterior = procesos[ultimoProceso - 1];
//             cout << "[Cambio de contexto]" << endl;
//             cout << "Guardando estado de Proceso " << anterior.pid
//                  << ": PC=" << anterior.pc
//                  << ", AX=" << anterior.ax
//                  << ", BX=" << anterior.bx 
//                  << ", CX=" << anterior.cx << endl;
//             cout << "Cargando estado de Proceso " << p.pid
//                  << ": PC=" << p.pc
//                  << ", AX=" << p.ax
//                  << ", BX=" << anterior.bx 
//                  << ", CX=" << anterior.cx << endl;
//         }

//         strcpy(p.estado, "Ejecutando");

//         // Ejecutar hasta 'quantum' instrucciones
//         int instruccionesEjecutadas = 0;
//         while (instruccionesEjecutadas < p.quantum && strcmp(p.estado, "Terminado") != 0) {
//             ejecutarIns(p);
//             instruccionesEjecutadas++;
//         }

//         if (strcmp(p.estado, "Terminado") == 0) {
//             procesosTerminados++;
//         } else {
//             strcpy(p.estado, "Listo");
//         }

//         ultimoProceso = p.pid;
//         indiceActual = (indiceActual + 1) % numProcesos;
//     }

//     cout << "Todos los procesos han terminado." << endl;
// }


int main() {
    ifstream archivo("procesos.txt");
    vector<Proceso> procesos;

    string proceso;
    while(getline(archivo, proceso)) {
        if(!proceso.empty()) {
            Proceso p = leerProceso(proceso);
            leerInstrucciones(p);
            procesos.push_back(p);
        }  
    }
    archivo.close();

    for (const Proceso& p : procesos) {
        cout << "PID: " << p.pid << ", AX: " << p.ax << ", BX: " << p.bx << ", CX: " << p.cx
             << ", Quantum: " << p.quantum << ", Estado: " << p.estado << endl;

        cout << "Instrucciones:" << endl;
        for (const string& inst : p.instrucciones) {
            cout << "  " << inst << endl;
        }
        cout << "---------------------" << endl;
    }

    simularRoundRobin(procesos);

    return 0;
}