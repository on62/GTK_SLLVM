#include <stdlib.h>
#include <stdio.h>
#include "main.h"

void randomizeArray2DInt(int** array, int dim1, int dim2, int nRand1, int nRand2, int nRand3){
	int i, j, k;
	for (i=0; i<dim1; i++){
		for (j=0; j<dim2; j++){
			k = (rand() % 100);
			if (k < nRand1)
				array[i][j] = 1;
			else if ((k-nRand1) < nRand2)
				array[i][j] = 2;
			else if ((((k-nRand1)-nRand2)) < nRand3)
				array[i][j] = 3;
			else
				array[i][j] = 0;
		}
	}
}

void printArray1D(float* array, int dim1){
	int i;
	for (i=0; i<dim1; i++)
		printf("%g", array[i]);
	printf("\n");
}

void printArray1DInt(int* array, int dim1){
	int i;
	for (i=0; i<dim1; i++)
		printf("%i", array[i]);
	printf("\n");
}

void printArray2DInt(int** array, int dim1, int dim2){
	int i, j;

	for (i=0; i<dim1; i++){
		for (j=0; j<dim2; j++){
			printf("%i", array[i][j]);
		}
		printf("\n");
	}
}

void copyArray2DInt(int** arrayOld, int** arrayNew, int dim1, int dim2){
	int i, j;
	for (i=0; i<dim1; i++)
		for (j=0; j<dim2; j++)
			arrayNew[i][j] = arrayOld[i][j];
}
