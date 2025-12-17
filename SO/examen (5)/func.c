#include <stdio.h>
#include <stdlib.h>
#include <signal.h>   
#include <unistd.h>   
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include "qdbmp.h"
#include "func.h"

//***************************************************
//INICIALIZACIÓN DEL SEGMENTO COMPARTIDO DE DATOS 
// shmArray : Segmento de memoria compartida para los pixels de la imagen
// shmSize  : Tamaño en bytes del segmento de memoria compartida
//***************************************************
void InitializeSharedMemory(int *shmArray, int shmSize, int lado, int distancia){
    int i;
    int maxLuminance=255; 
    int maxDistance=distancia;
    int increment,newVal,lastVal;
    int tamVector=lado*lado;

    srand((unsigned) time(NULL));
    for (i=0; i<tamVector; i++) {
        if (i==0) shmArray[i] = (int)(rand()%maxLuminance);
        else{
            lastVal=shmArray[i-1];
            increment=(int)(rand()%(maxDistance*2))-maxDistance;
            newVal=lastVal+increment; 
            shmArray[i]=Menor(255,Mayor(0,newVal));
        }
    }
}

//***************************************************
//GENERAR FICHERO RESULTADOS
//  fileName:   Nombre del fichero a generar
//  numHijo:    Numero de hijo (empezando por 0)   
//  arrayDatos: Array con los datos del bloque procesado por el hijo.
//***************************************************
void GenerarFicheroResultados(char *fileName, int numHijo, int* arrayDatos, int lado, int distancia)
{
    FILE *fp;

    fp = fopen(fileName, "w+");
        fprintf(fp, "%d\n",lado);
        fprintf(fp, "%d\n",distancia);
        fprintf(fp, "%d\n",carga);
        ixIni=numHijo*carga;
        ixEnd=numHijo*carga+(carga-1);
        fprintf(fp, "%d\n",ixIni);
        fprintf(fp, "%d\n",ixEnd);
        fprintf(fp, "%d\n",arrayDatos[0]);
        fprintf(fp, "%d\n",arrayDatos[1]);
        fprintf(fp, "%d\n",arrayDatos[2]);
        fprintf(fp, "%d\n",arrayDatos[3]);
    fclose(fp);
}

//***************************************************
//LECTURA DE UN ARRAY DE ENTEROS DE UN PIPE 
// fdPipe     : descriptor del pipe del que leer
// arrayDatos : array de enteros donde dejar los datos leidos del pipe
// longitud   : longitud del array de enteros que se lee del pipe (numero de enteros a leer)
//***************************************************
void LeerArrayPipe(int fdPipe, int *arrayDatos, int longitud){
    read(fdPipe, arrayDatos, longitud*intSize);
}

//***************************************************
//ESCRITURA DE UN ARRAY DE ENTEROS DE UN PIPE 
// fdPipe     : descriptor del pipe en el que escribir
// arrayDatos : array de enteros del que coger los datos para ecribirlos en el pipe
// longitud   : longitud del array de enteros que se escribe en el pipe (numero de enteros a escribir)
//***************************************************
void EscribirArrayPipe(int fdPipe, int *arrayDatos, int longitud){
    write(fdPipe, arrayDatos, longitud*intSize);
}

//***************************************************
//MOSTRAR UN ARRAY DE ENTEROS POR STDOUT
// name   : nombre que le damos al array para mostrarlo en stdout
// array  : array del que se muestran los datos
// length : Numero de elementos  mostrar 
//***************************************************
void PrintArray(char *name, int *array, int length){
    int i;
    printf("%s=[",name);
    for (i=0;i<length-1;i++)
        printf("%3d,",array[i]);
    printf("%3d]\n",array[length-1]);
}

//***************************************************
//FUNCION EsPar que dice si un numero es par 
// numero : Número que comprueba si es par
//RETORNA: true si el numero es par, false si no.
//***************************************************
bool EsPar(int numero)
{
    if ( numero % 2 == 0 ) return true;
    else return false;
}

