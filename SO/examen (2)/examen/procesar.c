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

#define MIN_BESTOF 3
#define MAX_BESTOF 9
#define NUM_PLAYERS 2

#define CMD_END 0
#define CMD_REAPEAT 1
#define CMD_BET 2

#define CHILD_A 0
#define CHILD_B 1


//Programa principal
int main( int argc, char *argv[] )
{

	//Declaramos las variables necesarias para la memoria compartida del Buffers de memoria MatchResults
	int shmIdMatchResults, shmMatchResultsSize; //Para el Shared Memory MatchResults
	int *shmMatchResults;  
	int shmIx; //para mantener el índice donde insertar en el array de memoria compartida (para el hijo)
	int ixA, ixB; //Para los indices de acceso al buffer compartido

	//Declaramos el semaforo Sincro de sincronización entre Padre e Hijos
	int sSincro;


	int pid; //Para el pid del fork
	int playerA, playerB; //Para almacenar los jugadores que se reciben como parametro
	int bestOf; //Para almacenar el valor del parametro bestOf
	int bet; //para la apuesta del hijo
	int contadorA, contadorB; //Para el conteo de veces que gana cada hijo
	int betA, betB; //Para la apuesta de cada hijo
	int numBets; // Numero de apuestas validas que van haciendo los hios

	int i,j; //Iteradores
	int cmd; //Para el codigo de comando a escribir en el pipe

	int status;           //Para status de retorno de la llamada al sistema wait
	char buffer[200]="";  //Buffer genérico para llamadas al sistema write/read
 	FILE * fd;  //Descriptor de fichero para usarlo para fichero de resultados
	char resultsfile[100]="";  //Para el nombre del fichero de resultados


	//TODO-BEGIN------------------------- MAIN 1
		//Control de paraḿetros y asignación a variables
		//Si error mostramos mensaje, sintaxis y terminamos con -1

		//Controlamos el numero de parametros.
		if(argc != 4){
			printf("ERROR: Error en el numero de parametros [%d], se esperaban %d.\n",argc-1,3);
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

		//Controlamos si el tercer parámetro es numérico 
		if (!isNumber(argv[3])){
			printf("ERROR: Error el tercer argumento debe ser numerico\n",argc);
			Sintaxis();
			exit(-1);
		}

		//Capturamos los datos  en las variables correspondientes según
		// playerA <-- argv[1]
		// playerB  <-- argv[2]
		// bestOf <-- argv[3]
		playerA=atoi(argv[1]);
		playerB=atoi(argv[2]);
		bestOf=atoi(argv[3]);

		//Componemos el nombre del fichero de resultados en la variable resultsfile
		sprintf(resultsfile, "%dvs%d.dat", playerA, playerB);
		//printf("El fichero de resultados sera: %s\n",resultsfile);

		//Conrolamos que bestOf está entre 3 y 9
		if (bestOf<MIN_BESTOF || bestOf>MAX_BESTOF){
			printf("ERROR: Error argumento bestOf debe estar en el rango [%d:%d]\n",MIN_BESTOF,MAX_BESTOF);
			Sintaxis();
			exit(-1);
		}

	//TODO-END-------------------------



	//TODO-BEGIN------------------------- MAIN 2
		//Creamos el array de estrucutras hijos llamando a NewArrayHijos
		//Esta funcion crea el array hijos, declarado globalmente en func.h
		//Si la función no devuelve el numero de hijos mostramos error y salimos con -1
		//El número de hijos a crear es 2 (NUM_PLAYERS)
		if (NUM_PLAYERS!=NewArrayHijos(NUM_PLAYERS)){
			printf("ERROR en la creación del array de hijos\n");
			exit(-1); 
		}
	//TODO-END-------------------------


	//TODO-BEGIN------------------------- MAIN 3
		//Creamos los pipes de los hijos llamando a CrearPipes.
		//Esta función recibe el número de hijos (NUM_PLAYERS).
		//Si  la funcion no devuelve el numero de hijos mostramos error y salimos con -1
		//printf("El padre crea los pipes para los hijos\n");
		if (NUM_PLAYERS!=CrearPipes(NUM_PLAYERS)){
			printf("ERROR en la creación de los pipes\n");
			exit(-1); 
		}
	//TODO-END-------------------------


	//TODO-BEGIN------------------------- MAIN 4
		//Definimos el tamaño, creamos el segmento de memoria compartida shmMatchResults y asignamos correctamente los punteros.
		//Si Error mostramos mensaje y salimos
		shmMatchResultsSize=bestOf*NUM_PLAYERS*sizeof(int); // Cada jugador tiene bestOf integers en el buffer
		shmIdMatchResults = shmget(ftok("./", SHMSEEDBUFFER), shmMatchResultsSize, IPC_CREAT | SHMPERMISOS);
		if (shmIdMatchResults==-1){
			printf("Error al crear el segmento MatchResults\n");
			exit(1);
		}
		//Apuntamos shmWorkResults a la memoria compartida reservada 
		shmMatchResults = shmat(shmIdMatchResults,0,0);
	//TODO-END-------------------------

	

	// El padre inicializa el shmMatchResults con ceros
	for (int i=0; i<bestOf*NUM_PLAYERS; i++){
		shmMatchResults[i]=0;
	}


	//TODO-BEGIN------------------------- MAIN 5
		//Creamos el semáforo sSincro de sincronización entre padre e hijos
		sSincro=sCreate(SSEED,0);  
	//TODO-END-------------------------


	//---************ Bucle para crear a los hijos ************---
	//El padre crea en bucle a los NUM_PLAYERS. 
	//TODO-BEGIN------------------------- MAIN 6
		for(i=0; i < NUM_PLAYERS; i++) { //El padre itera creando los jugadores
			//Nota: La variable i es el índice del jugador

			//llamamos a la función fork obteniendo en la variable pid el valor retornado
			pid=fork();


			//Si no se ha podido crear al jugadore mostramos error y salimos con -1
			if(pid==-1){
				sprintf(buffer,"ERROR. Fork ha fallado al crear al hijo %d\n",i);
				perror(buffer);
				exit(-1);
			}

			//Determinamos si el pid es de un hijo, en cuyo caso actuamos
			if(pid == 0){ // Código del hijo
	
				//Inicializamos el generador de numeros aleatorios con el pid del hijo
				srand(getpid());


				//Con el iterador del bucle de creación sabemos en que indice empieza el hijo, en el 0 o en el 1
				shmIx=i; 

				//El hijo hace la primera apuesta y señaliza al padre
					//El hijo apuesta. Obtiene un numero aleatrorio entre 0 y 2 incluidos
					bet=RandInt(0,2);
	
					//El hijo guarda su apuesta en la memoria compartida en el indice que le corresponde
					shmMatchResults[shmIx]=bet;

					//El hijo, señalizando el semaforo, indica que ya ha apostado
					sSignal(sSincro);


				//El hijo entra en bucle sin fin a apostar, hasta que lea el comando CMD_END del pipe
				while (true){
					//El hijo lee un comando de su pipe
					cmd=LeerPipe(hijos[i].p[0]);

					//Si el comando es CMD_END el hijo ya no apuesta mas ni se señaliza al padre.
					if (cmd==CMD_END){
						break;
					}

					//si el comando es CMD_REPEAT el hijo vuelve a apostar y a dejar en el mismo indice del array de memoria compartida
					if (cmd==CMD_REAPEAT){
						//El hijo apuesta de nuevo. Obtiene un numero aleatorio entre 0 y 2
						bet=RandInt(0,2);

						//El hijo guarda su apuesta en la memoria compartida en el indice que le corresponde
						shmMatchResults[shmIx]=bet;
					}					

					//si el comando es CMD_BET es una nueva apuesta, el hijo incrementa el indice, apuesta y guarda su apuesta en el nuevo indice
					if (cmd==CMD_BET){
						//El hijo incrementa su indice de acceso al shmMatchResults
						shmIx=shmIx+2;

						//El hijo apuesta
						bet=RandInt(0,2);

						//El hijo guarda su apuesta en el indice que le corresponde
						shmMatchResults[shmIx]=bet;
					}					

					//El hijo, señalizando el semaforo, indica que ya ha apostado
					sSignal(sSincro);
				}

				//El hijo (jugador) no itera creagndo hijos.
				exit(0);
			}
		} // end for de creación hijos
	//TODO-END-------------------------

	// Aquí sólo llega el padre, los hijos hacen exit.

	//El padre inicialize los indices de los que tiene que leer lo de cada hijo.
	ixA=0;
	ixB=1;	

	//El padre inicializa el contadore de apuestas validas 
	numBets=0;

	//El padre inicializa los contadores de manos ganadas
	contadorA=0;
	contadorB=0; 


	//TODO-BEGIN------------------------- MAIN 7
		//El padre abre el fichero de resultados para escritura
		fd = fopen (resultsfile, "w");

		//El padre escribe la primera linea, playerA, playerB, contadorA, ContadorB, 
		fprintf(fd, "%d, %d, %d, %d,\n", playerA, playerB, contadorA, contadorB);			
	//TODO-END-------------------------


	//TODO-BEGIN------------------------- MAIN 8
	//El padre entra en bucle hasta que el numero de apuestas validas sea el valor de bestOf
	while(numBets < bestOf){
		// Espera a que todos los hijos le señalicen que ya han apostado usando el semaforo
		for (j=0; j<NUM_PLAYERS; j++){
			sWait(sSincro);
		}
	
		//El padre obtiene en las varibles betA y betB las apuestas de los hijos, que las obtiene de los indices correspondientes a la apuesta y a cada hijo (ixA e ixB) 
		betA = shmMatchResults[ixA];
		betB = shmMatchResults[ixB];

		//El padre comprueba lo que han apostado los hijos, determina el ganador y decide el cmd que tienee que enviar por el pipe.
		//Si las apuestas son iguales el cmd es CMD_REPEAT
		if (betA==betB){
			//Asigna el comando correspondiente cuando las apuestas son iguales
			cmd=CMD_REAPEAT;
		}
		else { 
			//Se incrementa el contador de apuestas validas (numBets) y el comando se pone a CMD_BET 
			numBets=numBets+1;
			cmd = CMD_BET;

			//El padre incrementa apropiadamente los indices de lectura para la siguiente mano.
			ixA=ixA+2;
			ixB=ixB+2;

			//El padre comprueba quien ha ganado esta mano incrementandosu contador
			//utiliza las variables betA y betB para ver que ha apostado cada uno.
			//En función de quien gana incrementa contadorA o contadorB
			//Segun esta tabla:
			//  PIEDRA gana vs TIJERA
			//  PAPEL gana vs PIEDRA
			//  TIJERA gana vs PAPEL
			if (betA == PIEDRA && betB == PAPEL){
				contadorB++;
			}
			else if (betA == PIEDRA && betB == TIJERA){
				contadorA++;
			}
			else if (betA == PAPEL && betB == PIEDRA){
				contadorA++;
			}
			else if (betA == PAPEL && betB == TIJERA){
				contadorB++;
			}
			else if (betA == TIJERA && betB == PIEDRA){
				contadorB++;
			}
			else if (betA == TIJERA && betB == PAPEL){
				contadorA++;
			}
			//debug printf("La mano deja contadores a %d vs %d\n",contadorA, contadorB); fflush(stdout);


			//El padre escribe la apuesta y su resultado en el fichero de resultados, betA, betB, contadorA, contadorB
			fprintf(fd, "%c, %c, %d, %d,\n", GetCharBet(betA), GetCharBet(betB), contadorA, contadorB);			
		}

		//Si esta apuesta (numBets) ya es la que iguala a bestOf entonces el comando es CMD_END, no CMD_REPEAT 
		if (numBets == bestOf){
			cmd = CMD_END;
			//debug printf("El padre pone cmd=%d (END)) llevamos %d apuestas validas\n",cmd,numBets); fflush(stdout);
		}

		//Se envia el comando cmd por el pipe a los dos hijos.
		for (i=0; i<NUM_PLAYERS; i++){
			EscribirPipe(hijos[i].p[1], cmd);
		}
	}
	//TODO-END-------------------------



	//TODO-BEGIN------------------------- MAIN 9
		//Se ha terminado la partida el ganador es el player con mayor contador.
		//El padre escribe el ganador en el fichero de resultados y lo cierra
		if (contadorA>contadorB){
			fprintf(fd, "%d", playerA);			
		}
		else{		
			fprintf(fd, "%d", playerB);			
		}
		fclose(fd);
	//TODO-END-------------------------


	//El padre espera a que terminen los hijos.
	//TODO-BEGIN------------------------- MAIN 10
		//El padre espera la terminación de los hijos
		//Codificar el bucle para esperar la terminación de los Hijos
		//usando la funcion wait o la función waitpid
		for(i=0; i < NUM_PLAYERS; i++){
			pid=wait(&status); //Esperamos la terminación de cualquier hijo
			//printf("El padre recibe la terminacion del Launcher [%d]\n",pidE);  fflush(stdout);
		}
	//TODO-END-------------------------

	//TODO-BEGIN------------------------- MAIN 11
		//Proceso de cierre.
		//El padre se desvincula del buffer
		shmdt(shmMatchResults);
		 
		//El padre borra el buffer
		shmctl(shmIdMatchResults, IPC_RMID, 0);

		//El padre destruye el semaforo
		sDestroy(sSincro);
 
		//TODO-END-------------------------

	// El padre termina.
	return 0;

}