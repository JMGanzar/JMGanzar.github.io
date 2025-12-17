#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>   
#include <signal.h>   
#include <string.h>
#include <sys/ipc.h>   
#include <sys/shm.h>  
#include <stdbool.h>
#include <limits.h>
#include <sys/wait.h>
#include "func.h"

#define NUMHIJOS 4
#define NUMNIETOS 2
#define NUMNIETOSPORHIJO 4 
#define SSEED 99
#define SHMSEED 24 
#define SHMPERMISOS 0644

int hijosPipe[2];
int nietosPipe[2];

//***************************************************
//CODIGO HIJOS que crea a su vez a los nietos
// ixIni : Indice del pixel inicial del segmento compartido que le toca al procesar al hijo
// ixEnd : Indice del pixel final del segmeno compartido que le toca procesar al hijo
//***************************************************
void CodigoHijo(int ixIni, int ixEnd)
{
    //Begin NoTocar-------------------------------------
        //Código hijo
        int nieto; //Contador de nietos
        int i; //iterador generico
        int pidNieto,pidHijo; //pids de los procesos hijos y nietos
        int status; // Para recoger el valor de retorno de los hijos
	    char nombreArray[100];

        // Inicializamos arrayDatos
        // 0-pares 1-impares 2-positivos 3-negativos 4-maximo 5-minimo
        int arrayDatos[TAMDATOS]={0,0,0,0}; 

        // Inicializamos arrayCuentas: 
        // Este array es el que los nietos tienen que pasar a sus funciones para recoger las cuentas
        //0-{PARESIMPARES,POSITIVOSNEGATIVOS} 1-valor 2-valor
        int arrayCuentas[TAMCUENTAS]={0,0,0};

        //Guardamos el pid del hijo en curso
        pidHijo=getpid();
    //End NoTocar-------------------------------------

    //Ejemplos de como mostrar un array pasándole un nombre     
    /*
        sprintf(nombreArray,"Hijo(%d) arrayDatos",getpid());
        PrintArray(nombreArray,arrayDatos,TAMDATOS);

        sprintf(nombreArray,"Hijo(%d) arrayCuentas",getpid());
        PrintArray(nombreArray,arrayCuentas,TAMCUENTAS);
    */

    //Begin TODO *************************************
        //Creamos el pipe nietosPipe para comunicar con los nietos
        pipe(nietosPipe);

        //Creación de los dos nietos (en bucle o uno tras otro)
        //Comprobar si fork funciona correctamente
        //Los nietos procesan los pixels que le tocan al hijo en curso
            //El primer nieto debe llamar a la función  ContarPares(ixIni,ixEnd,arrayCuentas);
            //El segundo nieto debe llamar a la función ContarClaros(ixIni,ixEnd,arrayCuentas);
        //Cada nieto debe escribir su arrayCuentas en el pipe nietosPipe

        for (i=0;i<NUMNIETOS;i++) {
			pidNieto = fork();
			if (pidNieto==-1) {
				printf("Error: no se ha podido realizar fork\n");
				exit(-1);
			}
			if (pidNieto==0) {
                //Nieto
				if (i==0) {//como el primer nieto tiene que llamar a contarPares pues i=0 cada nieto tiene su array cuenta

					ContarPares(ixIni,ixEnd,arrayCuentas);
				} else {
					ContarClaros(ixIni,ixEnd,arrayCuentas);
				}
				sprintf(nombreArray,"Hijo(%d) arrayCuentas",getpid());
				PrintArray(nombreArray,arrayCuentas,TAMCUENTAS);
                //escribimos en nietos pipe y morimos con exit porque sino creamos más nietos de los nietos.
				EscribirArrayPipe(nietosPipe[1],arrayCuentas,TAMCUENTAS);
				exit(0);
			}
			
		}

    //End TODO ***************************************
    
    //Aquí solo llega el hijo, que espera la terminación de sus hijos (los nietos) para comunicar con el padre
    //Begin TODO *************************************
	    //Bucle para esperar la terminación de los nietos 
       for (i=0;i<NUMNIETOS;i++) {
		   waitpid(-1,&status,0);
	   } 

    //End TODO ***************************************
    
    //Begin NoTocar-------------------------------------
        //Han terminado todos los nietos
        //Componemos el arrayDatos de resultados con las cuentas de todos lo nietos para lo que leemos los arrays del nietosPipe
        // arrayDatos: 0-pares 1-impares 2-positivos 3-negativos 
        for (i=0; i<NUMNIETOS; i++){
            LeerArrayPipe(nietosPipe[0], arrayCuentas, TAMCUENTAS);
            switch(arrayCuentas[0]){
                case PARESIMPARES:
                    arrayDatos[0]=arrayCuentas[1];
                    arrayDatos[1]=arrayCuentas[2];
                break;
                case CLAROSOSCUROS:
                    arrayDatos[2]=arrayCuentas[1];
                    arrayDatos[3]=arrayCuentas[2];
                break;
            }
        }
    //End NoTocar-------------------------------------

	  sprintf(nombreArray,"Hijo(%d) arrayDatos",getpid());
        PrintArray(nombreArray,arrayDatos,TAMDATOS);

    //Begin TODO *************************************
        //Una vez compuesto el arrayDatos con los datos de los nietos, lo pasamos al padre mediante el pipe
        //Escribir en el hijosPipi el aray de datos arrayDatos   
        EscribirArrayPipe(hijosPipe[1], arrayDatos, TAMDATOS);


    //End TODO ***************************************

    
    //Begin TODO *************************************
	//El hijo se desvincula del array y termina
	
	 shmdt(shmArray);//para desvincular
	 exit(0);
    //End TODO ***************************************
}
 

