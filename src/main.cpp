#include <iostream>
#include <cstring> //char[]
#include <fstream> //std::ifstream para leer archivos
#include <sstream> //usar streams de texto en memoria como si fueran archivos
#include <vector> //arreglo dinámico
#include <string>
//random
#include <cstdlib>
#include <ctime>
//ms
#include <thread>
#include <chrono>

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include "../include/Proceso.h"
#include "../include/DualStreambuf.h"

using namespace std; //usar elementos del espacio de nombres std sin escribir std::

#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

termios termOriginal;

void configurarTerminal() {
    tcgetattr(STDIN_FILENO, &termOriginal);

    termios term = termOriginal;
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void restaurarTerminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &termOriginal);
}

bool teclaPresionada() {
    int oldf = fcntl(STDIN_FILENO, F_GETFL);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    char c;
    int n = read(STDIN_FILENO, &c, 1);

    fcntl(STDIN_FILENO, F_SETFL, oldf);

    return (n > 0);
}

Proceso leerProceso(const string& proceso) {
    Proceso proc;

    proc.pc = 0;
    proc.ax = 0;
    proc.bx = 0;
    proc.cx = 0;
    proc.quantum = 1;
    proc.ejecuciones = 0;

    strcpy(proc.estado, "Listo");

    string bloque;
    stringstream ssProc(proceso);

    while (getline(ssProc, bloque, ',')) {
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

void ejecutarIns(Proceso* proceso) {
    if(proceso->pc >= proceso->instrucciones.size()) {
        strcpy(proceso->estado, "Terminado");
        return;
    }

    string ins = proceso->instrucciones[proceso->pc];
    stringstream ss(ins);
    string oper, arg1, arg2;
    ss >> oper >> arg1 >> arg2;

    if(oper == "NOP") {
        proceso->pc++;
    } else if(oper == "INC") {
        if(arg1 == "AX") proceso->ax++;
        else if(arg1 == "BX") proceso->bx++;
        else if(arg1 == "CX") proceso->cx++;
        else cout << "Error en INC" << endl;
        proceso->pc++;
    } else if(oper == "JMP") {
        int salto = stoi(arg1);
        if(salto >= 0 && salto < proceso->instrucciones.size()) {
            proceso->pc = salto;
        } else {
            cout << "Error en JMP" << endl;
            proceso->pc = proceso->instrucciones.size();
            strcpy(proceso->estado, "Terminado");
        }
    } else if(oper == "ADD" || oper == "SUB" || oper == "MUL") {
        if(!arg1.empty() && arg1.back() == ',') {
            arg1.pop_back();
        }
        int* ar1 = nullptr;
        int ar2;

        if(arg1 == "AX") ar1 = &proceso->ax;
        else if(arg1 == "BX") ar1 = &proceso->bx;
        else if(arg1 == "CX") ar1 = &proceso->cx;

        if(arg2 == "AX") ar2 = proceso->ax;
        else if(arg2 == "BX") ar2 = proceso->bx;
        else if(arg2 == "CX") ar2 = proceso->cx;
        else ar2 = stoi(arg2);

        if(oper == "ADD") *ar1 += ar2;
        else if(oper == "SUB") *ar1 -= ar2;
        else if(oper == "MUL") *ar1 *= ar2;

        proceso->pc++;
    } else {
        cout << "else" << endl;
        proceso->pc++;
    }
}

void simularRoundRobin(vector<Proceso> &procesos) {
    int numProcesos = procesos.size();
    int actual = 0;
    bool todosTerminados = false;
    int interrup;
    int nuevoActual;
    vector<Proceso*> procesosInterrup;
    bool nuevaInterrup = false;
    
    while(!todosTerminados) {
        todosTerminados = true;
        Proceso* procesoActual;
        if(nuevaInterrup) {
            nuevaInterrup = false;
            actual = (actual + 2) % numProcesos;
            procesoActual = &procesos[actual];
        } else {
            if(procesosInterrup.empty()) {
                procesoActual = &procesos[actual];
            } else {
                procesoActual = procesosInterrup[0];
                procesosInterrup.erase(procesosInterrup.begin());
            }

        }
        
        if(strcmp(procesoActual->estado, "Terminado") != 0) {
            todosTerminados = false;

            cout << "[Cambio de contexto]" << endl;
            cout << "Cargando estado de Proceso " << procesoActual->pid
                 << ": PC = " << procesoActual->pc
                 << ", AX = " << procesoActual->ax
                 << ", BX = " << procesoActual->bx 
                 << ", CX = " << procesoActual->cx
                 << ", Estado: " << "Ejecutando" << endl;
            this_thread::sleep_for(chrono::milliseconds(1500));
            
            interrup = 1;
            for(int i = 0; i < procesoActual->quantum; i++) {
                if (teclaPresionada()) {
                    interrup = 0;
                    break;
                }
                procesoActual->ejecuciones++;
                ejecutarIns(procesoActual);
                if(strcmp(procesoActual->estado, "Terminado") == 0) break;
                if(procesoActual->pc >= procesoActual->instrucciones.size()) {
                    strcpy(procesoActual->estado, "Terminado");
                    break;
                }
                if(procesoActual->ejecuciones >= 100) {
                    cout << "CICLO INFINITO" << endl;
                    strcpy(procesoActual->estado, "Terminado");
                    break;
                }
            }

            cout << "Guardando estado de Proceso " << procesoActual->pid
                 << ": PC = " << procesoActual->pc
                 << ", AX = " << procesoActual->ax
                 << ", BX = " << procesoActual->bx 
                 << ", CX = " << procesoActual->cx;

            if(strcmp(procesoActual->estado, "Terminado") == 0) {
                cout << ", Estado: " << "Terminado" << endl;
            } else if(interrup == 0) {
                cout << ", Estado: " << "Interrumpido" << endl;
            } else {
                cout << ", Estado: " << "Listo" << endl;
            }

            if(interrup != 0) {
                actual = (actual + 1) % numProcesos;
            } else {
                procesosInterrup.push_back(procesoActual);
                // nuevoActual = rand() % numProcesos;
                // if(nuevoActual != actual) {
                //     actual = nuevoActual;
                // } else {
                //     while(nuevoActual == actual) {
                //         nuevoActual = rand() % numProcesos;
                //     }
                //     actual = nuevoActual;
                // }
                nuevaInterrup = true;
            }
            
        } else {
            for(int i = 0; i < numProcesos; i++) {
                if(strcmp(procesos[i].estado, "Terminado") != 0) {
                    todosTerminados = false;
                    actual = i;
                    break;
                }
            }
        }
    }
}

int main() {
    ofstream logfile("registro.txt");

    dual_streambuf dsbuf(cout.rdbuf(), logfile.rdbuf());
    cout.rdbuf(&dsbuf);

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

    srand(time(0));
    configurarTerminal();
    simularRoundRobin(procesos);

    restaurarTerminal();
    cout << "Programa terminado.\n";
    return 0;
}