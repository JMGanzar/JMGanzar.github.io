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
#include <time.h>
#include "func.h"
#include "sem.h"
#include "qdbmp.h"


#define SSEED 99   
#define SHMSEED 24 
#define SHMPERMISOS 0644
#define MINHIJOS 1
#define MAXHIJOS 20


//Puntero del proceso donde queremos ubicar el segmento de memoria compartida.
UCHAR *shmRGB; 

//Tamaño de un entero
int intSize; 

int lineasPipe[2]; //Pipe para volcar las líneas a procesar. Escribe el padre. Leen los hijos.
int hijosPipe[2]; //Pipe para que los hjos comuniquen al padre la línea que han procesado.
int numHijos; // para almacenar el numero de hijos

//Declaramos el identificador del semáforo que juega el rol mutex para el acceso a la memoria compartida
int sMutex;
 
//***************************************************
//CODIGO HIJOS 
// numHijo : Indica el numero de hijo que es
// width   : Indice el ancho de la imagen.
//***************************************************
void CodigoHijo(int numHijo, int width, int height)
{
    int i; //iterador generico
		int linea; //linea a procesar
    int mensaje[2]; // Array que usan los hijos para añadir al pipe tras cada linea procesada

	  //Declara tres punteos a UCHAR para almacenar los valores para la linea 
		//Un array para cada color : Red, Green y Blue.
		UCHAR* redValues;
		UCHAR* greenValues;
		UCHAR* blueValues;

		//Begin TODO H1 *************************************
			//Reserva memoria dinámica para los tres arrays de UCHAR, para los valores de RED, GREEN y BLUE
			//El tamaño del array viene definido por el parámetro width.
			redValues = (UCHAR *)malloc(width * sizeof(UCHAR));
			greenValues = (UCHAR *)malloc(width * sizeof(UCHAR));
			blueValues = (UCHAR *)malloc(width * sizeof(UCHAR));

			// Verifica si la reserva de memoria fue exitosa. 
			//En caso de error el hijo termina con código de terminación -1
			if (redValues == NULL || greenValues == NULL || blueValues == NULL ) {
					printf("Error al reservar memoria para los arrays de rgb.\n");
					exit(-1);  // Termina el programa con un código de error
			}
		//End TODO *************************************

		//Begin TODO H2 *************************************
			//Lee del pipe la primera linea que tiene que procesar.
			//Usa la variable linea para almacenar la línea leida.
			linea = LeerIntPipe(lineasPipe[IXR]);
		//End TODO *************************************
		
		//Begin TODO H3 *************************************
		//Mientras la linea sea distinta de -1 (condicion de terminación) itera.
		while(linea!=-1){ //Procesa la linea
			//Lee la linea en su memoria local llamando a la función ObtenerLineaBMP
			ObtenerLineaBMP(shmRGB, linea, width, height, redValues, greenValues, blueValues);

			//Voltea la linea en su memoria local. Voltea cada uno de los tres arrays.
			//Utilizar la funcion VoltearLinea() que debéis completar en func.c
			VoltearArray(redValues, width);
			VoltearArray(greenValues, width);
			VoltearArray(blueValues, width);

			//Pide acceso a la sección crítica, acceso a memoria compartida para escribir la linea tratada y sale de la sección crítica.
			//Usar la función EscribirLineaBMP definida en func.h para copiar la linea local en la compartida
			sWait(sMutex);
				EscribirLineaBMP(shmRGB, linea, width, height, redValues, greenValues, blueValues);	
			sSignal(sMutex);

			//Escribe en el pipeHijos su procesamiento.
				//Construye el mensaje en el array mensaje con el numero de hijo en el indice 0 y la linea procesada en el indice 1
				mensaje[0]=numHijo;
				mensaje[1]=linea; 
				//Escribe el array mensaje en el pipeHijos
				EscribirArrayPipe(hijosPipe[IXW], mensaje, 2);

			//Lee la siguiente linea del pipe de lineas
			linea = LeerIntPipe(lineasPipe[IXR]);
		}
		//End TODO ***************************************

		//Termina
		//Begin TODO H4 *************************************
			//El hijo se desvincula del segmento shmRGB 
			shmdt(shmRGB); 

			//El hijo libera su memoria local para los arrays redValues, greenValues y blueValues
			free(redValues);
			free(greenValues);
			free(blueValues);
		//End TODO ***************************************

		exit(0);
}
 

