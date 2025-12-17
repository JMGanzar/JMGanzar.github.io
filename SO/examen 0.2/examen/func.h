// Definimos el rango válido de partidos políticos
#define MIN_TALLERES 2
#define MAX_TALLERES 10

// Estructura que representa un taller mecánico con su información básica y pipe
typedef struct { 
    int pid;                 // PID del taller mecánico.
    int id_taller;           // Identificador del taller. Puede ser el iterador del bucle donde se crean los procesos hijo.
    int sSincroRecepcion;    // Semáforo para sincronizar la recepción de vehículos a reparar desde el padre.
    int vehiculos_reparados; // Variable donde se almacenará el número de vehiculos reparados por cada taller.
    int p[2];                // Descriptores del pipe.
} TALLER;

extern TALLER *talleres;     // Array de estructuras hijo (talleres mecánicos)
extern int talleresAbiertos; // variable glboal de número de talleres que han abierto

// Prototipos de funciones
void TallerAbre(int sig, siginfo_t *siginfo, void *context);
int CrearPipes(int numTalleres);
bool CheckIfFileExists(const char* filename);
bool isNumber(char number[]);
void Sintaxis();
int RandInt(int M, int N);
char **leerVehiculos(const char *nombreArchivo, int *numVehiculos);