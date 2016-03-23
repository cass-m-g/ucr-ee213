/**
	Symbol_Table.c
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

/* Electric type of a node variable */
#define SP_NOTYPE 	0
#define SP_VOLTAGE 	1
#define SP_CURRENT 	2

#define LINE_INFO   "\n[" << __FILE__ << " : line " << __LINE__ << " : " << __FUNCTION__ << "()] - "
#define PRINT_LINE_INFO 	printf("\n[%s : line %d : %s()]\n", __FILE__, __LINE__, __FUNCTION__);


Node_Entry **NodeTable;
int NodeTableSize;

Device_Entry **DeviceTable;
int DeviceTableSize;


void Init_Symbol_Tables(){
	/* Init Node Table */
	NodeTable=malloc(sizeof(Node_Entry*));
    NodeTableSize = 0;

	/* Init Device Table */
    DeviceTable=malloc(sizeof(Device_Entry*));
    DeviceTableSize = 0;
}


void Destroy_Symbol_Table(){
	Delete_Node_Table();
	Delete_Device_Table();
}


void Delete_Node_Table(){
	Node_Entry *nodePtr = *NodeTable;
	Node_Entry *kill;
	while(nodePtr != NULL){
		kill = nodePtr;
		nodePtr = nodePtr->next;
		free(kill);
	}
	free(NodeTable);
}

void Delete_Device_Table(){
	Device_Entry *devPtr = *DeviceTable;
	Device_Entry *kill;
	while(devPtr != NULL){
		kill = devPtr;
		devPtr = devPtr->next;
		free(kill);
	}
	free(DeviceTable);
}

void Delete_Node_Entry(const char *name){
	Node_Entry *nodePtr = *NodeTable;
	Node_Entry *kill;
	// If node to delete is head
	if(!strcmp(nodePtr->name,name)){
		*NodeTable = (*NodeTable)->next;
		free(nodePtr);
		--NodeTableSize;
		return;
	}
	// Else search the rest of the list
	while(nodePtr->next	!= NULL){
		if(!strcmp(nodePtr->next->name,name)){
			kill = nodePtr->next;
			nodePtr->next = nodePtr->next->next;
			free(kill);
			--NodeTableSize;
			return;
		}
		nodePtr = nodePtr->next;
	}
}

void Delete_Device_Entry(const char *name){
	Device_Entry *devPtr = *DeviceTable;
	Device_Entry *kill;
	// If dev to delete is head
	if(!strcmp(devPtr->name,name)){
		*DeviceTable = (*DeviceTable)->next;
		free(devPtr);
		--DeviceTableSize;
		return;
	}
	// Else search the rest of the list
	while(devPtr->next	!= NULL){
		if(!strcmp(devPtr->next->name,name)){
			kill = devPtr->next;
			devPtr->next = devPtr->next->next;
			free(kill);
			--DeviceTableSize;
			return;
		}
	}
	devPtr = devPtr->next;

}

Node_Entry* Lookup_Node_Entry(const char *name){
	Node_Entry *nodePtr = *NodeTable;
	while(nodePtr!=NULL){
		if(!strcmp(nodePtr->name,name)){
			return nodePtr;
		}
		nodePtr = nodePtr->next;
	}
	return  NULL;
}


Device_Entry* Lookup_Device_Entry(const char *name){
	Device_Entry *devPtr = *DeviceTable;
	while(devPtr!=NULL){
		if(!strcmp(devPtr->name,name)){
			return devPtr;
		}
		devPtr = devPtr->next;
	}
	return  NULL;
}


Node_Entry* Insert_Node_Entry(const char *name){
	/* Create and initialize new node */
	Node_Entry* newNode = malloc(sizeof(Node_Entry));
	newNode->name = name;
	newNode->node = NULL;
	newNode->index = 0;

	/* Insert new node (at front) */
	newNode->next = *NodeTable;
	*NodeTable = newNode;
	++NodeTableSize;

	return newNode;
}


Device_Entry* Insert_Device_Entry(const char *name,  const int numnodes, 
				Node_Entry **nodelist, const double value)
{
	/* Create and initialize new device  */
	Device_Entry* newDev= malloc(sizeof(Device_Entry));
	newDev->name = name;
	newDev->value = value;
	newDev->numnodes = numnodes;
	newDev->nodelist = nodelist;
	newDev->device= NULL;

	/* Insert new device (at front) */
	newDev->next = *DeviceTable;
	*DeviceTable = newDev;
	++DeviceTableSize;
	return newDev;
}


void Print_Node_Table(){
	Node_Entry *nodePtr = *NodeTable;
	for(nodePtr; nodePtr!=NULL; nodePtr = nodePtr->next){
		printf("Node name: %s\nNode index: %i\n\n",nodePtr->name,nodePtr->index);
	}
}


void Print_Device_Table(){
	Device_Entry *devPtr = *DeviceTable;
	for(devPtr; devPtr!=NULL; devPtr = devPtr->next){
		printf("Device name: %s\nDevice val: %f\nDevice nodes: %i\n",
			devPtr->name,devPtr->value,devPtr->numnodes);
		int i=0;
		for(i;i<devPtr->numnodes;++i){
			printf("\tNode name: %s\n",devPtr->nodelist[i]->name);
		}
	}
}

int NameHash(const char *name,  int tsize){
	const char *s; 
	register int i = 0; 

	/* In case no name */
	if (!name)	return (0);
	
	for (s = name; *s; s++) {
		i += (unsigned) *s; 
	}
	
	return (i % tsize);
}


