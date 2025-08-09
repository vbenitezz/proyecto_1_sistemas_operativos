# Proyecto 1 Sistemas Operativos

## Compilar
g++ -o bin/compilar src/main.cpp -Iinclude

## Ejecutar
./bin/compilar

## Estructura del proyecto
.    
├── bin/    
│   └── compilar (ejecutable principal del programa)    
├── include/    
│   ├── DualStreambuf.h (redirige cout a consola y a registro.txt simultáneamente)    
│   └── Proceso.h (estructura de datos que representa a un proceso)    
├── src/    
│   └── main.cpp (lógica principal del programa)    
├── procesos.txt (lista de procesos a cargar)    
├── PID.txt (instrucciones de cada proceso (uno por archivo, identificado por PID)    
├── registro.txt (log con todo lo mostrado por consola)    
└── README.md (instrucciones de compilación y ejecución)    

## Descripción del funcionamiento

* Estructura de procesos definida en Proceso.h.
* Carga de procesos leyendo procesos.txt.
* Lectura de instrucciones para cada proceso desde su archivo PID.txt.
* Ejecución de instrucciones, procesando una a la vez según el contador de programa (pc).
* Planificador Round-Robin que administra la ejecución de procesos.
* Detección de entrada de teclado para generar interrupciones manuales.
* Clase dual_streambuf para registrar en registro.txt todo lo que se muestra por consola.

## Consideraciones técnicas
* Compilador: g++ 13.3.0
* Procesador: Intel(R) Core(TM) i5-8265U CPU @ 1.60GHz
* Uso de memoria: ~3.6 MB de RAM
* Tiempo de ejecución en prueba: ~12 segundos (la mayor parte en sleep o esperando entrada del usuario)

## Estructura del archivo procesos.txt
PID:<número>,AX=<número>,BX=<número>,CX=<número>,Quantum=<número>        
    
PID:1,AX=0,BX=1,CX=2,Quantum=3    
PID:2,AX=1,BX=3,CX=4,Quantum=2    
PID:3,AX=2,BX=5,CX=6,Quantum=1    
PID:4,AX=3,BX=7,CX=8,Quantum=2    
PID:5,AX=4,BX=9,CX=10,Quantum=3    

## Estructura de las instrucciones
Las instrucciones deben escribirse cada una en una línea independiente, separando los elementos con un espacio.    
Esto es importante porque el programa las leerá usando la estructura:   
    
ss >> oper >> arg1 >> arg2;    
     
ADD AX, 2    
SUB BX, 1    
MUL CX, 8    
INC AX    
JMP 0    

## Conclusiones 
Más que conclusiones, estas son observaciones y mejoras a tener en cuenta en caso de desarrollar una nueva versión del mismo proyecto base:
* Implementar una cola en el planificador round-robin. Actualmente, me guío por el estado del proceso para decidir cuál debe ejecutarse.
* Los estados de los procesos realmente solo toman los valores Listo y Terminado; los estados Ejecutando e Interrumpido únicamente se muestran por consola mediante cout.
* Las interrupciones no siempre funcionan correctamente. Al producirse una interrupción, se ejecuta el proceso que está dos posiciones más adelante y luego se reanuda el proceso que se estaba ejecutando antes de la interrupción, llegando incluso a ejecutarse dos veces.
* Incorporar un archivo Makefile para la compilación del proyecto.
* Mejorar la estructura del proyecto, dividiendo el código en más archivos según sus funcionalidades, haciendo un uso más eficiente de las clases y reorganizando la estructura general en torno a una única clase principal.