//***************************************************
//FUNCION EsImpar que dice si un numero es impar 
// numero : Número que comprueba si es impar
//RETORNA: true si el numero es impar, false si no.
//***************************************************
bool EsImpar(int numero)
{
    if ( numero % 2 != 0 ) return true;
    else return false;
}

//***************************************************
//FUNCION EsClaro que dice si un pixel es claro ( pixel>=128 )
// pixValue : Pixel del que comprueba si es claro 
//RETORNA: true si el numero >= 128, false si no.
//***************************************************
bool EsClaro(int pixValue)
{
    if (pixValue>=128) return true;
    else return false; 
}

//***************************************************
//MAYOR DE DOS ENTEROS 
// a y b : son dos enteros
//RETORNA: el mayor de los dos
//***************************************************
int Mayor(int a, int b)
{
    if (a>=b) return a;
    else return b;
}

//***************************************************
//MENOR DE DOS ENTEROS 
// a y b : son dos enteros
//RETORNA: el menor de los dos
//***************************************************
int Menor(int a, int b)
{
    if (a<=b) return a;
    else return b;
}

//***************************************************
//GUARDAR COMO BMP
// name  : Nombre del fichero bmp a guardar (con la extension .bmp incluida)
// array : Array de datos con los pixels de la imagen. Segmento de memoria compartida
// lado  : Tamaño del lado de la imagen cuadrada (lado x lado)
//**************************************************
int SaveAsBMP(char *name, int *array, int lado){
    int x,y,i,ix=0;
    int luminance;
    BMP *bmp;

    //Creamos el BMP
    bmp=BMP_Create( lado, lado, 24);
    for (i=0; i<256; i++)
        BMP_SetPaletteColor(bmp, i, i,i,i);

    for (x=0;x<lado;x++){
        for(y=0;y<lado;y++){
            luminance=array[ix];
            BMP_SetPixelRGB( bmp, x, y, luminance, luminance, luminance);
            ix++;
        }
    }
    BMP_WriteFile( bmp, name);
    BMP_CHECK_ERROR( stdout, -2 );
    BMP_Free( bmp );
    return 0;
}


//***************************************************
//CONTAR PARES 
// Cuenta los enteros pares de un subconjunto del array de memoria compartida y los devuelve en un array
// ixIni : Indice del pixel por el que comenzar la cuenta
// ixEnd : Indice del pixel en el que se acaba la cuenta
// arrayCuentas : Array donde dejar los resultados
//       En el indice 0 indicamos que se trata de pares impares 
//       En el indice 1 van los pares
//       En el indice 2 van los impares
//***************************************************
void ContarPares(int ixIni, int ixEnd, int *arrayCuentas){
    int i,dato;
    arrayCuentas[0]=PARESIMPARES;
    for (i=ixIni; i<=ixEnd; i++){
        dato=shmArray[i];
        if (EsPar(dato)) arrayCuentas[1]=arrayCuentas[1]+1;
        else arrayCuentas[2]=arrayCuentas[2]+1;
    }
}


//***************************************************
//CONTAR CLAROS
// Cuenta los enteros claros de un subconjunto del array de memoria compartida y los devuelve en un array
// ixIni : Indice del pixel por el que comenzar la cuenta
// ixEnd : Indice del pixel en el que se acaba la cuenta
// arrayCuentas : Array donde dejar los resultados
//       En el indice 0 indicamos que se trata de claros y oscuros 
//       En el indice 1 van los claros
//       En el indice 2 van los oscuros
//***************************************************
void ContarClaros(int ixIni, int ixEnd, int *arrayCuentas){
    int i,dato;
    arrayCuentas[0]=CLAROSOSCUROS;
    for (i=ixIni; i<=ixEnd; i++){
        dato=shmArray[i];
        if (EsClaro(dato)) arrayCuentas[1]=arrayCuentas[1]+1;
        else arrayCuentas[2]=arrayCuentas[2]+1;
    }
}





