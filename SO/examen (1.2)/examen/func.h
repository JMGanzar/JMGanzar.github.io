// Definimos el rango válido de partidos políticos
#define MIN_PARTIDOS 2
#define MAX_PARTIDOS 6

// Estructura que representa una comunidad autónoma con su información básica y pipe
typedef struct { 
    int pid;          // PID de la comunidad autónoma
    char nombre[100]; // nombre de la comunidad autónoma
    int num_votantes; // Número de votantes de la comunidad autónoma
    int p[2];         // Descriptores del pipe
} COMUNIDAD;

// Estructura que almacena los resultados de votación de una comunidad autónoma
typedef struct {
    int pid; // PID de la comunidad autónoma
    int vectorResultados[MAX_PARTIDOS]; // vector donde se almacenarán los resultados de los diferentes partidos políticos
} RESULTADOS;

extern COMUNIDAD *comunidades;  // Array de estructuras hijo

// Prototipos de funciones
int CrearPipes(int numHijos);
float LeerParticipacion(int fdPipe);
void EscribirParticipacion(int fdPipe, float dato);
bool CheckIfFileExists(const char* filename);
bool isNumber(char number[]);
void Sintaxis();
int RandInt(int M, int N);
int contarLineas(const char *filename);
int extraer_string(const char *input, char *str, int *num);
int leerVotantes(const char *filename, int numComunidades);
float generar_porcentaje_aleatorio(float min, float max);