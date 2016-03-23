/**
	MNA_Matrix.c
	For the undergraduate EDA course offered at 
	Dept of Micro/Nano-electronics

	(c) Guoyong Shi, 
	All rights reserved, 2006-2015
	Shanghai Jiao Tong University

	If you modify the code, please keep the above copyright info.
*/

#ifdef MATLAB
#include "mex.h"
#endif

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "Symbol_Table.h"
#include "MNA_Matrix.h"
#include "parse_func.h"

#define WARNING_DIV_BY_ZERO \
		printf("\nWarning: divide by zero.");

int MatrixSize = 0;	/* size of the MNA matrix (i.e., the max dimension) */
int NASize = 0; /* size of the NA matrix portion */
int MSize = 0; /* Size of the M matrix portion */
int iColl = 0; /* number of current dependent elements */
char **iNames = NULL; /* names of current dependant elements */
double ***MNAMatrix = NULL;
double *RHS = NULL;

/**
	Assign indexes to all nodes in the node table.
	The ground node (with name "0") must be assigned index zero.
	The rest nodes are assigned indexes from 1, 2, 3 continuously.
*/
void Index_All_Nodes(){
	int size=0;
	Node_Entry *nodePtr = *NodeTable;
	while(nodePtr != NULL){
		nodePtr->index = NameHash(nodePtr->name,NodeTableSize);
		if(nodePtr->index > size){
			size = nodePtr->index;
		}
		nodePtr = nodePtr->next;
	}
	NASize = size;	
	MSize = nVsrc + nInd;
	MatrixSize = NASize + MSize ;
	iColl = size;
}

int Get_Matrix_Size(){
	return MatrixSize;
}

/**
	The MATLAB matrix data are stored in column-major format.
*/
void Get_MNA_System(double **A, double **b){
	int i, j;

	for (j = 0; j <= MatrixSize; j++) {
		for (i = 0; i <= MatrixSize; i++) {
			(*A)[i+j*(MatrixSize+1)] = MNAMatrix[0][i][j];  /* convert to column-major format */
		}
		(*b)[j] = RHS[j];
	}
}


/**
	Create memory for MNAMatrix and RHS.
	The adopted storage is in DENSE matrix format.
	The zeroth row/col correspond to the ground.

	Must call Index_All_Nodes() before calling this function.

	(!!) Students can try SPARSE matrix format.
*/
void Init_MNA_System(){
#if 1
	int i, j;
	
	if (MatrixSize == 0) {
		return;
	}

	MNAMatrix = malloc(2*sizeof(*MNAMatrix));
	for (i = 0; i < 2; i++) {
		MNAMatrix[i] = malloc((MatrixSize+1)*sizeof(MNAMatrix[i]));	
	}
	for (i = 0; i < 2; i++) {
		for (j = 0; j < MatrixSize+1; j++) {
			MNAMatrix[i][j] = malloc((MatrixSize+1)*sizeof(*MNAMatrix[i][j]));
		}
	}

	RHS = (double*) malloc( (MatrixSize+1) * sizeof(double) );

	/* Initialize to zero */
	for (i = 0; i <= MatrixSize; i++) {
		for (j = 0; j <= MatrixSize; j++) {
			MNAMatrix[0][i][j] = 0.0;
			MNAMatrix[1][i][j] = 0.0;
		}
		RHS[i] = 0.0;
	}
	/* Alocate space for iNames */
	iNames = malloc((nVsrc+nInd)*sizeof(char*));
#endif
}

/**
	Create the MNA matrix by scanning the device table.
	In principle stamping of each element should be done by the device instance.
	In this example code we do not create device objects. 
	Therefore, stamping is done inside this function for simplicity.
	
	(!!) Students are required to develop device classes to fulfil this task.
*/
void Create_MNA_Matrix(){
	Device_Entry *devPtr = *DeviceTable;
	while(devPtr != NULL){
		switch(devPtr->name[0]){
			case 'R':
				MNAMatrix[0][devPtr->nodelist[0]->index][devPtr->nodelist[0]->index] += 1.0/devPtr->value;
				MNAMatrix[0][devPtr->nodelist[1]->index][devPtr->nodelist[1]->index] += 1.0/devPtr->value;
				MNAMatrix[0][devPtr->nodelist[0]->index][devPtr->nodelist[1]->index] -= 1.0/devPtr->value;
				MNAMatrix[0][devPtr->nodelist[1]->index][devPtr->nodelist[0]->index] -= 1.0/devPtr->value;
				break;
			case 'C':
				MNAMatrix[1][devPtr->nodelist[0]->index][devPtr->nodelist[0]->index] += devPtr->value;
				MNAMatrix[1][devPtr->nodelist[1]->index][devPtr->nodelist[1]->index] += devPtr->value;
				MNAMatrix[1][devPtr->nodelist[0]->index][devPtr->nodelist[1]->index] -= devPtr->value;
				MNAMatrix[1][devPtr->nodelist[1]->index][devPtr->nodelist[0]->index] -= devPtr->value;
				break;
			case 'L':
				iNames[iColl-NASize] = devPtr->name;
				iColl++;
				MNAMatrix[0][devPtr->nodelist[0]->index][iColl] = 1; 
				MNAMatrix[0][devPtr->nodelist[1]->index][iColl] = -1; 
				MNAMatrix[0][iColl][devPtr->nodelist[0]->index] = 1; 
				MNAMatrix[0][iColl][devPtr->nodelist[1]->index] = -1; 
				MNAMatrix[1][iColl][iColl] -= devPtr->value; 
				break;	
			case 'G':
				MNAMatrix[0][devPtr->nodelist[0]->index][devPtr->nodelist[2]->index] += devPtr->value;
				MNAMatrix[0][devPtr->nodelist[1]->index][devPtr->nodelist[2]->index] -= devPtr->value;
				MNAMatrix[0][devPtr->nodelist[0]->index][devPtr->nodelist[3]->index] -= devPtr->value;
				MNAMatrix[0][devPtr->nodelist[1]->index][devPtr->nodelist[3]->index] += devPtr->value;
				break;	
			case 'I':
				RHS[devPtr->nodelist[0]->index] -= devPtr->value;
				RHS[devPtr->nodelist[1]->index] += devPtr->value;
				break;	
			case 'V':
				iNames[iColl-NASize] = devPtr->name;
				iColl++;
				MNAMatrix[0][devPtr->nodelist[0]->index][iColl] = 1; 
				MNAMatrix[0][devPtr->nodelist[1]->index][iColl] = -1; 
				MNAMatrix[0][iColl][devPtr->nodelist[0]->index] = 1; 
				MNAMatrix[0][iColl][devPtr->nodelist[1]->index] = -1; 
				RHS[iColl] += devPtr->value;
				break;
			default:
				break;
		}
		devPtr = devPtr->next;
	}
}

void Print_MNA_System(){
	int i, j;
	printf("\n\n");
	for (j = 0; j <= NASize; j++) {
		printf("\t\t%-12d", j);
	}
	for (j = 0; j < MSize; j++) {
		printf("\t\ti(%s)\t", iNames[j]);
	}
	printf("\tRHS");
	
	for (i = 0; i <= MatrixSize; i++) {
		printf("\n[%-3d]", i);
		for (j = 0; j <= MatrixSize; j++) {
			printf("\t%f", MNAMatrix[0][i][j]);
			printf("%+fs", MNAMatrix[1][i][j]);
		}
		printf("\t%-12f", RHS[i]);
	}
}


