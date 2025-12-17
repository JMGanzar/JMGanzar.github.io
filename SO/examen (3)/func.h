#define IDLE 0
#define WORKING -1

#define SIG_JOBDONE     0
#define JOB_END 0
#define JOB_DONE 1
#define READ 0
#define WRITE 1

//Definimos una unión usada en sigqueue
union sigval value;

//Definimos la estructura HIJO
typedef struct { 
    int pid ; // Pid del hijo 
  	int pPC[2]; // Descriptores el pipe usado para comunicacion PARENT -> CHILD
  	int pCP[2]; // Descriptores el pipe usado para comunicacion CHILD  -> PARENT 
  	int jobsPending; //Numero de Jobs pendientes de ejecutar
  	int jobsDone;    //Numero de jobs procesados por el hijo
} HIJO ;


int numHijos; //numero de hijos
HIJO *hijos;  //Array de estructuras hijo
bool doContinue; //para controlar el bucle de lectura de jobs
//int jobsDone; //Variable que indica el numero de jobs hechos 

void Sintaxis();
int NewArrayHijos(int numHijos);
int CrearPipes(int numHijos);
void DoJob(char *filePath, int childIndex, int carga);
void SaveJobCounts(char *filePath, int numChilds);
int GetChildIndex(HIJO *hijos, int numHijos, int childPid);
int LeerPipe(int fdPipe);
void EscribirPipe(int fdPipe, int dato);
int RandInt(int M, int N);
