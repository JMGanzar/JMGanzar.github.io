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

#define SSEED 99           // Semilla para Semáforos
#define SHMSEEDBUFFER 35   // Semilla para Buffer
#define SHMPERMISOS 0644   // Permisos para memoria compartida

#define CANAL_LECTURA 0
#define CANAL_ESCRITURA 1

// Definición de la variable 'comunidades'
COMUNIDAD *comunidades = NULL;  // Array global de estructuras COMUNIDAD

// Programa principal
int main(int argc, char *argv[])
{
    // Declaración de variables para memoria compartida
    int shmIdData, shmDataSize; // Para la memoria compartida
    RESULTADOS *shmData; // Puntero a la memoria compartida (vector de estructuras RESULTADOS)

    // Semáforo de sincronización entre padre e hijos
    int sSincro;
    // Semáforo de acceso a memoria compartida mediante exclusión mútua
    int sExclusion;

    int pid; // PID del fork
    int numComunidades; // Número de comunidades autónomas

    int i, j; // Iteradores

    char buffer[200] = "";                // Buffer genérico para almacenar texto
    FILE *fd;                             // Descriptor de fichero genérico
    char archivoVotantes[50] = "";        // Archivo donde se leerán los votantes de las comunidades autónomas
    char archivoResultadoVotos[100] = ""; // Archivo donde se escribirá el resultado de las votaciones de las comunidades autónomas
    int numPartidos;

    //BEGIN TODO 1 ***********************************************
        // Control de paraḿetros y asignación a variables
        // Si error mostramos mensaje, sintaxis y terminamos con -1
        
        // Controlamos el numero de parametros.
        if(argc != 4){
            printf("ERROR: Número incorrecto de parámetros.\n");
            Sintaxis();
            exit(-1);
        }
        
        // Controlamos si el tercer parámetro es numérico 
        if (!isNumber(argv[3])){
            printf("ERROR: El tercer argumento debe ser numérico.\n");
            Sintaxis();
            exit(-1);
        }

        // Asignación de parámetros a variables locales
        // archivoVotantes       <-- argv[1]
        // archivoResultadoVotos <-- argv[2]
        // numPartidos           <-- argv[3]
        strcpy(archivoVotantes, argv[1]);
        strcpy(archivoResultadoVotos, argv[2]);
        numPartidos      = atoi(argv[3]);
        
        // Controlamos que numPartidos está entre 2 y 6 (MIN_PARTIDOS y MAX_PARTIDOS)
        if (numPartidos < MIN_PARTIDOS || numPartidos > MAX_PARTIDOS){
            printf("ERROR: El parámetro numPartidos debe ser un número entre %d y %d.\n", MIN_PARTIDOS, MAX_PARTIDOS);
            Sintaxis();
            exit(-1);
        }

    //END TODO 1 ***********************************************

    //BEGIN TODO 2 ***********************************************
        // Obtenemos el número de comunidades autónomas a partir del archivo de votantes
        // Podéis usar la función contarLineas
        numComunidades = contarLineas(archivoVotantes);

        // Creamos el array de estructuras comunidades usando memoria dinámica y
        // lo asociamos al puntero global correspondiente.
        comunidades = (COMUNIDAD *)malloc(numComunidades * sizeof(COMUNIDAD));
        
        // Comprobamos que se ha asignado correctamente la memoria dinámica.
        // Si ha fallado, notificamos con un mensaje y salimos con -1.
        if (comunidades == NULL) {
            printf("Error al asignar memoria para las comunidades.\n"); 
            exit(-1); 
        }

        // Leemos el archivo de votantes y rellenamos el array de comunidades
        // Si hay error, mostramos mensaje y salimos con -1
        if (leerVotantes(archivoVotantes, numComunidades)){
            printf("Error al leer y asignar los datos del archivo de votantes\n.");
            exit(-1);
        }

    //END TODO 2 ***********************************************

    //BEGIN TODO 3 ***********************************************
        // Creamos los pipes de las comunidades llamando a CrearPipes.
        // Esta función recibe el número de comunidades (numComunidades).
        // Si  la funcion no devuelve el numero de comunidades mostramos
        // error y salimos con -1
        if (numComunidades != CrearPipes(numComunidades)){
            printf("ERROR en la creación de los pipes\n");
            exit(-1); 
        }

    //END TODO 3 ***********************************************

    

    //BEGIN TODO 4 ***********************************************
        // Definimos el tamaño, creamos el segmento de memoria compartida shmData y asignamos correctamente los punteros.
        // Si Error mostramos mensaje y salimos
        shmDataSize = numComunidades * sizeof(RESULTADOS);
        shmIdData = shmget(ftok("./", SHMSEEDBUFFER), shmDataSize, IPC_CREAT | SHMPERMISOS);
        
        // Comprobamos que se ha creado correctamente el segmento shmIdData,
        // si no, mostramos mensaje y salimos con -1.
        if (shmIdData == -1){
            printf("Error al crear el segmento de memoria compartida.\n");
            exit(-1);
        }
        
        // Apuntamos shmData a la memoria compartida reservada
        shmData = shmat(shmIdData, 0, 0);
        if (shmData == (void *)-1) {
            perror("Error al mapear la memoria compartida.\n");
            exit(-1);
        }

        // Se inicializa la memoria compartida (shmData) con valores iniciales,
        // tanto el campo pid [-1] como el vector vectorResultados [0].
        for (i = 0; i < numComunidades; i++) {
            shmData[i].pid = -1;
            for (int j = 0; j < MAX_PARTIDOS ; j++) {
                shmData[i].vectorResultados[j] = 0;
            }
        }

    //END TODO 4 ***********************************************

    //BEGIN TODO 5 ***********************************************
        // Creamos el semáforo `sSincro` de sincronización entre padre e hijos
        sSincro = sCreate(SSEED, 0);

        // Creamos el semáforo `sExclusion` para la exclusión mutua de la memoria compartida (usa una semilla diferente)
        sExclusion = sCreate(SSEED+1, 1);

    //END TODO 5 ***********************************************

    //BEGIN TODO 6 ***********************************************
        // Bucle para crear a las comunidades autónomas (hijos)
        for(i = 0; i < numComunidades; i++) { // El padre itera creando las comunidades. Utilizar i como iterador

            // Llamamos a la función fork() obteniendo en la variable pid el valor retornado
            pid = fork();

            // Si no se ha podido crear la comunidad, mostramos error y salimos con -1
            if(pid == -1){
                printf("ERROR. Fallo en fork al crear la comunidad %d - %s\n", i, comunidades[i].nombre);
                exit(-1);
            }

            //Determinamos si el pid es de un hijo, en cuyo caso actuamos
            if(pid == 0){ // Código del hijo
                // Inicializamos el generador de numeros aleatorios con el pid del hijo
                srand(getpid());
                
                // Cerramos el canal de escritura del pipe
                close(comunidades[i].p[CANAL_ESCRITURA]);

                // El hijo recibe por el pipe el porcentaje de participación de su comunidad autónoma
                float participacion = LeerParticipacion(comunidades[i].p[CANAL_LECTURA]);

                // El hijo espera en el semáforo hasta que la Junta Electoral Central de el permiso para abrir los colegios electorales (sincronismo)
                sWait(sSincro);

                printf("[%s] Apertura de colegios electorales con participación del %.2f %%.\n", comunidades[i].nombre, participacion);

                // VOTACIÓN

                printf("[%s] Realizando las votaciones.\n", comunidades[i].nombre);

                int total_votos_asignados = 0; // Contador de los votos asignados
                int votos_restantes = (int)(participacion * comunidades[i].num_votantes / 100); // Votos restantes por asignar

                // El hijo accede a la sección crítica
                sWait(sExclusion);

                // Guardamos el PID de la comunidad en memoria compartida
                shmData[i].pid = getpid();

                // Asignar votos aleatoriamente a los partidos
                for (int j = 0; j < numPartidos; j++) {
                    // Generar un número aleatorio de votos para este partido
                    shmData[i].vectorResultados[j] = rand() % (votos_restantes + 1); // Máximo: votos_restantes
                    total_votos_asignados += shmData[i].vectorResultados[j];
                    votos_restantes -= shmData[i].vectorResultados[j]; // Reducir los votos restantes
                }

                // El hijo sale de la sección crítica
                sSignal(sExclusion);

                printf("[%s] Cierre de los colegios electorales.\n", comunidades[i].nombre);

                // El hijo se desvincula de la memoria compartida
                shmdt(shmData);
                
                // Cerramos el canal de lectura del pipe
                close(comunidades[i].p[CANAL_LECTURA]);
                
                // El hijo (comunidad) cierra el proceso
                exit(0);
            }
            else { // Código del padre
                // El padre guarda el PID del hijo en el vector de estructuras de HIJO
                comunidades[i].pid = pid;

                // Cerramos el canal de lectura del pipe
                close(comunidades[i].p[CANAL_LECTURA]);
            }
        } // Fin del bucle de creación de hijos

    //END TODO 6 ***********************************************

    // Aquí sólo llega el padre, los hijos hacen exit.
    
    //BEGIN TODO 7 ***********************************************
        // Inicializamos el generador de numeros aleatorios con el pid del padre
        srand(getpid());

        printf("[Junta Electoral Central]: Enviando a las comunidades los datos de participación.\n");
        // El padre envía a las comunidades por pipe el porcentaje de participación
        for (i = 0; i < numComunidades; i++){
            // Número aleatorio entre 60 y 75
            float participacion = generar_porcentaje_aleatorio(50.0, 80.0);
            EscribirParticipacion(comunidades[i].p[CANAL_ESCRITURA], participacion);
        }

        // El padre da permiso a los hijos para abrir los colegios electorales (sincronismo)
        printf("[Junta Electoral Central]: Se abren los colegios electorales.\n");
        for (i= 0; i < numComunidades; i++){
            sSignal(sSincro);
        }

    //END TODO 7 ***********************************************

    //BEGIN TODO 8 ***********************************************
        // Cerramos los canales de escritura de todos los hijos
        for (i = 0; i < numComunidades; i++){
            close(comunidades[i].p[CANAL_ESCRITURA]);
        }

    //END TODO 8 ***********************************************

    //BEGIN TODO 9 ***********************************************
        // El padre espera la terminación de los hijos
        // Codificar el bucle para esperar la terminación de los hijos usando la funcion wait o la función waitpid
        for(i = 0; i < numComunidades; i++){
            pid = wait(NULL); // No necesitamos guardar el valor de retorno de los hijos
        }

        printf("[Junta Electoral Central] Todos los colegios electorales han cerrado.\n");
        
    //END TODO 9 ***********************************************

    //BEGIN TODO 10 ***********************************************
        // La Junta Electoral Central lee y procesa los resultados de las votaciones por cada comunidad autónoma

        // El padre abre el fichero de resultados para escritura
        // si el fichero no se puede abrir, mostramos error y salimos
        fd = fopen(archivoResultadoVotos, "w");
        if (fd == NULL){
            printf("Error al crear el archivo %s\n", archivoResultadoVotos);
            exit(1);
        }

        // Por cada comunidad autónoma
        for(i = 0; i < numComunidades; i++){
            char* nombre = comunidades[i].nombre;
            RESULTADOS resultado = shmData[i];

            // Comprobamos que los pid coinciden para ambas estructuras (COMUNIDAD y RESULTADOS)
            // Si no coincide, mostramos error y salimos
            if (comunidades[i].pid != resultado.pid){
                printf("Error de consistencia en la asignación de PIDs para %s\n.", nombre);
                exit(1);
            }

            // Escribimos en fichero el nombre de la comunidad autónoma y una coma
            fprintf(fd, "%s,", nombre);
            // Por cada partido político
            for (j=0; j<numPartidos; j++){
                // Escribimos en fichero el número de votos
                fprintf(fd, "%d", shmData[i].vectorResultados[j]);
                // Si no es el último partido político (la última iteración del bucle), escribimos una coma
                if (j < numPartidos - 1) {
                    fprintf(fd, ","); // Añadir coma si no es el último elemento
                }
            }
            // Escribimos un salto de línea
            fprintf(fd, "\n");
        }

        // Cerramos el archivo
        fclose(fd);

        printf("[Junta Electoral Central] Los votos han sido almacenados a la espera del recuento final.\n");
        
    //END TODO 10 ***********************************************

    //BEGIN TODO 11 ***********************************************
        // Liberación de recursos compartidos (memoria, semáforos)
        
        // El padre se desvincula de la memoria compartida
        shmdt(shmData);
        
        // El padre borra la asignación de memoria compartida
        shmctl(shmIdData, IPC_RMID, 0);
        
        // El padre destruye los semáforos
        sDestroy(sSincro);
        sDestroy(sExclusion);

        // El padre libera la memoria dinámica de comunidades
        free(comunidades);

    //END TODO 11 ***********************************************

    return 0;
}
