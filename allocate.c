#include <stdlib.h>
#include <stdio.h>
#include "main.h"

tuple_t* allocateTupleList(const int dim1) {
	tuple_t* newArray;
	newArray = (tuple_t*)malloc(dim1*sizeof(tuple_t));
	if (newArray == NULL)
		printf("Malloc failed!");
	return newArray;
}

GtkWidget** allocateWidgetList(const int dim1) {
	GtkWidget** newArray;
	newArray = (GtkWidget**)malloc(dim1*sizeof(GtkWidget*));
	if (newArray == NULL)
		printf("Malloc failed!");
	return newArray;
}

void deallocateWidgetList( GtkWidget** widget ) {
	free( widget );
}

float* allocateArray1D(const int dim1) {
	float* newArray;
	newArray = (float*)malloc(dim1*sizeof(float));
	if (newArray == NULL)
		printf("Malloc failed!");
	return newArray;
}

int* allocateArray1DInt(const int dim1) {
	int* newArray;
	newArray = (int*)malloc(dim1*sizeof(int));
	if (newArray == NULL)
		printf("Malloc failed!");
	return newArray;
}

float** allocateArray2D(const int dim1, const int dim2) {
	int i;
	float** newArray;
	newArray = (float**)malloc(dim1*sizeof(float*));
	if (newArray == NULL)
		printf("Malloc failed!");
	for (i = 0; i < dim1; i++) {
		newArray[i] = (float*)malloc(dim2*sizeof(float));
		if (newArray[i] == NULL)
			printf("Malloc failed!");
	}
	return newArray;
}

int** allocateArray2DInt(const int dim1, const int dim2) {
	int i;
	int** newArray;
	newArray = (int**)malloc(dim1*sizeof(int*));
	if (newArray == NULL)
		printf("Malloc failed!");
	for (i = 0; i < dim1; i++) {
		newArray[i] = (int*)malloc(dim2*sizeof(int));
		if (newArray[i] == NULL)
			printf("Malloc failed!");
	}
	return newArray;
}

float*** allocateArray3D(const int dim1, const int dim2, const int dim3) {
	int i, j;
	float*** newArray;
	newArray = (float***)malloc(dim1*sizeof(float**));
	if (newArray == NULL)
		printf("Malloc failed!");
	for (i = 0; i < dim1; i++) {
		newArray[i] = (float**)malloc(dim2*sizeof(float*));
		if (newArray[i] == NULL)
			printf("Malloc failed!");
		for (j = 0; j < dim2; j++) {
			newArray[i][j] = (float*)malloc(dim3*sizeof(float));
			if (newArray[i][j] == NULL)
				printf("Malloc failed!");
		}
	}
	return newArray;
}

int*** allocateArray3DInt(const int dim1, const int dim2, const int dim3) {
	int i, j;
	int*** newArray;
	newArray = (int***)malloc(dim1*sizeof(int**));
	if (newArray == NULL)
		printf("Malloc failed!");
	for (i = 0; i < dim1; i++) {
		newArray[i] = (int**)malloc(dim2*sizeof(int*));
		if (newArray[i] == NULL)
			printf("Malloc failed!");
		for (j = 0; j < dim2; j++) {
			newArray[i][j] = (int*)malloc(dim3*sizeof(int));
			if (newArray[i][j] == NULL)
				printf("Malloc failed!");
		}
	}
	return newArray;
}
