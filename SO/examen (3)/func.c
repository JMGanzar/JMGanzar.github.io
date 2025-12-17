#include <stdio.h>
#include <stdlib.h>
#include <signal.h>   
#include <unistd.h>   
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/time.h>
#include "func.h"


//***************************************************
//SINTAXIS : Función que vuelca la sintaxis correcta de la llamada
//***************************************************
void Sintaxis(){
	printf("-------------------------------------------------\n");
	printf("Sintaxis del programa:\n");
	printf("procesar num_hijos max_jobs\n");
	printf("	num_hijos: Numero de hijos que procesarán jobs [1-26]\n");
	printf("	num_jobs:  Numero de jobs que procesarán en total [1-100]\n");
	printf("-------------------------------------------------\n");
}

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
	  hijos[i].jobsDone=0;
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
		//Primero crea el pipe para pasar dados del padre al hijo
		if (pipe(hijos[i].pPC) == -1) {
			retVal=-1;
			break; 
           }
		//Luego crea el pipe para pasar dados del hijo al padre
		if (pipe(hijos[i].pCP) == -1) {
			retVal=-1;
			break; 
           }
	}	
	return retVal; 
}

void DoJob(char *filePath, int childIndex, int carga){
	FILE *fPtr;
	int c; 
	char theChar=childIndex+65;
	fPtr = fopen(filePath, "a");
	for (c=0; c<carga; c++){
		fputc(theChar,fPtr);
	}
	fclose(fPtr);
}


void SaveJobCounts(char *filePath, int numChilds){
	FILE *fPtr;
	char buffer[100];
	int h; 

	fPtr=fopen(filePath,"w");
	for(h=0; h<numChilds; h++){
		sprintf(buffer,"%d\n",hijos[h].jobsDone);
		fputs(buffer,fPtr);
	}
	fclose(fPtr);
}


int GetChildIndex(HIJO *hijos, int numHijos, int childPid){
	int h; 
	int childIndex=-1;
	for (h=0; h<numHijos; h++){
		if (hijos[h].pid==childPid){
			childIndex=h;
			break;
		}
	}
	return childIndex;
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

 
//***************************************************
//ENTERO ALEATORIO ENTRE DOS VALORES
// M    : valor bajo del rango de valores
// N    : valor alto del rango de valores
//***************************************************
int RandInt(int M, int N){
	return rand () % (N-M+1) + M;
}

