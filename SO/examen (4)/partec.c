#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
//#include "sem.h"
#include "functions.h"

typedef struct { 
    int pid ; // Pid del hijo 
	int pipehijo[2]; // Descriptores el pipe del hijo
	int ceros; //ceros contados por el hijo
	int pares; //pares no nulos contados por el hijo
	int impares; //impares no nulos contados por el hijo
} HIJO;

int main( int argc, char **argv ){
	int i,j,r,c,pid,key,wstatus,dim,rows,cols,numChilds,matrixSize;
	int blockSize;
	int ceros=0,pares=0,impares=0;

	int numBlocks;
	int condHijosMod16, condDivHijos;
	char filename[50];
	 
	char buffer[100];
	int shmId, shmSize;
	int *shmMatrix; //unrolled matrix in shared memory

	
	//Inicializamos el generador de números aleatorios
	srand(getpid());   // Initialization, should only be called once.

	//Obtenemos los parámetros
	numChilds=atoi(argv[1]);
	dim=atoi(argv[2]);

	// La matriz es cuadrada por lo que el numero de filas y columnas es el indidcado en el parametro dim
	rows=dim;
	cols=dim;  


	//Realizamos al comprobación de parametros con la función CheckParameters
	//Si los parametros no son correctos llamamamos a PrintCorrectChilds para mostrar al usuarios el 
	//numero correcto de hijos que se pueden usar para esa matrix de dim x dim
	if ( !CheckParameters(numChilds,rows) ) {
		printf("Tamaño de bloque no homogéneo o matrix demasiado grande.\n");
		//Mostramos los numeros de hijos que se pueden usar para una matriz de tamaño dim x dim
		PrintCorrectChilds(rows);
		exit(1);
	}

	//Tamaño de bloque
	blockSize = (int) BlockSize(rows,cols,numChilds);

	//BEGIN TODO *************************************************************
	// Reservar de Memoria compartida para una matriz de enteros de rows x cols
	// Vincularse al segmento obtenido
	// Gestionar los errores.
	if ((shmId = shmget(ftok(".", SHMSEED), rows*cols*sizeof(int), IPC_CREAT | SHMPERMISOS))==-1){
		perror("Error en la reserva de memoria compartida.");
		exit(1);
	}
	shmMatrix = shmat(shmId, 0, 0);
	if (shmMatrix == (int*)(-1)) {
        perror("shmat");	
        exit(1);
    }
	//END TODO *************************************************************

    //LLamamos a la funcíon InitMatrix para inicializar el segmento de memoria compartida de la matriz.
 	//printf("El padre inicializa el array de memoria compartida con id: %d\n",shmId);
    InitMatrix(shmMatrix, rows, cols);

	//BEGIN FOR DEBUG ------------------------------------------------------
		//Mostramos la Matriz en pantalla (tener en cuenta que si es muy grande no se ve bien, probar con 10x10)
			//PrintMatrix(shmMatrix,rows,cols);

		// Mostramos el contenido de los datos (bloques de la matriz) para cada hijo
		// Los hijos comienzan por 0. Una matriz con 4 bloques (uno para cada hijo) serían los hijos 0,1,2,3
			//for (i=0;i<numChilds;i++){
			//	PrintBlockData(i, shmMatrix, rows, cols, numChilds);
			//}
	//END FOR DEBUG ------------------------------------------------------

	//BEGIN TODO *************************************************************
	// El padre crea un array de estructuras HIJO 
	HIJO hijos[numChilds]; // Declaramos el vector de estructuras HIJO
	//END TODO *************************************************************
	 
	//BEGIN TODO *************************************************************
	//Inicializamos el array de estructuras HIJO, con valores por defecto todo a cero excepto el pipe
	//puesto que creamos un pipe para cada hijo 
	for(i=0; i<numChilds;i++){ 
	  hijos[i].pid=0;
	  pipe(hijos[i].pipehijo);
	  hijos[i].ceros=0;
	  hijos[i].pares=0;
	  hijos[i].impares=0;
	}
	//END TODO *************************************************************

	//BEGIN TODO *************************************************************
    //Bucle para crear a los hijos
    //Controlar los errores
    //Se crean los hijos sin esperar a que terminen para crear el siguiente.
    //El código del hijo:
    	//Cierra el lado del pipe que no use
		//Crea un array de enteros donde obtendrá su parte de la matriz de memoria compartida
		//Obtiene su parte de matriz compartida con la función GetBlock.
		//LLama a las funciones ContarCeros, ContarParesNoCeros,ContarImpares y ContarDecena para obtener sus datos
		//Una vez obtenidos los envía (write) al padre mediante el pipe.
		//El hijo termina
    //El código del padre (dentro del bucle for de creación de los hijos)
    	//El padre actualiza el pid del hijo recien creado en su estructura dentro del array de HIJOS.
		//El padre cierra el lado del pipe que no usa.

	for(i=0; i < numChilds; i++)	{ //El padre itera creando hijos
		pid=fork();
		if(pid == -1){   
		   sprintf(buffer,"ERROR. Fork ha fallado al crear al hijo %d\n",i);
		   perror(buffer);
		   exit(-1);
		}
		if(pid == 0){
			//Código hijo
			//El hijo cierra el lado de lectura de su pipe
			close(hijos[i].pipehijo[0]); 

			//El hijo define un bloque de enteeros de tamaño blockSize
			//y obtiene el bloque que le corresponde de shmMatrix utilizando la funcion GetBlock
			int block[blockSize];
			GetBlock(shmMatrix,i,rows,cols,numChilds,rows,&block[0]);
			//printf("Child %d (%d) data: ",i,getpid());
			//PrintBlock(block,blockSize);

		 	//El hijo obtiene los datos que le piden (ceros, pares, impares, decena1, decena2 ... )
		 	ceros =  ContarCeros(block, blockSize);
		 	pares = ContarParesNoCeros(block, blockSize);
		 	impares = ContarImpares(block, blockSize);

		   //El hijo envía sus datos al padre mediante su pipe
			write( hijos[i].pipehijo[1], &ceros, sizeof(int));
			write( hijos[i].pipehijo[1], &pares, sizeof(int));
			write( hijos[i].pipehijo[1], &impares, sizeof(int));

		   //El hijo termina

		   //printf("El hijo %d termina.\n",getpid());
		   exit(0); //El hijo termina, no itera.
		}
		else{
			//El padre actualiza el pid del hijo en su estructura
			hijos[i].pid=pid;

			//El padre ciera el lado de escritura del pipe del hijo
			close(hijos[i].pipehijo[1]); 
		}
	}
	//END TODO *************************************************************


 	// Aquí sólo llega el padre    

	//BEGIN TODO *************************************************************
    //Bucle de espera a la terminación de los hijos
 		//El padre espera la terminación de un hijo
 		//Cuando el hijo termina lee sus datos del pipe y los actualiza en su estructura HIJO
 	int ixChild; 
	for(i=0; i < numChilds; i++)	{
		pid=wait(&wstatus); //Esperamos la terminación de cualquier hijo recogiendo el pid del mismo.
		//Buscamos cual es el pipe del hijo
		for (j=0; j<numChilds;j++){
			if (hijos[j].pid==pid){
				ixChild=j;
				break;
			}
		}
		//El padre lee del pipe del hijo que ha terminado y actualiza los datos globales
		read(hijos[ixChild].pipehijo[0], &(hijos[ixChild].ceros), sizeof(int));
		read(hijos[ixChild].pipehijo[0], &(hijos[ixChild].pares), sizeof(int));
		read(hijos[ixChild].pipehijo[0], &(hijos[ixChild].impares), sizeof(int));
	}
	//END TODO *************************************************************


	//BEGIN TODO *************************************************************
	//Han terminado todos los hijos el padre agrega los datos de cada HIJO sumandolos en las variables
	//ceros, pares, impares y decenas que están definidas al comienzo del código.
	for (i=0; i<numChilds; i++){
		ceros+=hijos[i].ceros;
		pares+=hijos[i].pares;
		impares+=hijos[i].impares;
	}
	//END TODO *************************************************************

	//Si han terminado todos los hijos el padre genera el informe de resultados
	SaveData(ceros,pares,impares);

	//BEGIN TODO *************************************************************
	// El padre se desvincula del segmento de memoria compartida (controlar posibles errores)
    if (shmdt(shmMatrix) == -1) {
        perror("shmdt");
        exit(1);
    }
	//END TODO *************************************************************


	//BEGIN TODO *************************************************************
    //El padre borra el segmento de memoria compartida.
    shmctl(shmId, IPC_RMID, NULL);
	//END TODO *************************************************************

    //El padre termina
	return 0;
} 
