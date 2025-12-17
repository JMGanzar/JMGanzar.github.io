#include <stdio.h>
#include <stdlib.h>
#include <signal.h>   
#include <unistd.h>   
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include "qdbmp.h"
#include "func.h"


// BEGIN TODO **********************************************************
    //***************************************************
    //VOLTEA UN ARRAY
    //***************************************************
    void VoltearArray(UCHAR* values, int width){
        //La función recibe el puntero a un array de UCHARs y la longitud del mismo.
        //Debe voltear el array, por ejemplo: 
        //  original:  1 2 3 4 5 6
        //  volteado:  6 5 4 3 2 1
        //  Al recibir un puntero a los datos, trabaja directamente con el array pasado.
        int inicio = 0;
        int fin = width - 1;
        UCHAR temp; 
        
        while (inicio < fin) {
            // Intercambia los elementos en inicio y fin
            temp = values[inicio];
            values[inicio] = values[fin];
            values[fin] = temp;
            
            // Avanza el puntero de inicio y retrocede el puntero de fin
            inicio++;
            fin--;
        }
    }
// END TODO **********************************************************


//EL RESTO DE FUNCIONES YA ESTAN IMPLEMENTADAS, NO TOCAR.

//***************************************************
//INICIALIZACIÓN DEL SEGMENTO COMPARTIDO DE DATOS 
// shmArray : Segmento de memoria compartida para los pixels de la imagen
//***************************************************
void InitializeSharedMemory(UCHAR *shmArray, int width, int height, BMP *bmp){
    int i;
    int numPixels;
    int ixPixel,ixRed,ixGreen,ixBlue;

	UCHAR   r, g, b; 
    UINT    x, y;

    numPixels=width*height;

    /* Iterate through all the image's pixels */
    for ( y = 0 ; y < height ; ++y )
    {
        for ( x = 0 ; x < width ; ++x )
        {
            /* Get pixel's RGB values */
            //bmp almacena por planoos, primero todos r, luego todos g y luego todos b
            BMP_GetPixelRGB( bmp, x, y, &r, &g, &b );  
            ixPixel=y*width+x;
            ixRed=ixPixel;
            ixGreen=ixRed+numPixels;
            ixBlue=ixGreen+numPixels;
            shmArray[ixRed]=r;
            shmArray[ixGreen]=g;
            shmArray[ixBlue]=b;
	        //printf("(%d,%d)->ix(%d)(%d,%d,%d)->shm(%d,%d,%d)=(%d,%d,%d)\n",x,y,ixPixel,r,g,b,ixRed,ixGreen,ixBlue,shmArray[ixRed],shmArray[ixGreen],shmArray[ixBlue]);
        }
    }

}

int SaveAsBMP(char *outFileName, UCHAR *shmArray, int width, int height, BMP *bmp){
    int i;
    int maxLuminance=255; 
    int numPixels;
    int ixPixel,ixRed,ixGreen,ixBlue;

	UCHAR   r, g, b; 
    UINT    x, y;

    int *shmRed, *shmGreen, *shmBlue;
    int UCHARSize= sizeof(UCHAR);

    numPixels=width*height;
    /* Iterate through all the image's pixels */
    for ( y = 0 ; y < height ; ++y )
    {
        for ( x = 0 ; x < width ; ++x )
        {
            /* Get pixel's RGB values */
            ixPixel=y*width+x;
            ixRed=ixPixel;
            ixGreen=ixRed+numPixels;
            ixBlue=ixGreen+numPixels;
            BMP_SetPixelRGB( bmp, x, y,  shmArray[ixRed], shmArray[ixGreen], shmArray[ixBlue] );
	        //printf("(%d,%d)->ix(%d)shm(%d,%d,%d)=(%d,%d,%d)\n",x,y,ixPixel,ixRed,ixGreen,ixBlue,shmArray[ixRed],shmArray[ixGreen],shmArray[ixBlue]);
        }
    }

    /* Save result */
    BMP_WriteFile( bmp, outFileName );
    BMP_CHECK_ERROR( stderr, -2 );

    /* Free all memory allocated for the image */
    BMP_Free( bmp );
}



