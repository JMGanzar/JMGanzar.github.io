#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h> 
#include <sys/types.h>
#include "functions.h"

/* ESTAS FUNCIONES NO HAY QUE TOCARLAS */
/* SI QUEREIS PONER FUNCIONES NUEVAS   */
/* HACERLO AL FINAL DEL FICHERO        */
/* LAS FUNCIONES QUE HAY QUE COMPLETAR ESTAN AL FINAL */

//NO TOCAR LAS SIGUIENTES FUNCIONES
int rcIx(int row, int col, int numRows){
	return numRows*row+col;
}

int ixRow(int ix, int numRows){
	return ix/numRows;
}

int ixCol(int ix, int numRows){
	return ix%numRows;
}

float NumBlocksPerSide(int numChilds){
	return sqrt(numChilds);
}

float BlockSize(int rows, int cols, int numChilds){
	float numElements=(float)rows*cols; 

	return numElements/(float)numChilds;
}


void PrintMatrix(int *shmMatrix, int rows, int cols){
	int r,c;

	for (r=0; r<rows; r++) 	{ 
		printf("\n"); 
		for (c=0; c<cols; c++) 
			//printf("\t%d", rcIx(r,c,rows)); 
			printf("\t%d", shmMatrix[rcIx(r,c,rows)]); 
	} 
	printf("\n");
}

void PrintBlock(int *pBlock, int numItems){
	int i; 
	for (i=0;i<numItems;i++)
		printf("%d ", pBlock[i]); 
	printf("\n");
}

void PrintBlockData(int child, int *shmMatrix, int rows, int cols, int numChilds){
	int i;
	int blockSize = (int)BlockSize(rows, cols, numChilds);
	int block[blockSize];

	GetBlock(shmMatrix,child,rows,cols,numChilds,rows,&block[0]);
	printf("Block %d Data: ",child);
	for (i=0;i<blockSize;i++)
		printf("%d ", block[i]); 
	printf("\n");
}

void PrintCorrectChilds(int dim){
	int i;
	//Control masivo de parametros
	printf("Número de hijos validos para matrices %dx%d : ",dim,dim);
	for (i=1; i<=(dim*dim); i++){
		if (CheckParameters(i,dim)){
			printf("%d ",i);
		}
	}
	printf("\n");
}

bool CheckParameters(int numChilds, int dim){
	int rows=dim; 
	int cols=dim; 
	float numElements,blockSize,sideBlocks,blockSide,numBlocks;

	if (dim>10000) return false; 

	//numElements=(float)rows*cols; 

	blockSize=BlockSize(rows, cols, numChilds); //numElements/numChilds;
	sideBlocks=NumBlocksPerSide(numChilds);
	blockSide=sqrt(blockSize);
	numBlocks=sideBlocks*sideBlocks;
	
	if ( numChilds==1 || (sideBlocks==(int)sideBlocks && blockSize==(int)blockSize) ) {
		//printf("[%d,%d] %.f blocks %.fx%.f = %.f elements\n",numChilds,dim,numBlocks,blockSide,blockSide,numBlocks*blockSize);
		return true;
	}
	else {
		return false;
	}
}

void GetBlock(int *shmMatrix, int child, int matrixRows, int matrixCols, int numChilds, int matrixDim, int *pBlock){
	int blockSide=NumBlocksPerSide(numChilds);
	float blockSize=BlockSize(matrixRows, matrixCols, numChilds);

	float blockDim=sqrt(blockSize);
	int blockRows=blockDim;
	int blockCols=blockDim;
	int r,c,ixMat=0,matValue; 
	int *pItem=pBlock;

	int childRow = (int)child/blockSide;
	int childCol = child%blockSide;
	int offsetRow = (int)childRow*sqrt(blockSize);
	int offsetCol = (int)childCol*sqrt(blockSize);

	//printf("offset=[%d,%d]\n",offsetRow,offsetCol);
	//printf("ixMat=[%d]\n",ixMat);

	for (r=0;r<blockRows;r++){
		for (c=0;c<blockCols; c++){
			ixMat=rcIx(offsetRow+r, offsetCol+c, matrixDim);
			matValue=shmMatrix[ixMat];
			//printf("shmMatrix[%d][%d](%d)=%d\n",offsetRow+r,offsetCol+c,ixMat,shmMatrix[ixMat]);
			*(pItem++)=matValue;
		}
	}
}

int ContarCeros(int *pBlock, int numItems){
	//Nos pasan un puntero a un bloque de enteros de tamaño numItems y tenemos que contar y devolver
	//el número de valores iguales a cero.
	int i,cont=0;
	for (i=0; i<numItems; i++){
		if (pBlock[i]==0)
			cont++;
	}
	return cont;
}

int ContarParesNoCeros(int *pBlock, int numItems){
	//Nos pasan un puntero a un bloque de enteros de tamaño numItems y tenemos que contar y devolver
	//el número de valores pares no nulos.
	int i,cont=0;
	for (i=0; i<numItems; i++){
		if ((pBlock[i]%2)==0 && pBlock[i]!=0)
			cont++;
	}
	return cont;
}

int ContarImpares(int *pBlock, int numItems){
	//Nos pasan un puntero a un bloque de enteros de tamaño numItems y tenemos que contar y devolver
	//el número de valores impares.
	int i,cont=0;
	for (i=0; i<numItems; i++){
		if ((pBlock[i]%2)!=0)
			cont++;
	}
	return cont;
}


/*------------------------------------------------- */
/* ESTAS SON LAS FUNCIONES QUE HAY QUE COMPLETAR    */
void InitMatrix(int *shmMatrix, int rows, int cols){	
	//Nos pasan un puntero a la matriz compartida en memoria.
	//Debemos inicializarla con números aleatorios entre 0 y 100
	//Podemos usar la funcion rcIx para obtener el índice de la matriz que ocupa 
	//la fila "r" y columna "c" en una matriz de "rows" numero de filas. 
	int maxInteger,minInteger; 
	int r,c;

	//BEGIN TODO ----------------------------------------------------------
	maxInteger=100; 
	minInteger=0;
	for (r=0;r<rows;r++){
		for (c=0; c<cols; c++){
		 	shmMatrix[rcIx(r,c,rows)]=rand() % (maxInteger-minInteger + 1) ;
		}
	}
	//END TODO ----------------------------------------------------------
}


void SaveData(int ceros, int pares, int impares){
	//Nos pasan los datos a guardar en el fichro.
	//Se debe generar un fichero con un dato en cada línea, en el orden en que se pasan.
	//El array decentas también ira un elemento del array en una línea independiente.
	//El nombre del fichero tiene el patron: "Pnnnnnn_data.txt" donde nnnnnn es el PID del proceso que lo ejecuta
	//BEGIN TODO ----------------------------------------------------------
	int i; 
	FILE *fp;
	char filename[20];

	sprintf(filename,"P%d_data.txt",(int)getpid());
	//printf("Grabando fichero %s\n",filename);

	// Open file with write permission (create if doesn't exist).
	fp = fopen(filename, "w");
	if (!fp){
		perror("Error grabando el fichero:");
	}
	else{
		fprintf(fp, "%d\n%d\n%d\n",ceros,pares,impares);
		fclose(fp);
	}
   	//END TODO ----------------------------------------------------------
}


/*------------------------------------------------- */
/* A PARTIR DE AQUI PONER LAS FUNCIONES QUE QUERAIS */


