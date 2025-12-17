#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>   
#include <stdbool.h>
#include <sys/wait.h>
#include "func.h"
#include "sem.h"

#define SSEED 99           //Semilla para Semáforos
#define SHMSEEDBUFFER 35   //Semilla para Buffer1
#define SHMPERMISOS 0644   //Permisos para shared Memory Segments

//#define NUM_LAUNCHERS 3    //Numero de hijos que lanzan Workers.
#define MIN_LAUNCHERS 1
#define MAX_LAUNCHERS 5
#define MIN_WORKERS 1      //Numero minimo de Workers por Launcher
#define MAX_WORKERS 20     //Numero maximo de Workers por Launcher
#define MIN_WORKLOAD 1     //Nuemro minimo de segundos de carga de trabajo
#define MAX_WORKLOAD 5     //Nuemro minimo de segundos de carga de trabajo

#define H1 0    //constante para H1  
#define H2 1    //constante para H2

//Programa principal
int main( int argc, char *argv[] )
{

	//Declaramos las variables necesarias para la memoria compartida del Buffers de memoria  WorkResults.
	int shmIdWorkResults, shmWorkResultsSize; //Para el Shared Memory WorkResults
	int *shmWorkResults;  

 	//Declaramos el semaforo mutex de acceso al shmWorkResults
	int sMutex;
 
 	//int maxWorkers;  //Máximo numero de workers por hijo

	int pidP, pidL, pidW; //Para los pids, pidP para pid el padre, pidL usarlo para Launchers, pidW usarlo para Workers
	int i,j;          //iterador bucle creación hijos
	int numLaunchers; //Para el numero de launchers
	int numWorkers; //Para el número de workers de un launcher
	int workLoad;   //Para la carga de un Worker
	int workResult; //Para el resultado del DoWork()

	int status;           //Para status de retorno de la llamada al sistema wait
	char buffer[200]="";  //Buffer genérico para llamadas al sistema write/read
 	FILE * fd;  //Descriptor de fichero para usarlo para fichero de resultados
	char resultsfile[100]="";  //Para el nombre del fichero de resultados



	// int numDatos;    //Numero de datos. 
 	// int numHijos=2;  //Numero de hijos a crear.
 	// int filtro1, filtro2, filtro; //Para las variables filtro
 	// int dato;   //Para el valor del dato leido/a escribir el pipe y del buffer

	// char datafile[100]="";     //Para el nombre del fichero de datos


	//TODO-BEGIN------------------------- MAIN1
		//Control de paraḿetros y asignación a variables
		//Si error mostramos mensaje, sintaxis y terminamos con -1

		//Controlamos el numero de parametros.
		if(argc != 5){
			printf("ERROR: Error en el numero de parametros [%d], se esperaban %d.\n",argc-1,4);
			Sintaxis();
			exit(-1);
		}

		//Controlamos si el primer parámetro es numérico 
		if (!isNumber(argv[1])){
			printf("ERROR: Error el primer argumento debe ser numerico\n",argc);
			Sintaxis();
			exit(-1);
		}

		//Controlamos si el segundo parámetro es numérico 
		if (!isNumber(argv[2])){
			printf("ERROR: Error el segundo argumento debe ser numerico\n",argc);
			Sintaxis();
			exit(-1);
		}

		//Controlamos si el tercer parámetro es string 
		if (!isNumber(argv[3])){
			printf("ERROR: Error el tercer argumento debe ser numerico\n",argc);
			Sintaxis();
			exit(-1);
		}

		//Capturamos los datos  en las variables correspondientes según
		// numLaunchers <-- argv[1]
		// maxWorkers  <-- argv[2]
		// maxWorkLoad <-- argv[3]
		numLaunchers=atoi(argv[1]);
		numWorkers=atoi(argv[2]);
		workLoad=atoi(argv[3]);
		strcpy(resultsfile, argv[4]);

		//Conrolamos maxLaunchers está entre MIN_LAUNCHERS y MAX_LAUNCHERS
		if (numLaunchers<MIN_LAUNCHERS || numLaunchers>MAX_LAUNCHERS){
			printf("ERROR: Error argumento numLaunchers debe estar en el rango [%d:%d]\n",MIN_LAUNCHERS,MAX_LAUNCHERS);
			Sintaxis();
			exit(-1);
		}

		//Conrolamos maxWorkers está entre MIN_WORKERS y MAX_WORKERS
		if (numWorkers<MIN_WORKERS || numWorkers>MAX_WORKERS){
			printf("ERROR: Error argumento maxWorkers debe estar en el rango [%d:%d]\n",MIN_WORKERS,MAX_WORKERS);
			Sintaxis();
			exit(-1);
		}

		//Conrolamos que workLoad está entre MIN_WORKLOAD y MAX_WORKLOAD
		if (workLoad<MIN_WORKLOAD || workLoad>MAX_WORKLOAD){
			printf("ERROR: Error argumento workLoad debe estar en el rango [%d:%d]\n",MIN_WORKLOAD,MAX_WORKLOAD);
			Sintaxis();
			exit(-1);
		}

		//Comprobamos si el fichero de resultados existe. Si existe mostramos error, debería haberse borrado.
		if (CheckIfFileExists(resultsfile)){
			printf("ERROR: El fichero [%s] existe.\n",resultsfile);
			Sintaxis();
			exit(-1);
		}
	//TODO-END-------------------------

	//TODO-BEGIN------------------------- MAIN2
		//Definimos el tamaño, creamos el segmento de memoria compartida shmWorkResults y asignamos correctamente los punteros.
		//Si Error mostramos mensaje y salimos
		shmWorkResultsSize=numLaunchers*sizeof(int);
		//debug printf("Tamaño del segmento a pedir: %d\n",shmWorkResultsSize); fflush(stdout);
		shmIdWorkResults = shmget(ftok("./", SHMSEEDBUFFER), shmWorkResultsSize, IPC_CREAT | SHMPERMISOS);
		if (shmIdWorkResults==-1){
			printf("Error al crear el segmento WorkLoad\n");
			exit(1);
		}
		//Apuntamos shmWorkResults a la memoria compartida reservada 
		shmWorkResults = shmat(shmIdWorkResults,0,0);
	//TODO-END-------------------------

	// El padre inicializa el shmWorkResults con ceros
	for (int i=0; i<numLaunchers; i++){
		shmWorkResults[i]=0;
	}
	//debug printf("El padre ha inicializado el shmWorkResults de memoria compartida %d con [%d][%d][%d]\n", shmIdWorkResults, shmWorkResults[0], shmWorkResults[1], shmWorkResults[2]); fflush(stdout);


	//TODO-BEGIN------------------------- MAIN3
		//Creamos el semáforo mutex de acceso al buffer de resultados inicializado a sus valor correcto
		sMutex=sCreate(SSEED,1);  //Inicializar para que el primer proceso que llegue entra
	//TODO-END-------------------------

	//Antes de comenzar a crear launchers, guardamos el pid del padre en pidP.
	pidP=getpid();

	//---************ Bucle para crear a los Launchers ************---
	//El padre crea en bucle a los NUM_LAUNCHERS. 
	//Cada Launcher crea un número aleatorio de Workers entre 1 y maxWorkers
	//Cada Worker llama a DoWork() y actualiza el valor en shmWorkResults correspondiente a su Launcher.
	//debug printf("El padre entra en el bucle de cración de los %d Launchers\n",NUM_LAUNCHERS); fflush(stdout);
	//TODO-BEGIN------------------------- MAIN4
		for(i=0; i < numLaunchers; i++) { //El padre itera creando los launchers
			//Nota: La variable i es el índice del launcher

			//llamamos a la función fork obteniendo en la variable pid el valor retornado
			pidL=fork();

			//Si no se ha podido crear al launcher mostramos error y salimos con -1
			if(pidL==-1){
				sprintf(buffer,"ERROR. Fork ha fallado al crear al launcher %d\n",i);
				perror(buffer);
				exit(-1);
			}

			//Determinamos si el pid es de un launcher, en cuyo caso actuamos, o del padre
			if(pidL == 0){ // Código launcher 
				//debug printf("  Launcher [%d] el padre me ha creado con pid %d\n",i,getpid()); fflush(stdout);

				//---************ Bucle para crear a los Workers ************---
				//El Launcher crea en bucle a sus numWorkers Workers con fork(). Nota: no usar i como iterador del bucle, usar j
				for (j=0; j<numWorkers; j++){

					//llamamos a la función fork obteniendo en la variable pid el valor retornado
					pidW=fork();

					//Si no se ha podido crear al worker mostramos error y salimos con -1
					if(pidW==-1){
						sprintf(buffer,"ERROR. Fork ha fallado al crear al worker %d para el launcher %d\n",j,i);
						perror(buffer);
						exit(-1);
					}

					//Determinamos por el valor de pidW si es Launcher o Worker
					if (pidW==0){
						
						//Inicializamos el generador de numeros aleatorios
						srand (getpid());

						//printf("    Worker [%d][%d] el launcher me ha creado con pid %d\n",i,j,getpid()); fflush(stdout);

						//El worker ejectura su carga de trabajo llamando a DoWork(), pasándole su workLoad
						//Recibe comor resultado de DoWork() el dato workResult, que lo suma al índice de su Launcher en shmWorkResults
						//Para acceder al shWorkResults debe hacerlo en exclusión mutua usando sMutex. 
						workResult=DoWork(workLoad);

						//debug printf("    Worker [%d][%d] obtiene valor %d\n",i,j,workResult); fflush(stdout);
						sWait(sMutex);
							shmWorkResults[i] = shmWorkResults[i] + workResult;
						sSignal(sMutex);

						//El Worker termina
						//printf("  Worker %d del Launcher %d termina.\n",j,i); fflush(stdout);
						exit(0);
					}

				}//end de la creacion de workers;
				//Soy Launcher y he credo a mis workers, no sigo iterando por el bucle de creación de Launchers que he heredado de mi padre
				//debug printf("  Launcher %d sale del bucle.\n",i); fflush(stdout);
				break; 
			}
		} // end for de creación launchers
	//TODO-END-------------------------

	// Aquí sólo llega el padre y los launchers, los workers hacen exit.
	// Usamos pidP para determinar si somos el padre o un launcher

	if (pidP==getpid()){
		//Soy el padre
		//El padre espera a que terminen los launchers.
		//TODO-BEGIN------------------------- MAIN5
			//El padre espera la terminación de los Launchers
			//Codificar el bucle para esperar la terminación de los Launchers
			//usando la funcion wait o la función waitpid
			//debug printf("El padre espera a la terminacion de los Launchers\n");  fflush(stdout);
			int pidE; 
			for(int k=0; k < numLaunchers; k++){
				pidE=wait(&status); //Esperamos la terminación de cualquier Worker
				//printf("El padre recibe la terminacion del Launcher [%d]\n",pidE);  fflush(stdout);
			}
			//debug printf("Los launchers han terminado\n");  fflush(stdout);
		//TODO-END-------------------------
	}
	else{
		//Soy un Launcher.
		//Los launcher sólo deben esperar a que sus Workers terminen.
		//Cuando todos sus Workers hayan terminado ellos terminan con exit(0);
		//Nota: en numWorkers tenemos el número de Workers de este Launcher.
		//TODO-BEGIN------------------------- MAIN6
			//El Launcher espera la terminación de sus Workers
			//Codificar el bucle para esperar la terminación de los Workers
			//usando la funcion wait o la función waitpid
			//debug printf("El Launcher %d espera la terminacion de sus workers\n",i);fflush(stdout);
			int pidE;
			for(int k=0; k < numWorkers; k++){
				pidE=wait(&status); //Esperamos la terminación de cualquier Worker
				//printf("El Launcher recibe la terminacion del Workder [%d]\n",pidE);  fflush(stdout);
			}
			//debug printf("El launcher %d termina\n",i);fflush(stdout);
			exit(0); // Termina el launcher.
		//TODO-END-------------------------
	}

	// Aquí sólo llega el padre cuando los launchers y los workers ya han terminado.
	// El padre recoje los datos de shmWorkResults y genera un fichero.
	// El fichero tiene que tener una línea por dato, total tres líneas.
	//TODO-BEGIN------------------------- MAIN7
		//El padre itera por el shmWorkResults para obtener los datos
		//Aquí ya no hace falta acceso en mutex puesto que los hijos ya han terminado.
		fd = fopen (resultsfile, "w");
		int dato; 
		for (int i=0; i<numLaunchers; i++){
			dato=shmWorkResults[i];
			//debug printf("dato[%d]=%d\n",i,dato);
			//Escribe el dato en el fichero
			fprintf(fd, "%d\n", dato);			
		}
		//Cierra el fichero de resultados
		fclose(fd);
	//TODO-END-------------------------	


	//TODO-BEGIN------------------------- MAIN8
		//Proceso de cierre.
		//El padre se desvincula del buffer
		//debug printf("El padre se desvincula del Buffer\n");
		shmdt(shmWorkResults);
		 
		//El padre borra el buffer
		//debug printf("El padre se destruye los Buffers\n");
		shmctl(shmIdWorkResults, IPC_RMID, 0);
 
		//El padre destruye el semáforo
		//debug printf("El padre se destruye el Semaforo\n");
		sDestroy(sMutex);
		//TODO-END-------------------------

	// El padre termina.
	return 0;

}