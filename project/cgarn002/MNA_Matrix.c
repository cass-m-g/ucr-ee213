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
#include "Eigen/Dense"

#define WARNING_DIV_BY_ZERO \
		printf("\nWarning: divide by zero.");

int MatrixSize = 0;	// size of the MNA matrix (i.e., the max dimension)
Eigen::MatrixXd MNAMatrix;
Eigen::VectorXd RHS;
int eqnIndex = 0; //index of extra equations

/**
	Assign indexes to all nodes in the node table.
	The ground node (with name "0") must be assigned index zero.
	The rest nodes are assigned indexes from 1, 2, 3 continuously.
*/
void Index_All_Nodes()
{
	eqnIndex = NodeTableSize;
	MatrixSize = NodeTableSize - 1 + nVsrc + nInd;
	Node_Entry *n;
	for(n = NodeTableHead; n; n = n->next){
		n->index = NameHash(n->name, NodeTableSize);
	}
}

int Get_Matrix_Size()
{
	return MatrixSize;
}

/**
	The MATLAB matrix data are stored in column-major format.
*/
void Get_MNA_System(double **A, double **b)
{
	int i, j;

	for (j = 0; j <= MatrixSize; j++) {
		for (i = 0; i <= MatrixSize; i++) {
			(*A)[i+j*(MatrixSize+1)] = MNAMatrix(i,j);  // convert to column-major format
		}
		(*b)[j] = RHS(j);
	}
}


/**
	Create memory for MNAMatrix and RHS.
	The adopted storage is in DENSE matrix format.
	The zeroth row/col correspond to the ground.

	Must call Index_All_Nodes() before calling this function.

	(!!) Students can try SPARSE matrix format.
*/
void Init_MNA_System()
{
#if 1
	int i, j;
	
	if (MatrixSize == 0) {
		return;
	}

	MNAMatrix.resize(MatrixSize+1,MatrixSize+1);
	RHS.resize(MatrixSize+1);

	for(i = 0; i<MatrixSize +1; i++){
		for(j = 0; j<MatrixSize+1; j++){
			MNAMatrix(i,j) = 0.0;
		}
		RHS(i) = 0.0;
	}
#endif
}

/**
	Create the MNA matrix by scanning the device table.
	In principle stamping of each element should be done by the device instance.
	In this example code we do not create device objects. 
	Therefore, stamping is done inside this function for simplicity.
	
	(!!) Students are required to develop device classes to fulfil this task.
*/
void Create_MNA_Matrix()
{
	Device_Entry *dev;
	for(dev = DeviceTableHead; dev; dev = dev->next){
		int indxp, indxm, indxcp, indxcm;
		if(dev->numnodes >= 2){
			indxp = dev->nodelist[0]->index;
			indxm = dev->nodelist[1]->index;
		}
		double val = dev->value;
		switch(dev->type){
			case DEV_RESISTOR:
				MNAMatrix(indxp,indxp) += 1.0/val;
				MNAMatrix(indxp,indxm) += -1.0/val;
				MNAMatrix(indxm,indxp) += -1.0/val;
				MNAMatrix(indxm,indxm) += 1.0/val;
				break;
			case DEV_INDUCTOR:
				MNAMatrix(eqnIndex,indxp) += 1;
				MNAMatrix(eqnIndex,indxm) += -1;
				MNAMatrix(eqnIndex,eqnIndex) += -val;
				MNAMatrix(indxp,eqnIndex) += 1;
				MNAMatrix(indxm,eqnIndex) += -1;
				++eqnIndex;				
				break;
			case DEV_CAPACITOR:
				MNAMatrix(indxp,indxp) += val;
				MNAMatrix(indxp,indxm) += -val;
				MNAMatrix(indxm,indxp) += -val;
				MNAMatrix(indxm,indxm) += val;

				break;
			case DEV_VS:
				MNAMatrix(eqnIndex,indxp) += 1;
				MNAMatrix(eqnIndex,indxm) += -1;
				MNAMatrix(indxp,eqnIndex) += 1;
				MNAMatrix(indxm,eqnIndex) += -1;
				RHS(eqnIndex) += val;
				++eqnIndex;				
				break;
			case DEV_CS:
				RHS(indxp) += -val;
				RHS(indxm) += val;	
				break;
			case DEV_VCCS:
				if(dev->numnodes >= 4){
					indxcp = dev->nodelist[2]->index;
					indxcm = dev->nodelist[3]->index;
				}
				else{
					printf("ERROR\n");
					return;
				}
				MNAMatrix(indxp,indxcp) += val;
				MNAMatrix(indxp,indxcm) += -val;
				MNAMatrix(indxm,indxcp) += -val;
				MNAMatrix(indxm,indxcm) += val;

				break;
			case DEV_NO_TYPE:			
				printf("MUST BE AN ERROR\n");
				break;
		}
	}
}

void Solve_MNA(){
	Eigen::VectorXd x = MNAMatrix.colPivHouseholderQr().solve(RHS);
	int i, j;
	
	printf("\n\nSolution:");
	for (i = 0; i <= MatrixSize; i++) {
		printf("\n[%-3d]", i);
		printf("\t%-12f", x(i) - x(0));
	}
	printf("\n");
}

void Print_MNA_System()
{
	int i, j;
	
	printf("\n\n");
	for (j = 0; j <= MatrixSize; j++) {
		printf("\t%-12d", j);
	}
	printf("\tRHS");
	
	for (i = 0; i <= MatrixSize; i++) {
		printf("\n[%-3d]", i);
		for (j = 0; j <= MatrixSize; j++) {
			printf("\t%-12f", MNAMatrix(i,j));
		}
		printf("\t%-12f", RHS(i));
	}
	printf("\n");
}