int main( int args, char *argv[] )
{
		FILE* fd; //Descriptor de fichero para generar el fichero de resultados

    //Begin TODO P1 *************************************
			// Inicializamos el generador de números aleatorios con una semilla
			srand(time(NULL));
    //End TODO *************************************

    //Begin NoTocar-------------------------------------
	    int i; //iterador generico
      int pidHijo;  //para registrar el pid del hijo tras fork     
	    int status;   //para capturar el valor de retorno del hijo
      int numPixels; // Tamaño en pixels (num enteros) de la imagen . 

			//Numero de linea que el padre enviará por el pipe 
			int linea; 
			int *lineas; //Array de lineas que el padre enviará por el pipe, se creará dinamicamente más adelante.

			//Mensaje de dos posiciones que el hijo envía al padre por el pipeHijos para que sepa que hijo es y que linea ha tratado.
			int mensaje[2]={0,0}; //mensaje[0] el numero de hijo    mensaje[1] numero de línea
			
			//Nombres de los ficheros de entrada y salida.
	    char intpuBMPFilename[100], outputBMPFilename[100], outputTXTFilename[100]; 
	
	    BMP*    bmp; //fichero bmp cargado en memoria
	    UINT    width, height, x, y; //ancho y alto de la imagen, y posiciones x e y de los pixels 

	    //Variablee para el identificador del segmento de memoria compartida para el array  RGB
	    int shmId_RGB;

      //Variable para  el tamaño del segmento de memoria compartida
      int shmSize_RGB;
    //End NoTocar-------------------------------------

    //Begin TODO P2 *************************************
 		//Creamos el semáforo sMutex inicializado apropiadamente
 		sMutex=sCreate(SSEED,1);

      //Comprobamos los parametros. 
        //Error si el numero de argumentos no es 3
        //Error si el numero de hijos no esta entre MINHIJOS y MAXHIJOS
	    if ( args !=3 )
	    {
	        fprintf( stderr, "Usage: %s <input file> <numero hijos>\n", argv[ 0 ] );
	        return 0;
	    }
	    numHijos=atoi(argv[2]);
	    if (numHijos<MINHIJOS || numHijos >MAXHIJOS){
	    	fprintf( stderr, "El número de hijos debe estar entre %d y %d\n",MINHIJOS,MAXHIJOS);
	      fprintf( stderr, "Usage: %s <input file> <numero hijos>\n", argv[ 0 ] );
	    	return 0;
	    }
    //End TODO *************************************

    //Begin NoTocar-------------------------------------
			//Se construyen los nombres de los fichero de entrada, salida y resultados.
			sprintf(intpuBMPFilename,"%s.bmp",argv[1]); //Nombre del fichero BMP
			sprintf(outputBMPFilename,"%s_VOLT.bmp",argv[1]); //Nombre del fichero BMP INVERTIDO
			sprintf(outputTXTFilename,"%s.txt",argv[1]); //Nombre del fichero de resultados

      //Cargamos la foto bmp en memoria
	    bmp = BMP_ReadFile(intpuBMPFilename);
	    BMP_CHECK_ERROR( stderr, -1 ); /* If an error has occurred, notify and exit */

	    /* Obtenemos y comprobamos las dimensiones de la imagen */
	    width  = BMP_GetWidth( bmp );  //width es el número de columnas de la imagen
	    height = BMP_GetHeight( bmp ); //height es el número de lineas de la imagen.
      numPixels=width*height; // Tamaño del vector (Numero de pixels de la imagen) (width x height) 
	    printf("Cargada imagen %s de %d x %d (%d pixels) ... espere, por favor...\n",intpuBMPFilename,width,height,numPixels);
    //End NoTocar-------------------------------------

    //Begin TODO P3 *************************************
	  //Creamos con memoria dinámica un array tantas lineas como indique la variable height y lo ordenamos aleatoriamente
		    //Reserva dinámica de memoria para el array lineas
    		lineas = (int *)malloc(height * sizeof(int));

				// Verifica si la reserva de memoria fue exitosa. En caso de error terminar 
				if (lineas == NULL) {
						printf("Error al reservar memoria para el array lineas.\n");
						exit(1);  // Termina el programa con un código de error
				}

				// Llena el array con números los numeros de linea en orden creciente
				for (int i = 0; i < height; i++) {
						lineas[i] = i;
				}

				//Reordena aleaotriamente el array con la función ShuffleArray()
				ShuffleArray(lineas,height);

    //End TODO *************************************

    //Begin TODO P4 *************************************
        //Creamos los pipes para comunicar con los hijos, el lineasPipe y el hijosPipe
				pipe(lineasPipe);
        pipe(hijosPipe);

				//El padre escribe ya en el array de lineasPipe las lineas ordenadas 
				//aleatoriamente así cuando los hijos arranquen ya habrá datos en el hijosPipe
				for (i=0; i<height; i++){
					EscribirIntPipe(lineasPipe[IXW],lineas[i]);
				}

				//El padre escribe en el lineasPipe tantos -1 (condicion de terminación de los hijos)
				//como hijos tenga que crear.
				for (i=0; i<numHijos; i++){
					EscribirIntPipe(lineasPipe[IXW],-1);
				}

    //End TODO *************************************

    //Begin TODO P5 *************************************
	    //Se define el tamaño del segmentos de memoria compartida
			//Tener en cuenta que una imagen RGB tiene tres valores para cada pixel correspondientes a los colores Red, Green y Blue, y que la imagen tiene tantos pixels como el producto de su ancho y alto y que el tipo de datos es UCHAR
	    shmSize_RGB = numPixels * sizeof(UCHAR) * 3;

	    //Creamos los segmento de memoria commpartida y obtenemos su id en shmId_RGB 
	    shmId_RGB = shmget(ftok("/bin/ls", SHMSEED), shmSize_RGB, IPC_CREAT | SHMPERMISOS);
		
		  //Se asingna (vincula, attach) ese segmento a la variable shmRGB definida previamente
	    shmRGB = shmat(shmId_RGB,0,0);

      //Inicializaos los segmentos de memoria compartida mediante la función InitializeSharedMemmory ver func.h
      InitializeSharedMemory(shmRGB, width, height, bmp);
			
			printf("La memoria compartida ha sido cargada.\n"); fflush(stdout);
    //End TODO *************************************


    //Begin TODO P6 *************************************
	    //---***Bucle para crear a los hijos***---
			//Se crean numHijos  hijos, comporbando que fork no provoca errores
			//Se llama al código del Hijo con la función CodigoHijo (definida ariba en este mismo fichero) 
			for(i=0; i < numHijos; i++)	{ //El padre itera creando hijos
				pidHijo = fork();
				if (pidHijo==-1) {
					printf("Error: no se ha podido realizar fork\n");
					exit(-1);
				}
				if (pidHijo==0) {
					//Hijo es el que hace algo.
					//Se llama a código de hijo. Le decimos que hijo es con i, y el ancho de la imagen con width
					CodigoHijo(i, width, height); 
				}
			}
    //End TODO *************************************

    //Aquí sólo llega el padre

    //Begin TODO P7 *************************************
	    //Bucle para esperar la terminación de los hijos 
	    for(i=0; i < numHijos; i++){
		     waitpid(-1,&status,0);
		 }
    //End TODO *************************************
	 
    //Begin TODO P8 *************************************
				//Abrimos el fichero de resultados para escritura comprobando si se ha creado correctamente.
				fd = fopen(outputTXTFilename, "w+");
				if (fd == NULL) {
						perror("Error al crear el fichero de resultados: ");
						return 1;
				}

        //Leemos del hijosPipe en bucle cada una de las lineas procesadas, es decir, el mensaje que
				//escribió el hijo cuando procesó una linea. 
				//Hay por tanto height mensajes que leer del pipeHijos.
				//Se aprovecha la lectura el pipe para escribir en el fichero de resultados outputTXTFilename en el mismo bucle.
        for(i=0; i<height; i++){
            //Leemos el pipe hijosPipe el mensaje con el numero de hijo y el numero de linea
            LeerArrayPipe(hijosPipe[IXR], mensaje, 2);
						fprintf(fd,"Hijo %2d; Linea %4d\n",mensaje[0],mensaje[1]);
        }
				
				//Cerramos el fichero de resultados.
				fclose(fd); 
    //End TODO *************************************


    //Begin TODO P9 *************************************
        //Guardamos el array compartido como un fichero .BMP usando la funcion SaveAsBMP, ver func.h
					SaveAsBMP(outputBMPFilename, shmRGB, width, height, bmp);
    //End TODO *************************************

    printf("Imagen volteada: %s\n",outputBMPFilename);

    //Begin TODO P10 *************************************
	    //El padre se desvincula del array de memoria compartida
        shmdt(shmRGB); 

    //End TODO *************************************
	
    //Begin TODO P11 *************************************
	    //El padre destruye el array de memoria compartida 
	    shmctl(shmId_RGB,IPC_RMID,0);

	    //El padre destruye el mutex
        sDestroy(sMutex);

	    //Libera la memoria reservada para el array lineas
    	free(lineas);

    //End TODO *************************************

	return 0;
}
