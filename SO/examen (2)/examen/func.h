//Definimos la estructura HIJO
typedef struct { 
    int pid ; // Pid del hijo 
  	int p[2]; // Descriptores el pipe del hijo
} HIJO ;


HIJO *hijos;  //Array de estructuras hijo

#define PIEDRA 0
#define PAPEL 1
#define TIJERA 2

//Definimos una unión usada en sigqueue
union sigval value;

int NewArrayHijos(int numHijos);
int CrearPipes(int numHijos);
void EnviarSenyal();
void Sintaxis();
int RandInt(int M, int N);
bool isNumber(char number[]);
int LeerPipe(int fdPipe);
void EscribirPipe(int fdPipe, int dato);
char GetCharBet(int bet);
bool CheckIfFileExists(const char* filename);
