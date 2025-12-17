#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/time.h>
#include <ctype.h>
#include <fcntl.h>
#include "func.h"

//***************************************************
// CREAR PIPES: Función que crea los pipes para las comunidades
// Devuelve el número de pipes creados si se crean correctamente
// o -1 en caso de error
//***************************************************
int CrearPipes(int numComunidades){
    //BEGIN TODO 1 ***********************************************
        int i; 
        int retVal = numComunidades;
        // Creación de los pipes de comunicación con las comunidades
        for(i = 0; i < numComunidades; i++) {
            if (pipe(comunidades[i].p) == -1) {
                retVal = -1;
                break; 
            }
        }   
        return retVal;
    //END TODO 1 ***********************************************
}

//***************************************************
// Recibir por pipe un número float
//***************************************************
float LeerParticipacion(int fdPipe){
    //BEGIN TODO 2 ***********************************************
        float dato;
        read(fdPipe, &dato, sizeof(float));
        return dato;
    //END TODO 2 ***********************************************
}

//***************************************************
// Enviar por pipe un número float
//***************************************************
void EscribirParticipacion(int fdPipe, float dato){
    //BEGIN TODO 3 ***********************************************
        write(fdPipe, &dato, sizeof(float));
    //END TODO 3 ***********************************************
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
    printf(" ./votar <archivoVotantes> <numPartidos>\n");
    printf("    archivoVotantes   : String. Nombre del archivo de votantes.\n");
    printf("    archivoResultados : String. Nombre del archivo donde se volcarán los resultados.\n");
    printf("    numPartidos       : Entero. Número de partidos políticos. Entre 2 y 6\n");
    printf("-------------------------------------------------\n");
}

//***************************************************
// RandInt: entero aleatorio entre dos valores
//***************************************************
int RandInt(int M, int N){
    return rand() % (N - M + 1) + M;
}

//***************************************************
// contarLineas: cuenta las líneas que tiene un fichero
//***************************************************
int contarLineas(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error al abrir el archivo.\n");
        return -1;
    }

    int lines = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            lines++;
        }
    }

    fclose(file);
    return lines;
}

//***************************************************
// extraer_string: extrae información de un string (input) con formato `CADENA,NUMERO`
//                 guarda en srt `CADENA` y en num `NUMERO`.
//***************************************************
int extraer_string(const char *input, char *str, int *num) {
    if (sscanf(input, "%99[^,],%d", str, num) == 2) {
        return 1; // Éxito
    }
    return 0; // Error si no se pudo analizar correctamente
}


//***************************************************
// leerVotantes: Lee el archivo de votantes (filename) obtenido en bash. Se debe conocer previamente
//               el número de comunidades autónomas (numComunidades).
//               Esta función asignará al vector de COMUNIDADES el nombre y el número de votantes.
//               Si todo va bien, devuelve 0, si no, devuelve -1.
//***************************************************
int leerVotantes(const char *filename, int numComunidades){
    //BEGIN TODO 4 ***********************************************
        // Abrir el archivo en modo lectura
        // Si hay error, mostramos mensaje y salimos con -1.
        FILE *file = fopen(filename, "r");
        if (file == NULL) {
            printf("Error al abrir el archivo %s.\n", filename); // Imprimir error si el archivo no se abre
            return -1;
        }

        // Buffer para leer cada línea del archivo
        char line[150]; 

        // Por cada comunidad autónoma
        for(int count=0; count<numComunidades; count++){

            // Guardamos el contenido de la línea en el buffer file
            if (fgets(line, sizeof(line), file) == NULL) {
                break; // Salir del bucle si no hay más líneas
            }

            // Variables temporales
            char nombre[50];
            int votantes;

            // Extraer el nombre y el número de votantes usando la función extraer_string
            // Si hay error, cerrar fichero, mostrar mensaje y salir con -1.
            if (!extraer_string(line, nombre, &votantes)) {
                fclose(file);
                printf("Error al extraer nombre,votantes de la linea %d <%s>.\n", count, line);
                return -1;
            }

            // Actualizamos los datos de la comunidad correspondiente (vector global de estructura COMUNIDADES)
            strcpy(comunidades[count].nombre, nombre);
            comunidades[count].num_votantes = votantes;
        }

        // Cierre del fichero y devolvemos 0
        fclose(file);
        return 0;
    //END TODO 4 ***********************************************
}

//***************************************************
// generar_porcentaje_aleatorio: número aleatorio (float) entre min y max (float)
//***************************************************
float generar_porcentaje_aleatorio(float min, float max) {
    // rand() devuelve un número entero entre 0 y RAND_MAX
    // Dividimos por RAND_MAX para obtener un valor entre 0.0 y 1.0
    float aleatorio = (float)rand() / RAND_MAX;
    return min + aleatorio * (max - min);
}