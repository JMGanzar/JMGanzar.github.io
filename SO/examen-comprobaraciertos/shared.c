#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>   
#include <signal.h>   
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>   
#include <sys/ipc.h>   
#include <sys/shm.h>  
#include "./sem.h"
#include "./func.h"

#define SSEED 'a'               //Semilla para los semáforos
#define SHMFILE "~/.profile"    //Fichero único por alumno usado para obtener el id de la memoria compartida
#define SHMSEED 35              //Semilla para obtener el id de la memoria compartida
#define SHMPERMISOS 0644        //Permisos para obtener el id de la memoria compartida

int main( int args, char *argv[] )
{
	int i,j,x,y;  //variables auxiliares
	int pid;      //para contener el pid de los hijos
	int numHijos;  //Numero de hijos (4 o 16)
	int status;    //Status de retorno de los hijos
	char buffer[100];  //buffer auxiliar para mostrar errores
	int shmId;  //Identificador de segmento de memoria compartida
	int shmSize; //Tamaño del segmento de memoria compartida
	int *shmArray; //Puntero donde queremos ubicar el segmento de memoria compartida.
	int *numeros;  //Puntero a un array que contendrá los numeros que genera cada hijo. 
    int numsPorHijo; //Cantidad de números que genera cada hijo.

	//Obtenemos el número de hijos de los paramateros de entrada. 
	//Si no hay exactamente 2 parámetros llamamos a Sintaxis() y terminadmos con exit(-1)
	//printf("Numero de parámetros = %d\n",args);
	//**** INICIO COMPLETAR--------------------------------------------------
	numHijos=...
	//**** FIN COMPLETAR--------------------------------------------------

    //Comprobamos que el parámetro de numHijos sea correcto, es decir un 4 o un 16
	//Si no es correcto se llama a Sintaxis y se termina con exit(-1)
	//printf("Numero de hijos = %d\n",numHijos);
	//**** INICIO COMPLETAR--------------------------------------------------
    ...
	//**** FIN COMPLETAR--------------------------------------------------

    //Calculamos la cantidad de números que le tocan a cada hijo.
	numsPorHijo=(NFIL*NCOL)/numHijos;

	//Declaramos el identificadordel semáforo que juega el rol de barrera del padre
	int sBarreraPadre;

	//Creamos el semáforo inicializado a cero
	//**** INICIO COMPLETAR--------------------------------------------------
    ...
	//**** FIN COMPLETAR--------------------------------------------------


	//Declararmos el array sACKS de semáforos para los ACKs de los hijos y los inicializamos a cero
	int sACKS[numHijos];
	//**** INICIO COMPLETAR--------------------------------------------------
    ...
	//**** FIN COMPLETAR--------------------------------------------------

	
	//Obtenemos el tañamo del segmento de memoria a pedir
	shmSize = NFIL * NCOL * sizeof(int);
	
	//Creamos el segmento de memoria compartida y asignamos los punteros.
	//**** INICIO COMPLETAR--------------------------------------------------
    ...
	//**** FIN COMPLETAR--------------------------------------------------

	//Se inicializa el array de memoria compartida a ceros
	//**** INICIO COMPLETAR--------------------------------------------------
    ...
	//**** FIN COMPLETAR--------------------------------------------------

	//Imprimimos la matriz de memoria compartida, que esta todo a ceros
	PrintMatrix(shmArray);

	//Bucle para crear a los hijos
	//En el bucle los hijos envían su señal, reciben su ACK, operan y terminan.
	for(i=0; i < numHijos; i++)	{ 
		//El padre crea el hijo
		//**** INICIO COMPLETAR--------------------------------------------------
	    ...
		//**** FIN COMPLETAR--------------------------------------------------
		if(pid == -1){   
		   sprintf(buffer,"ERROR. Fork ha fallado al crear al hijo %d\n",i);
		   perror(buffer);
		   exit(-1);
		}
		if(pid == 0){
			//Código hijo

		    //El hijo pide memoria para su array de numeros.
			//**** INICIO COMPLETAR--------------------------------------------------
		    ...
			//**** FIN COMPLETAR--------------------------------------------------
			
			//El hijo inicializa la semilla de numeros aleatorios con su pid
			//**** INICIO COMPLETAR--------------------------------------------------
		    ...
			//**** FIN COMPLETAR--------------------------------------------------

			//El hijo Genera sus números 
			//**** INICIO COMPLETAR--------------------------------------------------
		    ...
			//**** FIN COMPLETAR--------------------------------------------------

			//El hijo indica que ha llegado a la barrera
			//**** INICIO COMPLETAR--------------------------------------------------
		    ...
			//**** FIN COMPLETAR--------------------------------------------------

			//El hijo espera en su semaforo de ACK para seguir.
			//**** INICIO COMPLETAR--------------------------------------------------
		    ...
			//**** FIN COMPLETAR--------------------------------------------------
		   
		    //El hijo asigna en la memoria compartida el numero de aciertos
			//**** INICIO COMPLETAR--------------------------------------------------
		    ...
			//**** FIN COMPLETAR--------------------------------------------------
			
			//El hijo imprime su fichero de aciertos
			//**** INICIO COMPLETAR--------------------------------------------------
		    ...
			//**** FIN COMPLETAR--------------------------------------------------
			
			//El hijo se desvincula del array.
			//**** INICIO COMPLETAR--------------------------------------------------
		    ...
			//**** FIN COMPLETAR--------------------------------------------------
		   
			//El hijo termina, no itera.
			//**** INICIO COMPLETAR--------------------------------------------------
		    ...
			//**** FIN COMPLETAR--------------------------------------------------
		}
	}
    //Aquí sólo llega el padre
    
    //El padre espera a que todos los hijos lleguen a la barrera
	//**** INICIO COMPLETAR--------------------------------------------------
    ...
	//**** FIN COMPLETAR--------------------------------------------------


	//El padre inicializa la semilla de numeros aleatorios con su pid
	//**** INICIO COMPLETAR--------------------------------------------------
    ...
	//**** FIN COMPLETAR--------------------------------------------------

	//El padre genera en la memoria compartida los valores ganadores 
	//**** INICIO COMPLETAR--------------------------------------------------
    ...
	//**** FIN COMPLETAR--------------------------------------------------

	PrintMatrix(shmArray);
	
	//El padre envía los ACKs
	//**** INICIO COMPLETAR--------------------------------------------------
    ...
	//**** FIN COMPLETAR--------------------------------------------------
   
	//Bucle para esperar la terminación de los hijos y notificarlo
	//**** INICIO COMPLETAR--------------------------------------------------
    ...
	//**** FIN COMPLETAR--------------------------------------------------

	PrintMatrix(shmArray);
	
	//El padre se desvincula del array.
	//**** INICIO COMPLETAR--------------------------------------------------
    ...
	//**** FIN COMPLETAR--------------------------------------------------
	
	//El padre borra el array
	//**** INICIO COMPLETAR--------------------------------------------------
    ...
	//**** FIN COMPLETAR--------------------------------------------------

	return 0;
}
