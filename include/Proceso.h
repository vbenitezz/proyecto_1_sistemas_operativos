#ifndef PROCESO_H
#define PROCESO_H

#include <vector>
#include <string>

using namespace std;

typedef struct {
    int pid;
    int pc;
    int ax, bx, cx;
    int quantum;
    //"Listo", "Ejecutando", "Terminado"
    char estado[10];
    vector<string> instrucciones;
} Proceso;

#endif