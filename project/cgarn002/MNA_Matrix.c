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
#include "Eigen/Sparse"

#define WARNING_DIV_BY_ZERO \
		printf("\nWarning: divide by zero.");

int MatrixSize = 0;	// size of the MNA matrix (i.e., the max dimension)
Eigen::MatrixXd MNAMatrix;
Eigen::VectorXd RHS;
Eigen::VectorXd InitialRHS;
int eqnIndex = 0; //index of extra equations
double timeStep = 1; //in ns
double endTime = 10; //in ns
bool toggleGroundRef = false;

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
	InitialRHS.resize(MatrixSize+1);

	for(i = 0; i<MatrixSize +1; i++){
		for(j = 0; j<MatrixSize+1; j++){
			MNAMatrix(i,j) = 0.0;
		}
		RHS(i) = 0.0;
		InitialRHS(i) = 0.0;
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
	//Fill MNA at time 0 assuming 1ns timestep
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
				MNAMatrix(eqnIndex,eqnIndex) += -val/timeStep;
				MNAMatrix(indxp,eqnIndex) += 1;
				MNAMatrix(indxm,eqnIndex) += -1;
				dev->index_i = eqnIndex;
				dev->needs_updating = true;
				UpdateDeviceListSize++;
				++eqnIndex;				
				break;
			case DEV_CAPACITOR:
				MNAMatrix(indxp,indxp) += val/timeStep;
				MNAMatrix(indxp,indxm) += -val/timeStep;
				MNAMatrix(indxm,indxp) += -val/timeStep;
				MNAMatrix(indxm,indxm) += val/timeStep;
				dev->index_i = indxp;
				dev->index_j = indxm;
				dev->needs_updating = true;
				UpdateDeviceListSize++;
				break;
			case DEV_VS:
				if(dev->pwl){
					dev->index_i = eqnIndex;
					dev->needs_updating = true;
					UpdateDeviceListSize++;
					if(dev->pwl[0][0] == 0){
						val = dev->pwl[1][0];
						dev->pwlIndex++;
					}

					if(dev->pwl[0][dev->pwlSize - 1] > endTime)
						endTime = dev->pwl[0][dev->pwlSize - 1];
				}
				else{
					InitialRHS(eqnIndex) += val;
				}
				MNAMatrix(eqnIndex,indxp) += 1;
				MNAMatrix(eqnIndex,indxm) += -1;
				MNAMatrix(indxp,eqnIndex) += 1;
				MNAMatrix(indxm,eqnIndex) += -1;
				RHS(eqnIndex) += val;
				++eqnIndex;				
				break;
			case DEV_CS:
				if(dev->pwl){
					dev->index_i = indxp;
					dev->index_j = indxm;
					dev->needs_updating = true;
					UpdateDeviceListSize++;
					if(dev->pwl[0][0] == 0){
						val = dev->pwl[1][0];
						dev->pwlIndex++;
					}
					if(dev->pwl[0][dev->pwlSize - 1] > endTime)
						endTime = dev->pwl[0][dev->pwlSize - 1];
				}
				else{
					InitialRHS(indxp)+= -val;
					InitialRHS(indxm)+= val;
				}
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

	UpdateDeviceList = (Device_Entry**)malloc(sizeof(Device_Entry*)*UpdateDeviceListSize);
	//add all devices that need to be updated in list
	int i;
	for(dev = DeviceTableHead, i = 0; dev && i<UpdateDeviceListSize; dev = dev->next, i++){
		if(dev->needs_updating)
			UpdateDeviceList[i] = dev;
	}
}

void Solve_MNA(){
	int i, j;
	printf("\n\n%f", endTime);
	printf("\n\nSolution with ground calculated:\n");
	printf("Time\\Nodes");
	for (i = 0; i <= MatrixSize; i++) {
		printf("\t%-12d", i);
	}

	Eigen::SparseMatrix<double, Eigen::ColMajor> sparse = MNAMatrix.sparseView();
	sparse.makeCompressed();
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<double> > solver;
	solver.compute(sparse);

	for(double i = 0; i <= endTime; i+=timeStep){
		//solve for voltages/currents
		Eigen::VectorXd ans;
		if(MatrixSize <= 50) ans = MNAMatrix.colPivHouseholderQr().solve(RHS);
		else ans = solver.solve(RHS);
		if(i-(int)i == 0.0)printf("\n[%4dns]",(int)i);
		else printf("\n[%4fns]",i);
		for(int j = 0; j <= MatrixSize; j++){
			if(toggleGroundRef)
				printf("\t%-12e", ans(j) - ans(0));
			else
				printf("\t%-12e", ans(j));
		}

		// no need to update again
		if(i+timeStep>endTime)
			return;

		RecalculateRHS(i, ans);

	}

	printf("\n\n");
}

void RecalculateRHS(double time, Eigen::VectorXd &ans)
{
	RHS = InitialRHS;
	//update voltages/currents
	for(int j = 0; j < UpdateDeviceListSize; j++){
		Device_Entry* dev = UpdateDeviceList[j];
		if(dev==NULL) return; //wtf???
		switch(dev->type){
			case DEV_VS:
				if(dev->pwlIndex+1 < dev->pwlSize 
					&& dev->pwl[0][dev->pwlIndex+1] >= time){
					dev->pwlIndex++;
				}
				RHS(dev->index_i) += dev->pwl[1][dev->pwlIndex];
				break;

			case DEV_CS:
				if(dev->pwlIndex+1 < dev->pwlSize 
					&& dev->pwl[0][dev->pwlIndex+1] >= time){
					dev->pwlIndex++;
				}
				RHS(dev->index_i) += -dev->pwl[1][dev->pwlIndex];
				RHS(dev->index_j) += dev->pwl[1][dev->pwlIndex];
				break;

			case DEV_INDUCTOR:
				RHS(dev->index_i) += -ans(dev->index_i)*dev->value/timeStep;
				break;

			case DEV_CAPACITOR:
				RHS(dev->index_i) += ans(dev->index_i)*dev->value/timeStep;
				RHS(dev->index_j) += -ans(dev->index_j)*dev->value/timeStep;
				break;
			default:
				//do nothing
				break;
		}
	}
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
			printf("\t%-12e", MNAMatrix(i,j));
		}
		printf("\t%-12f", RHS(i));
	}
	printf("\n");
}


