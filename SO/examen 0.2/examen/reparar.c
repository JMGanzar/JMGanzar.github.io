#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>   
#include <stdbool.h>
#include <sys/wait.h>
#include "func.h"
#include "sem.h"

#define SSEED 99           // Semilla para Semáforos
#define SHMSEEDBUFFER 35   // Semilla para Buffer
#define SHMPERMISOS 0644   // Permisos para memoria compartida

#define CANAL_LECTURA 0
#define CANAL_ESCRITURA 1

// Definición de la variable 'talleres'
TALLER *talleres = NULL;  // Array global de estructuras TALLER
// Definición de variable que medirá el número de talleres que han abierto
int talleresAbiertos = 0;

// Programa principal
int main(int argc, char *argv[])
{
    // Declaración de variables para memoria compartida
    int shmIdData, shmDataSize; // Para la memoria compartida
    int *shmData; // Puntero a la memoria compartida (vector de enteros)

    // Semáforo de sincronización entre padre e hijos
    int sSincro;
    // Semáforo de acceso a memoria compartida mediante exclusión mútua
    int sExclusion;
    // Existe un tercer semáforo, que está definido en la estructura TALLER (func.h)

    int pid; // PID del fork
    int numTalleres; // Número de talleres mecánicos

    int i, j; // Iteradores

    char buffer[1024] = "";           // Buffer genérico para almacenar texto
    FILE *fd;                         // Descriptor de fichero genérico
    char archivoVehiculos[50] = "";   // Archivo donde se leerán los vehículos (matrículas)
    char archivoResultados[100] = ""; // Archivo donde se escribirá el resultado de las reparaciones de los talleres

    //BEGIN TODO 1 ***********************************************
        // Control de paraḿetros y asignación a variables
        // Si error mostramos mensaje, sintaxis y terminamos con -1
        
        // Controlamos el numero de parametros.
        if(argc != 4){
            fprintf(stderr, "ERROR: Número incorrecto de parámetros.\n");
            Sintaxis();
            exit(-1);
        }
        
        // Controlamos si el primer parámetro es numérico 
        if (!isNumber(argv[1])){
            fprintf(stderr, "ERROR: El primer argumento debe ser numérico.\n");
            Sintaxis();
            exit(-1);
        }

        // Asignación de parámetros a variables locales
        // numTalleres                  <-- argv[1]
        // archivoVehiculos             <-- argv[2]
        // archivoResultados <-- argv[3]
        numTalleres = atoi(argv[1]);
        strcpy(archivoVehiculos, argv[2]);
        strcpy(archivoResultados, argv[3]);
        
        // Controlamos que numTalleres está entre 2 y 10 (MIN_TALLERES y MAX_TALLERES)
        if (numTalleres < MIN_TALLERES || numTalleres > MAX_TALLERES){
            printf("ERROR: El parámetro numTalleres debe ser un número entre %d y %d.\n", MIN_TALLERES, MAX_TALLERES);
            Sintaxis();
            exit(-1);
        }

        // Comprobamos si el segundo parámetro (archivoVehiculos) es un fichero existente.
        // Puedes usar alguna función de func.c
        if (!CheckIfFileExists(archivoVehiculos)){
            printf("ERROR: El archivo %s no existe.\n", archivoVehiculos);
            Sintaxis();
            exit(-1);
        }

    //END TODO 1 ***********************************************

    //BEGIN TODO 2 ***********************************************
        //Se define la estructura para el manejador de las señales que recibe el padre
        struct sigaction manejador;

        // Este es el manejador de señales en tiempo real
        manejador.sa_sigaction = TallerAbre;    // Nombre de la funcion manejadora
        manejador.sa_flags = SA_SIGINFO;        // Esto lo definimos para enviar informacion adicional junto a la señal
        sigaction(SIGRTMIN, &manejador , NULL); // Establecemos el manejador de la señal a la función manejadora

    //END TODO 2 ***********************************************

    //BEGIN TODO 3 ***********************************************
        // Creamos el array de estructuras TALLER usando memoria dinámica y
        // lo asociamos al puntero global correspondiente.
        talleres = (TALLER *)malloc(numTalleres * sizeof(TALLER));

        // Comprobamos que se ha asignado correctamente la memoria dinámica.
        // Si ha fallado, notificamos con un mensaje y salimos con -1.
        if (talleres == NULL) {
            fprintf(stderr, "Error al asignar memoria para los talleres.\n"); 
            exit(-1); 
        }
    
    //END TODO 3 ***********************************************

    //BEGIN TODO 4 ***********************************************
        // Creamos los pipes de los talleres llamando a CrearPipes.
        // Esta función recibe el número de talleres (numTalleres).
        // Si  la funcion no devuelve el numero de talleres mostramos
        // error y salimos con -1

        if (numTalleres != CrearPipes(numTalleres)){
            fprintf(stderr, "ERROR en la creación de los pipes\n");
            exit(-1); 
        }

    //END TODO 4 ***********************************************

    //BEGIN TODO 5 ***********************************************
        // Obtenemos los vehículos a reparar a partir del archivo de vehículos.
        int numVehiculos = 0; // Variable donde se van a almacenar la cantidad de vehículos siniestrados
        
        // Declara la variable 'vehiculos'
        char **vehiculos = NULL; // Array de cadena de caracteres, donde se almacenarán las matrículas de los vehículos
        
        // Llamamos a la función leerVehiculos, pasando el nombre del archivo y la variable numVehiculos por referencia.
        vehiculos = leerVehiculos(archivoVehiculos, &numVehiculos);

        // Si ha habido algún fallo en leerVehiculos, mostramos error y salimos.
        if (!vehiculos){
            fprintf(stderr, "No se pudieron leer los vehiculos.\n");
            exit(-1);
        }

        // Recorremos el array y mostramos los vehiculos (depuración)
        /*
        for (i = 0; i < numVehiculos; i++) {
            printf("Vehiculo [%d]: %s\n", i, vehiculos[i]);
        }
        */

    //END TODO 5 ***********************************************
    
    //BEGIN TODO 6 ***********************************************
        // Definimos el tamaño, creamos el segmento de memoria compartida shmData y asignamos correctamente los punteros.
        // Si Error mostramos mensaje y salimos
        shmDataSize = sizeof(int);
        shmIdData = shmget(ftok("./", SHMSEEDBUFFER), shmDataSize, IPC_CREAT | SHMPERMISOS);
        
        // Comprobamos que se ha creado correctamente el segmento shmIdData,
        // si no, mostramos mensaje y salimos con -1.
        if (shmIdData == -1){
            fprintf(stderr, "Error al crear el segmento de memoria compartida.\n");
            exit(-1);
        }
        
        // Apuntamos shmData a la memoria compartida reservada
        shmData = shmat(shmIdData, 0, 0);
        if (shmData == (void *)-1) {
            fprintf(stderr, "Error al mapear la memoria compartida.\n");
            exit(-1);
        }

        // Se inicializa la memoria compartida (shmData) con valores iniciales [0]
        shmData[0] = 0;

    //END TODO 6 ***********************************************

    //BEGIN TODO 7 ***********************************************
        // Creamos el semáforo `sSincro` de sincronización entre padre e hijos
        sSincro = sCreate(SSEED, 0);

        // Creamos el semáforo `sExclusion` para la exclusión mutua de la memoria compartida (utiliza otra semilla)
        sExclusion = sCreate(SSEED + 1, 1);

        // Creamos los semáforos `sSincroRecepcion` (dentro del vector TALLER talleres) para sincronización de petición
        // de vehículos entre padre e hijos (utiliza una semilla única para cada taller)
        // La semilla debe inicializarse con la capacidad máxima de cada taller
        for(i = 0; i<numTalleres; i++){
            talleres[i].sSincroRecepcion = sCreate(SSEED + 2 + i, 1);
        }

    //END TODO 7 ***********************************************
    
    //BEGIN TODO 8 ***********************************************
        // Bucle para crear a los talleres mecánicos (hijos)
        for(i = 0; i < numTalleres; i++) { // El padre itera creando los talleres. Utilizar i como iterador

            // Llamamos a la función fork() obteniendo en la variable pid el valor retornado
            pid = fork();

            // Si no se ha podido crear el taller mecánico, mostramos error y salimos con -1
            if(pid == -1){
                fprintf(stderr, "ERROR. Fallo en fork al crear el taller mecánico con ID %d\n", i);
                exit(-1);
            }

            //Determinamos si el pid es de un hijo, en cuyo caso actuamos
            if(pid == 0){ // Código del hijo                
                // Cerramos el canal de escritura del pipe
                close(talleres[i].p[CANAL_ESCRITURA]);
                
                // Asociamos a id_taller correspondiente el iterador del bucle for, `i`.
                talleres[i].id_taller = i;

                // Inicializamos el generador de numeros aleatorios con el pid del hijo
                srand(getpid());

                int min_us = 50000;  // Tiempo mínimo de reparación (microsegundos)
                int max_us = 100000; // Tiempo máximo de reparación (microsegundos)

                // El taller (hijo) notifica a la Aseguradora (padre) que ha abierto el taller mecánico (señal en tiempo real)
                union sigval valor;
                valor.sival_int = getpid(); //Asignamos el PID del hijo al dato
                
                // Enviamos una señal al padre con el pid del hijo para que me envíe el ACK
                sigqueue(getppid(), SIGRTMIN, valor);  // Enviamos AL PADRE la señal en tiempo real con el valor como argumento

                printf("[Taller %d] Apertura del taller.\n", talleres[i].id_taller);

                // Recepción de vehiculos
                while(1){
                    //printf("[Taller %d] Esperando vehiculo.\n", i);

                    // Leemos por pipe el vehículo y lo guardamos en buffer
                    read(talleres[i].p[CANAL_LECTURA], buffer, sizeof(buffer));

                    // Sale del bucle while si recibe END
                    if (strcmp(buffer, "END") == 0) {
                        break;
                    }

                    //printf("[Taller %d] Reparando vehiculo [%s]...\n", i, buffer);
                    
                    // Simulamos tiempo de reparación aleatorio
                    // usa la función usleep y los tiempos máximo y mínimo establecidos previamente
                    // Puedes usar la función RandInt (func.c) para generar el valor aleatorio
                    usleep(RandInt(min_us, max_us));
                    
                    // Incrementamos el contador de vehiculos reparados (de la estructura TALLER)
                    talleres[i].vehiculos_reparados++;

                    printf("[Taller %d] vehiculo [%s] reparado.\n", i, buffer);

                    // El hijo accede a la sección crítica
                    sWait(sExclusion);

                    // Incrementamos el contador de vehículos reparados (memoria compartida)
                    shmData[0]++;

                    // Comprobamos si se han reparado todos los vehículos, en cuyo caso
                    // liberamos al padre de su semáforo de sincronización
                    if (shmData[0] == numVehiculos){
                        sSignal(sSincro);
                    }
                    
                    // El hijo sale de la sección crítica
                    sSignal(sExclusion);

                    // Señaliza a su semáforo que puede recibir otro vehículo
                    sSignal(talleres[i].sSincroRecepcion);
                }

                printf("[Taller %d] Cierre del taller.\n", talleres[i].id_taller);

                // El hijo se desvincula de la memoria compartida
                shmdt(shmData);
                
                // Cerramos el canal de lectura del pipe
                close(talleres[i].p[CANAL_LECTURA]);
                
                // El hijo (taller) cierra el proceso, saliendo con en número de vehiculos reparados
                exit(talleres[i].vehiculos_reparados);
            }
            else { // Código del padre
                // El padre guarda el PID y el ID del hijo en el vector de estructuras de HIJO
                talleres[i].pid = pid;
                talleres[i].id_taller = i;

                // Cerramos el canal de lectura del pipe
                close(talleres[i].p[CANAL_LECTURA]);
            }
        } // Fin del bucle de creación de hijos

    //END TODO 8 ***********************************************

    // Aquí sólo llega el padre, los hijos hacen exit.
    
    //BEGIN TODO 9 ***********************************************
        // Inicializamos el generador de numeros aleatorios con el pid del padre
        srand(getpid());
        
        // La Aseguradora (padre) espera a que todos los talleres mecánicos (hijos) abran.
        // (Recepción de señales en tiempo real)
        while(talleresAbiertos < numTalleres){
            pause();
        }
        printf("[Aseguradora]: Todos los talleres se encuentran abiertos.\n");

    //END TODO 9 ***********************************************

    //BEGIN TODO 10 ***********************************************
        // La aseguradora distribuye los vehículos entre los diferentes talleres mecánicos
        for (i = 0; i < numVehiculos; i++) {
            // Se elige un taller de forma aleatoria (puedes usar RandInt)
            int idTaller = RandInt(0, numTalleres - 1);

            // Espera a que el taller tenga espacio (semáforo de sincronismo de recepción)
            sWait(talleres[idTaller].sSincroRecepcion);

            // Envía por el pipe correspondiente el vehículo
            printf("[Aseguradora] Enviando vehículo [%s] al Taller %d\n", vehiculos[i], idTaller);
            write(talleres[idTaller].p[CANAL_ESCRITURA], vehiculos[i], strlen(vehiculos[i]) + 1);
        }

    //END TODO 10 ***********************************************

    //BEGIN TODO 11 ***********************************************
        // Esperamos a que todos los vehiculos se hayan reparado
        // Para eso, usamos el semáforo de sincronización sSincro
        sWait(sSincro);
        
        printf("[Aseguradora] Todos los vehiculos han sido reparados.\n");
        // Enviamos por pipe la señal de finalización a todos los talleres ("END")
        for (i = 0; i < numTalleres; i++) {
            write(talleres[i].p[CANAL_ESCRITURA], "END", strlen("END") + 1);
            //printf("[Aseguradora] Enviado END al Taller %d.\n", i);
        }

    //END TODO 11 ***********************************************

    //BEGIN TODO 12 ***********************************************
        // Cerramos los canales de escritura de todos los hijos
        for (i = 0; i < numTalleres; i++){
            close(talleres[i].p[CANAL_ESCRITURA]);
        }

    //END TODO 12 ***********************************************

    //BEGIN TODO 13 ***********************************************
        // El padre espera la terminación de los hijos, recogiendo el valor retornado (número de vehículos reparados)
        // Codificar el bucle para esperar la terminación de los hijos usando la funcion wait o la función waitpid
        // Se deberá determinar a qué hijo pertenece y almacenarlo en la variable vehiculos_reparados correspondiente.
        int status;
        for(i = 0; i < numTalleres; i++){
            pid = wait(&status); // No necesitamos guardar el valor de retorno de los hijos
            for(j = 0; j < numTalleres; j++){
                if (pid == talleres[j].pid){
                    talleres[j].vehiculos_reparados = (status >> 8) & 0xFF;
                    break;
                }
            }
        }

        printf("[Aseguradora] Todos los talleres mecánicos han cerrado.\n");
        
    //END TODO 13 ***********************************************
    
    //BEGIN TODO 14 ***********************************************
        // La Aseguradora genera el archivo de resultados, donde almacenará por cada línea, correspondiente a cada taller,
        // el número de vehículos reparado

        // El padre abre el fichero de resultados para escritura
        // si el fichero no se puede abrir, mostramos error y salimos
        fd = fopen(archivoResultados, "w");
        if (fd == NULL){
            fprintf(stderr, "Error al crear el archivo %s\n", archivoResultados);
            exit(-1);
        }

        // Por cada taller
        for(i = 0; i < numTalleres; i++){
            // Escribimos en fichero el número de vehículos reparados
            fprintf(fd, "%d\n", talleres[i].vehiculos_reparados);
        }

        // Cerramos el archivo
        fclose(fd);

        printf("[Aseguradora] Se ha grabado el fichero de resultados.\n");
        
    //END TODO 14 ***********************************************

    //BEGIN TODO 15 ***********************************************
        // Liberación de recursos compartidos (memoria, semáforos)
        
        // El padre se desvincula de la memoria compartida
        shmdt(shmData);
        
        // El padre borra la asignación de memoria compartida
        shmctl(shmIdData, IPC_RMID, 0);
        
        // El padre destruye los semáforos
        sDestroy(sSincro);
        sDestroy(sExclusion);
        for(i = 0; i<numTalleres; i++){
            sDestroy(talleres[i].sSincroRecepcion);
        }

        // El padre libera la memoria dinámica de talleres
        free(talleres);

        // El padre libera la memoria dinámica de vehículos
        for (i = 0; i < numVehiculos; i++) {
            free(vehiculos[i]);
        }
        free(vehiculos);

    //END TODO 15 ***********************************************

    return 0;
}
