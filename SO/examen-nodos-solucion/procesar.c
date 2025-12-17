#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   
#include <string.h>
#include <sys/ipc.h>   
#include <stdbool.h>
#include <sys/wait.h>
#include <time.h>
#include "func.h"
#include "sem.h"

#define SSEED 99

//Programa principal
int main( int argc, char *argv[] )
{
	int status;
	char buffer[100];
	char depFilename[100];

	int i,c,data;       
	int pid;    
	int carga=0; 
	int numSignals=0;
	int numWaits=0; 

	int numProcs;

	// Start measuring time
    struct timespec begin, end; 
    clock_gettime(CLOCK_REALTIME, &begin);

	//Control de paraḿetros
	//TODO-BEGIN MAIN1 
		//Controlamos el numero de parametros. 
		//Sólo permitimos el nombre del fichero de dependencias
		//Si pasan más o menos parametros mostramos mensaje, sintaxis y terminamos con -1
		if(argc != 2){
			printf("Error en el numero de parametros\n");
			Sintaxis();
			exit(-1);
		}
	//TODO-END-------------------------

	//Obtenemos el numero de procesos 
	strcpy(depFilename,argv[1]);
	numProcs=NumProcesos(depFilename);
	printf("Procesando grafo %s con %d procesos, espere por favor...\n",depFilename, numProcs);
	
	//Declararmos el array de semáforos para los Procesos
	int sProcs[numProcs];
	
	//TODO-BEGIN MAIN2
		//Creamos los semáforos en el array inicializandolos a cero
		for(i=0; i<numProcs; i++) 
			sProcs[i]=sCreate(SSEED+i+1,0);
	//TODO-END-------------------------

	//TODO-BEGIN MAIN3
		//Creamos el array de estrucutras procesos llamando a la función NewArrayProcesos
		//Esta funcion crea el array procesos, declarado globalmente en func.h
		//Si la función no devuelve el numero de procesos mostramos error y salimos con -1
		if (numProcs!=NewArrayProcesos(numProcs)){
			printf("ERROR en la creación del array de procesos\n");
			exit(-1); 
		}
		//For-Debug-Only
		//PrintProcesos(numProcs);
	//TODO-END-------------------------


	//TODO-BEGIN MAIN4
		//Creamos los pipes de los proceosos llamando a la la función CrearPipes.
		//Si  la funcion no devuelve el numero de procesos mostramos error y salimos con -1
		if (numProcs!=CrearPipes(numProcs)){
			printf("ERROR en la creación de los pipes\n");
			exit(-1); 
		}
		//For-Debug-Only
		//PrintProcesos(numProcs);
	//TODO-END-------------------------

	//TODO-BEGIN MAIN5
		//Cargamos el fichero de dependencias
		//Llamamos a la función CargarDependencias para realizar esa tarea
		CargarDependencias(depFilename);
		//For-Debug-Only
		//PrintProcesos(numProcs);
	//TODO-END-------------------------

	//---***Bucle para crear a los procesos ***---
	for(i=0; i < numProcs; i++) { //El padre itera creando los procesos hijo
		//TODO-BEGIN MAIN6
			//llamamos a la función fork obteneiendo en la variable pid el valor retornado
			pid=fork();
		//TODO-END

		//TODO-BEGIN MAIN7
			//Si ha fallado la llamada a fork por cualquier causa mostramos mensaje y salimos con -1
			if(pid==-1){ 
				sprintf(buffer,"ERROR. Fork ha fallado al crear al proceso %d\n",i);
				perror(buffer);
				exit(-1);
			}
		//TODO-END-------------------------


		//TODO-BEGIN MAIN8
			//Sustituir ... por su codigo correcto
			//Completar los TODO que se piden
			   //...
			if(pid == 0){ //... // Código hijo Código hijo Código hijo Código hijo Código hijo 
				//TODO-BEGIN MAIN8.1
					//El proceso cierra su pipe de escritura
					close(procesos[i].p[1]);
				//TODO-END-------------------------
				
				//TODO-BEGIN MAIN8.2
					//El proceso lee la carga de trabajo de su pipe. Usar la función LeerPipe
					carga=LeerPipe(procesos[i].p[0]);; 	
				//TODO-END-------------------------

				//TODO-BEGIN MAIN8.3
					//El proceso lee los signals de su pipe. Usar la función LeerPipe
					numSignals=LeerPipe(procesos[i].p[0]);; 	
				//TODO-END-------------------------

				//TODO-BEGIN MAIN8.4
					//El procseo lee los waits e su pipe. Usar la función LeerPipe
					numWaits=LeerPipe(procesos[i].p[0]);; 	
				//TODO-END-------------------------


				int ixProc;
				//TODO-BEGIN MAIN8.5 
					//Si numWaits>0 se leerán tantos valores del pipe como indique numWaits.  Usar la función LeerPipe
					//Si numWaits<=0 no se leen ya mas valores del pipe
					//En el bucle de lectura
						//Cada valor leido sera el índice del proceso en el que hacer wait
						//Se realiza el wait sobre el semaforo con el índice leido
					if (numWaits>0){
						for (int w=0; w<numWaits; w++){ //para los numWaits
								ixProc=LeerPipe(procesos[i].p[0]);; 	
								//Hacemos wait sobre el semaforo indicado
								sWait(sProcs[ixProc]);
						}
					}
				//TODO-END-------------------------

				//Cuando el hijo llega aqui es porque ha salido de todos los waits que le preceden
				//Y por tanto puede ejecutar su tarea
				DoJob(carga);

				//Una vez realizado su trabajo el proceso libera a los procesos que estan esperando por el.
				//Para ello hace tantos signals sobre su semaforo como se le ha indicado
				//TODO-BEGIN MAIN8.6
					//Lanzar todos los signals sobre el semaforo del proceso actual
					for (int s=0; s<numSignals; s++){
						sSignal(sProcs[i]);
					}
				//TODO-END-------------------------

				exit(0); //El hijo termina, no itera.
			}
			else{ //Código padre Código padre Código padre Código padre Código padre
				//TODO-BEGIN MAIN8.7
					//El padre cierra el pipe de lectura y 
					//asigna el pid del hijo en su posición del array de procesos
					close(procesos[i].p[0]);
					procesos[i].pid=pid;
				//TODO-END-------------------------
			}
		//TODO-END-------------------------
	}

	// Aquí sólo llega el padre
	//For-Debug-Only
	//PrintProcesos(numProcs);

	// El padre entra en bucle para enviar datos por el pipe de cada proceso. Los datos que envía son:
	//    la carga, el numero de signals, el numero de waits y los procesos a los que espera (waitfor ver estructura PROCESOS)
	//    para ello llamamos a la funcion EscribirPipe 
	//    Pasar parametros correctos, ver definicion en func.h
	for (int p=0; p<numProcs; p++){
			carga = procesos[p].carga; 
			numSignals = procesos[p].signals;
			numWaits = procesos[p].waits;
			//TODO-BEGIN MAIN9
				//Escribimos la carga en el pipe del proceso en curso
				EscribirPipe(procesos[p].p[1],carga);

				//Escribimos el numero de signals en el pipe del proceso en curso
				EscribirPipe(procesos[p].p[1],numSignals);

				//Escribimos el numero de waits en el pipe del proceso en curso
				EscribirPipe(procesos[p].p[1],numWaits);

				//En bucle escribimos el indice de los procesos en waitfor si los hay (numWaits indica cuantos)
				for (int j=0; j<numWaits; j++){
					EscribirPipe(procesos[p].p[1],procesos[p].waitfor[j]);
				} 

			//TODO-END-------------------------
	}

	//TODO-BEGIN MAIN10
		//Codificar el bucle para esperar la terminación de los hijos
		//usando la funcion wait o la función waitpid
		for(i=0; i < numProcs; i++){
			pid=waitpid(procesos[i].pid,&status,0); //Esperamos la terminación de cualquier hijo recogiendo el pid del mismo.
		}
	//TODO-END-------------------------

	//Capturar el tiempo total y mostrarlo por pantalla.
	clock_gettime(CLOCK_REALTIME, &end);
	long seconds = end.tv_sec - begin.tv_sec;
	long nanoseconds = end.tv_nsec - begin.tv_nsec;
	double elapsed = seconds + nanoseconds*1e-9;
	printf("CPU Time measured: %.3f seconds.\n", elapsed);

	return 0;
}