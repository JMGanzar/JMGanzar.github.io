#define TAMDATOS 4
#define TAMCUENTAS 3

#define PARESIMPARES 0
#define CLAROSOSCUROS 1

//Puntero del proceso donde queremos ubicar el segmento de memoria compartida.
int *shmArray; 

//tamaño de un entero
int intSize; 

int carga;
int ixIni,ixEnd;


//***************************************************
//INICIALIZACIÓN DEL SEGMENTO COMPARTIDO DE DATOS 
//***************************************************
void InitializeSharedMemory(int *shmArray, int shmSize, int lado, int distancia);

//***************************************************
//FUNCION EsPar que dice si un numero es par 
//***************************************************
bool EsPar(int numero);

//***************************************************
//FUNCION EsImpar que dice si un numero es impar 
//***************************************************
bool EsImpar(int numero);

//***************************************************
//FUNCION EsClaro que dice si un pixel es claro ( pixel>=128 )
//***************************************************
bool EsClaro(int pixValue);

//***************************************************
//MAYOR DE DOS ENTEROS 
//***************************************************
int Mayor(int a, int b);

//***************************************************
//MENOR DE DOS ENTEROS 
//***************************************************
int Menor(int a, int b);

//***************************************************
//LECTURA DE UN ARRAY DE ENTEROS DE UN PIPE 
//***************************************************
void LeerArrayPipe(int fdPipe, int *arrayDatos, int longitud);

//***************************************************
//ESCRITURA DE UN ARRAY DE ENTEROS DE UN PIPE 
//***************************************************
void EscribirArrayPipe(int fdPipe, int *arrayDatos, int longitud);

//***************************************************
//MOSTRAR UN ARRAY DE ENTEROS POR STDOUT
//***************************************************
void PrintArray(char *name, int *array, int length);

//***************************************************
//GUARDAR COMO BMP
//**************************************************
int SaveAsBMP(char *name, int *array, int lado);

//***************************************************
//GENERAR FICHERO RESULTADOS
//***************************************************
void GenerarFicheroResultados(char *fileName,int numHijo, int* arrayDatos, int lado, int distancia);