//***************************************************
//OBTIENE LAS LINEA A VOLTEAR
//***************************************************
void ObtenerLineaBMP(UCHAR *shmArray, int linea, int width, int height, UCHAR* redValues, UCHAR* greenValues, UCHAR* blueValues){
    int x; 
    int y=linea;
    int numPixels=width*height;
    int ixPixel, ixRed, ixGreen, ixBlue;

    UCHAR r,g,b;
    
    //Obtenemos los valores red, green y blue.
    //printf("    Leyendo %d columnas: ",width); fflush(stdout);
    for (x=0; x<width; x++){
        //printf("      %d : ",x);
        ixPixel=y*width+x;
        ixRed=ixPixel;
        ixGreen=ixRed+numPixels;
        ixBlue=ixGreen+numPixels;
        r=shmArray[ixRed];
        g=shmArray[ixGreen];
        b=shmArray[ixBlue];
        //printf("%d r%d(%d) g%d(%d) b%d(%d)\n",ixPixel, ixRed, r, ixGreen, g, ixBlue,b); fflush(stdout);
        redValues[x]=r;
        greenValues[x]=g;   
        blueValues[x]=b;
        //printf("(%d,%d)->ix(%d)shm(%d,%d,%d)=(%d,%d,%d)->(%d,%d,%d)\n",x,y,ixPixel,ixRed,ixGreen,ixBlue,r,g,b,shmArray[ixRed],shmArray[ixGreen],shmArray[ixBlue]); fflush(stdout);
    }
    //printf("\n"); fflush(stdout);
}

//***************************************************
//ESCRIBE LA LINEA A VOLTEAR
//***************************************************
void EscribirLineaBMP(UCHAR *shmArray, int linea, int width, int height, UCHAR* redValues, UCHAR* greenValues, UCHAR* blueValues){
    int x; 
    int y=linea;
    int numPixels=width*height;
    int ixPixel, ixRed, ixGreen, ixBlue;
    UCHAR   r, g, b; 
    
    //Obtenemos los valores red, green y blue.
    for (x=0; x<width; x++){
        ixPixel=y*width+x;
        ixRed=ixPixel;
        ixGreen=ixRed+numPixels;
        ixBlue=ixGreen+numPixels;
        shmArray[ixRed]=redValues[x];
        shmArray[ixGreen]=greenValues[x];
        shmArray[ixBlue]=blueValues[x];
    }
}

//***************************************************
//MUESTRA LA LINEA A VOLTEAR
//***************************************************
void PrintLineaBMP(UCHAR *shmArray, int linea, int width, int height){
    int x; 
    int y=linea;
    int numPixels=width*height;
    int ixPixel, ixRed, ixGreen, ixBlue;
    UCHAR   r, g, b; 
    
    //Obtenemos los valores red, green y blue.
    printf("\n*************************\nBegin shmArray:\n"); fflush(stdout);
    for (x=0; x<width; x++){
        ixPixel=y*width+x;
        ixRed=ixPixel;
        ixGreen=ixRed+numPixels;
        ixBlue=ixGreen+numPixels;
        printf("[%d][%d][%d]\n",shmArray[ixRed],shmArray[ixGreen],shmArray[ixBlue]);
    }
    printf("\n*************************\nEnd shmArray:\n"); fflush(stdout);
}

//***************************************************
//LECTURA DE UN ENTERO DE UN PIPE 
// fdPipe     : descriptor del pipe del que leer
//***************************************************
int LeerIntPipe(int fdPipe){
    int value;
    read(fdPipe, &value, sizeof(int));
    return value;
}

//***************************************************
//ESCRITURA DE UN ENETRO EN UN PIPE 
//***************************************************
void EscribirIntPipe(int fdPipe, int value){
    write(fdPipe, &value, sizeof(int));
}

//***************************************************
//LECTURA DE UN ARRAY DE ENTEROS DE UN PIPE 
// fdPipe     : descriptor del pipe del que leer
// arrayDatos : array de enteros donde dejar los datos leidos del pipe
// longitud   : longitud del array de enteros que se lee del pipe (numero de enteros a leer)
//***************************************************
void LeerArrayPipe(int fdPipe, int *arrayDatos, int longitud){
    read(fdPipe, arrayDatos, longitud*sizeof(int));
}

//***************************************************
//ESCRITURA DE UN ARRAY DE ENTEROS DE UN PIPE 
// fdPipe     : descriptor del pipe en el que escribir
// arrayDatos : array de enteros del que coger los datos para ecribirlos en el pipe
// longitud   : longitud del array de enteros que se escribe en el pipe (numero de enteros a escribir)
//***************************************************
void EscribirArrayPipe(int fdPipe, int *arrayDatos, int longitud){
    write(fdPipe, arrayDatos, longitud*sizeof(int));
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
//REORDENA ALEATORIAMENTE UN ARRAY
//**************************************************
void ShuffleArray(int* array, int length) {
    for (int i = length - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}



