#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>   
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <ctype.h>
#include <fcntl.h>
#include "func.h"

//***************************************************
// TallerAbre: Función manejadora para señales en tiempo real
// Cuando el hijo manda una señal en tiempo real, se ejecuta este
// código, que deberá incrementar el contador de talleres abiertos
//***************************************************
void TallerAbre(int sig, siginfo_t *siginfo, void *context){
    //BEGIN TODO 1 ***********************************************
    printf("[Aseguradora] El taller con PID [%d] ha abierto\n", siginfo->si_value.sival_int);
    
    talleresAbiertos++;
    
    //END TODO 1 ***********************************************
}

//***************************************************
// CREAR PIPES: Función que crea los pipes para los talleres
// Devuelve el número de pipes creados si se crean correctamente
// o -1 en caso de error
//***************************************************
int CrearPipes(int numTalleres){
    //BEGIN TODO 2 ***********************************************
        int i; 
        int retVal = numTalleres;
        // Creación de los pipes de comunicación con los talleres
        for(i = 0; i < numTalleres; i++) {
            if (pipe(talleres[i].p) == -1) {
                retVal = -1;
                break; 
            }
        }   
        return retVal;
        
    //END TODO 2 ***********************************************
}

//***************************************************
// FUNCIÓN que comprueba la existencia de un fichero
//***************************************************
bool CheckIfFileExists(const char* filename){
    if(access(filename, F_OK) == 0) {
        // El fichero existe
        return true;
    } 
    else {
        // El fichero no existe
        return false;  
    }
}

//***************************************************
// isNumber: Función que comprueba si una cadena de caracteres es un número
//***************************************************
bool isNumber(char number[])
{
    int i = 0;
    // Comprobación de números negativos
    if (number[0] == '-')
        i = 1;
    for (; number[i] != 0; i++)
    {
        if (!isdigit(number[i]))
            return false;
    }
    return true;
}

//***************************************************
// Sintaxis: Función que muestra la sintaxis correcta de la llamada
//***************************************************
void Sintaxis(){
    printf("-------------------------------------------------\n");
    printf("Sintaxis del programa:\n");
    printf(" ./reparar <numTalleres> <archivoVehiculos> <archivoResultados>\n");
    printf("    numTalleres       : Entero. Número de talleres mecánicos. Entre %d y %d.\n", MIN_TALLERES, MAX_TALLERES);
    printf("    archivoVehiculos  : String. Nombre del archivo que contiene los vehículos.\n");
    printf("    archivoResultados : String. Nombre del archivo donde se volcarán los resultados.\n");
    printf("-------------------------------------------------\n");
}

//***************************************************
// RandInt: entero aleatorio entre dos valores
//***************************************************
int RandInt(int M, int N){
    return rand() % (N - M + 1) + M;
}

//***************************************************
// Función que lee los vehículos de un archivo de texto y los almacena en 
// un array de cadenas.
//    nombreArchivo Nombre o ruta del archivo que contiene los vehículos.
//    numVehiculos  Parámetro de salida en el que se almacenará 
//                    el número de vehículos leido.
//    return        Un array (dinámico) de cadenas (char*). Cada elemento 
//                    del array es un vehículo. Si ocurre un error, retorna NULL.
//***************************************************
char **leerVehiculos(const char *nombreArchivo, int *numVehiculos) {
    // Abrimos el archivo
    FILE *fp = fopen(nombreArchivo, "r");
    if (!fp) {
        fprintf(stderr, "Error al abrir el archivo %s\n.", nombreArchivo);
        return NULL; 
    }

    // Inicializamos el array de punteros a cadenas (matrículas) y contador
    char **vehiculos = NULL;
    *numVehiculos = 0;

    // Buffer para leer cada línea
    char buffer[12];

    // Leemos línea por línea
    while (fgets(buffer, sizeof(buffer), fp)) {
        // Eliminamos el salto de línea, si lo hay, sustituyéndolo por el final de línea
        buffer[strcspn(buffer, "\n")] = '\0';

        // Redimensionamos el array de matrículas para guardar 1 más
        char **temp = realloc(vehiculos, (*numVehiculos + 1) * sizeof(char*));
        if (!temp) {
            fprintf(stderr, "Error al hacer realloc.\n");
            // Liberamos la memoria ya reservada
            for (int i = 0; i < *numVehiculos; i++) {
                free(vehiculos[i]);
            }
            free(vehiculos);
            fclose(fp);
            return NULL;
        }
        vehiculos = temp;

        // Reservamos espacio para la nueva matrícula y la copiamos
        vehiculos[*numVehiculos] = malloc(strlen(buffer) + 1);
        if (!vehiculos[*numVehiculos]) {
            fprintf(stderr, "Error al hacer malloc.\n");
            // Liberamos la memoria ya reservada
            for (int i = 0; i < *numVehiculos; i++) {
                free(vehiculos[i]);
            }
            free(vehiculos);
            fclose(fp);
            return NULL;
        }
        strcpy(vehiculos[*numVehiculos], buffer);

        (*numVehiculos)++;
    }

    fclose(fp);
    return vehiculos;
}