int main( int args, char *argv[] )
{
    //Begin NoTocar-------------------------------------
	    int i; //iterador generico
        int pidHijo;  //para registrar el pid del hijo tras fork     
	    int status;   //para capturar el valor de retorno del hijo 
	    char strBuffer[100],nombreFicheroBMP[100]; //buffer string y string para nombre del fichero bmp
        int arrayDatos[TAMDATOS]; // Array para los datos que se capturan de los hijos via hijosPipe
        int shmId, shmSize;  // Identificador de la Shared Memory y Tamaño del segmento compartido en bytes
        int tamVector; // Tamaño en pixels (num enteros) del segmento compartido (de la imagen). 
        //parametros de la imagen
        int lado;
        int distancia; 


        //El control de argumentos se hace en Bash. 
        //Se capturan los parametros pasados por linea de comandos
        //Usar esas variables
        sprintf(nombreFicheroBMP,"%s.bmp",argv[1]); //Nombre del fichero BMP
        lado=atoi(argv[2]); // Lado de la imagen en pixels
        distancia=atoi(argv[3]); // Distancia de textura 

        tamVector=lado*lado; // Tamaño del vector (Numero de pixels de la imagen) (lado x lado) 
        carga=tamVector/NUMHIJOS; // Cantidad de pixels que tiene que procesar cada hijo
        intSize= sizeof(int); //Numero de bytes para un entero en esta arquitectura
    //End NoTocar-------------------------------------

    //Begin TODO *************************************
        //Creamos el pipe para comunicar con los hijos
       pipe(hijosPipe);

    //End TODO *************************************

    //Begin TODO *************************************
	    //Reserva de memoria para la imagen en bytes
	    shmSize = tamVector * intSize;
	    shmId = shmget(ftok("/bin/ls", SHMSEED), shmSize, IPC_CREAT | SHMPERMISOS);
		
	    //Creamos el segmento de memoria commpartida y lo asignamos a shmArray
	    shmArray = shmat(shmId,0,0);

    //End TODO *************************************
	
    //Begin TODO *************************************
        //Inicializaos el array de memoria compartida con la imagen de textura
       InitializeSharedMemory(shmArray,shmSize,lado,distancia);

    //End TODO *************************************

    
    //Begin NoTocar-------------------------------------
        //Mostramos los 10 primeros elementos del array de memoria compartida, los 10 primeros pixels de la imagen
        PrintArray("Compartida",shmArray,10);
    //End NoTocar-------------------------------------

    //Begin TODO *************************************
	    //---***Bucle para crear a los hijos***---
        //Se crean NUMHIJOS  hijos, comporbando qeu fork no provoca errores
        //Se calculan los indices ixIni y ixEnd en base a la carga, que es la misma para todos los hijos.
        //Se llama al código del Hijo CodigoHijo pasándole los índices que le tocan.
	    for(i=0; i < NUMHIJOS; i++)	{ //El padre itera creando hijos
		   pidHijo = fork();
		   if (pidHijo==-1) {
				printf("Error: no se ha podido realizar fork\n");
				exit(-1);
			}
			if (pidHijo==0) {
                //Hijo es el que hace algo.
				ixIni = i * carga; //esto sale en el examen
				ixEnd = i * carga + (carga-1); //esto también
				CodigoHijo(ixIni,ixEnd); //se llama a código de hijo.
				
			}

	    }
    //End TODO *************************************

    //Aquí sólo llega el padre
    //Begin TODO *************************************
        //Leemos del hijosPipe el arrayDatos de cada uno de los hijos y generamos su fichero de resultados
        
        for(i=0; i<NUMHIJOS; i++){
            //Leemos el pipe 
           LeerArrayPipe(hijosPipe[0], arrayDatos, TAMDATOS);

           //Generamos el fichero de resultados para este hijo
           sprintf(strBuffer,"R%d.dat",i);
           GenerarFicheroResultados(strBuffer,i, arrayDatos, lado, distancia);
        }
    //End TODO *************************************

    //Begin TODO *************************************
	    //Bucle para esperar la terminación de los hijos 
	    for(i=0; i < NUMHIJOS; i++){
		     waitpid(-1,&status,0);
	    }
    //End TODO *************************************
	 
    //Begin TODO *************************************
        //Guardamos el array compartido como un fichero .BMP
        SaveAsBMP(nombreFicheroBMP, shmArray, lado);

    //End TODO *************************************


    //Begin TODO *************************************
	    //El padre se desvincula del array de memoria compartida
        shmdt(shmArray);

    //End TODO *************************************
	
    //Begin TODO *************************************
	    //El padre destruye el array de memoria compartida
	    shmctl(shmId,IPC_RMID,0);

    //End TODO *************************************

	return 0;
}
