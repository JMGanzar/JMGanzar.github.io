#include <stdbool.h>

#define SSEED 99
#define SHMSEED 35 
#define SHMPERMISOS 0644


void PrintCorrectChilds(int dim);
bool CheckParameters(int numChilds, int dim);
float NumBlocksPerSide(int numChilds);
void InitMatrix(int *shmMatrix, int rows, int cols);
void PrintMatrix(int *shmMatrix, int rows, int cols);
float BlockSize(int rows, int cols, int numChilds);
void PrintBlockData(int child, int *shmMatrix, int rows, int cols, int numChilds);
void PrintBlock(int *pBlock, int numItems);
void GetBlock(int *shmMatrix, int child, int matrixRows, int matrixCols, int numChilds, int matrixDim, int *pBlock);
int rcIx(int row, int col, int rowSize);
int ixRow(int ix, int rowSize);
int ixCol(int ix, int rowSize);
int ContarCeros(int *pBlock, int numItems);
int ContarParesNoCeros(int *pBlock, int numItems);
int ContarImpares(int *pBlock, int numItems);
void SaveData(int ceros, int pares, int impares);
