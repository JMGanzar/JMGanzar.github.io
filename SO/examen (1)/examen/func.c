#include <stdio.h>
#include <stdlib.h>
#include <signal.h>   
#include <unistd.h>   
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/time.h>
#include <ctype.h>
#include "func.h"

//*****************************************************************
//FUNCIÓN que comprueba la existencia de un fichero
//*****************************************************************
bool CheckIfFileExists(const char* filename){

	if( access( filename, F_OK ) == 0 ) {
	    // file exists
	    return true;
	} 
	else {
	    // file doesn't exist
	    return false;	
	}
}


//***************************************************
//isNumber : Función que comprueba si una cadena de caracteres es un número
//***************************************************
bool isNumber(char number[])
{
    int i = 0;

    //checking for negative numbers
    if (number[0] == '-')
        i = 1;
    for (; number[i] != 0; i++)
    {
        //if (number[i] > '9' || number[i] < '0')
        if (!isdigit(number[i]))
            return false;
    }
    return true;
}


//***************************************************
//Sintaxis : Función que vuelca la sintaxis correcta de la llamada
//***************************************************
void Sintaxis(){
	printf("-------------------------------------------------\n");
	printf("Sintaxis del programa:\n");
	printf("procesar <numLaunchers> <numWorkers> <workLoad> <resultsFileName>\n");
	printf("	numLaunchers      : Numero de Launchers a utilizar. Valores entre [1:5]\n");
	printf("	numWorkers        : Numero de Workers por Launcher. Valores entre [1:20]\n");
	printf("	workLoad          : Numero de segundos de la carga de trabajo para cada Worker. Valore entre [1:5]\n");
	printf("	resultsFileName   : Nombre del fichero de resultados. No debe existir en el path actual.\n");
	printf("-------------------------------------------------\n");
}


//***************************************************
//RandInt: ENTERO ALEATORIO ENTRE DOS VALORES
// M    : valor bajo del rango de valores
// N    : valor alto del rango de valores
//***************************************************
int RandInt(int M, int N){
	return rand () % (N-M+1) + M;
}


//***************************************************
//DoWork: Función qeu simula la ejecución del trabajo.
// workLoad    : carga de trabajo en segundos
//***************************************************
int DoWork(int workLoad){
    sleep(workLoad);
    return RandInt(1,100);
}
