#include "qdbmp.h"

#define TAMDATOS 4
#define TAMCUENTAS 3

#define PARESIMPARES 0
#define CLAROSOSCUROS 1

#define IXR 0 //define el indice de lectura en el pipe
#define IXW 1 //define el indice de escritura en el pipe

//***************************************************
//INICIALIZACIÓN DEL SEGMENTO COMPARTIDO DE DATOS 
//***************************************************
void InitializeSharedMemory(UCHAR *shmArray, int width, int heigth, BMP *bmp);

//***************************************************
//OBTIENE LAS LINEA A VOLTEAR
//***************************************************
void ObtenerLineaBMP(UCHAR *shmArray, int linea, int width, int height, UCHAR* redValues, UCHAR* greenValues, UCHAR* blueValues);

//***************************************************
//ESCRIBE LA LINEA A VOLTEADA
//***************************************************
void EscribirLineaBMP(UCHAR *shmArray, int linea, int width, int height, UCHAR* redValues, UCHAR* greenValues, UCHAR* blueValues);


//***************************************************
//MUESTRA LA LINEA DEL SHMARRAY
//***************************************************
void PrintLineaBMP(UCHAR *shmArray, int linea, int width, int height);

//***************************************************
//VOLTEA UNA ARRAY
//***************************************************
void VoltearArray(UCHAR* values, int width);

//***************************************************
//LECTURA DE UN ENTERO DE UN PIPE 
//***************************************************
int LeerIntPipe(int fdPipe);

//***************************************************
//ESCRITURA DE UN ENETRO EN UN PIPE 
//***************************************************
void EscribirIntPipe(int fdPipe, int value);

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
int SaveAsBMP(char *outFileName, UCHAR *shmRGB, int width, int height, BMP *bmp );

// //***************************************************
// //GENERAR FICHERO RESULTADOS
// //***************************************************
// void GenerarFicheroResultados(char *filename);


//***************************************************
//REORDENA ALEATORIAMENTE UN ARRAY
//**************************************************
void ShuffleArray(int* array, int length); 
