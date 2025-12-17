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

//***************************************************
//CREAR HIJOS : Función que crea un array dinámico de estructuras HIJO
//              Devuelve el numero de hijos si se crea correctamente el array
//              o -1 en caso de error
//***************************************************
int NewArrayHijos(int numHijos){
	int i; 
	hijos=(HIJO *)malloc(numHijos*sizeof(HIJO));
	if (hijos==NULL) {
		printf("Error allocating memory!\n"); 
		return -1; 
	}   
	//Inicializamos las estructuras de hijos
	for(i=0; i<numHijos;i++){ 
	  hijos[i].pid=i;
	}

	return numHijos;
}


//***************************************************
//CREAR PIPES : Función que crea los pipe para los hijos dado el array de hijos
//              Devuelve el numero de pipes creados si se crean correctamente el array
//              o -1 en caso de error
//***************************************************
int CrearPipes(int numHijos){
	int i; 
	int retVal=numHijos;
	//Creación de los pipes de comunicación con los hijos.
	for(i=0; i < numHijos; i++) { //El padre itera creando los pipes
		if (pipe(hijos[i].p) == -1) {
			retVal=-1;
			break; 
           }
	}	
	return retVal; 
}


//***************************************************
//LECTURA DE UN ENTERO DE UN PIPE 
//      fdPipe     : descriptor del pipe del que leer
// retorna
//      dato       : entero leido del pipe
//***************************************************
int LeerPipe(int fdPipe){
	int dato,n;
    n=read(fdPipe, &dato, sizeof(int));
    return dato; 
}

//***************************************************
//ESCRITURA DE UN ENTERO EN UN PIPE 
// fdPipe     : descriptor del pipe en el que escribir
// dato       : entero a ecribirlos en el pipe
//***************************************************
void EscribirPipe(int fdPipe, int dato){
    write(fdPipe, &dato, sizeof(int));
}


//*****************************************************************
//FUNCIÓN que traduce una apuesta en su string (R,P,S)  (Rock, Paper, Scissors)
//*****************************************************************
char GetCharBet(int bet){
	char retVal;

	switch(bet){
		case 0: 
			retVal='R';
			break;
		case 1: 
			retVal='P';
			break;
		case 2: 
			retVal='S';
			break;
		default:
			retVal = '-';
			break;
	}
	return retVal;
}




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
	printf("rps <playerA> <playerB> <bestOf>\n");
	printf("	playerA        : Integer. Number of the player as PlayerA\n");
	printf("	playerB        : Integer. Number of the player as PlayerB\n");
	printf("	bestOf         : Integer. Number of hands to play. Valid values 3, 5, 7 or 9\n");
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


