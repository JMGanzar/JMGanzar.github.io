#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "sem.h"
#include "functions.h"

int main( int argc, char **argv ){
	int key,numHijos;
	char buffer[100];

	//Declaramos más variables necesarias
	...

	//Declaramos el identificador del semáforo que juega el rol de barrera
	...
	
	//Obtenemos los parámetros que nos pasan
	key      <---- primer parametro
	numHijos <---- segundo parametro

	//Declararmos el array de semáforos para los hijos y los inicializamos a cero
	...

	//Creamos la barrera (semáforo) para el padre
	...

	//---***Bucle para crear a los hijos***---
	for(i=0; i < numHijos; i++)	{ //El padre itera creando hijos
		pid=fork();
		if(pid == -1){   
		   sprintf(buffer,"ERROR. Fork ha fallado al crear al hijo %d\n",i);
		   perror(buffer);
		   exit(-1);
		}
		if(pid == 0){//Código hijo
		   
		   //El hijo espera en su semáforo
		   ...
		   
		   //El hijo decodifica su parte llamando a Decrypt() pasando la clave y la parte que debe decodificar.
		   Decrypt(key, i);
		   
		   //El hijo indica que ha desencriptado, señaliza la barrera del padre
		   ...

	       //El hijo termina, no itera
		   exit(0); 
		}
	}

    //Aquí sólo llega el padre
    
    //El padre trocea el fichero .crypt llamando a Partir()
    Partir(numHijos);

	//El padre señaliza la barrera de todos los hijos para que continúen
	...
        
    //El padre espera en su barrera a que todos los hijos lo señalicen
    ...
	
	//Bucle para esperar la terminación de los hijos
	...

	return 0;
} 
