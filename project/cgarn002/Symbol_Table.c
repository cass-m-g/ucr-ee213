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

// Electric type of a node variable
#define SP_NOTYPE 	0
#define SP_VOLTAGE 	1
#define SP_CURRENT 	2

#define LINE_INFO   "\n[" << __FILE__ << " : line " << __LINE__ << " : " << __FUNCTION__ << "()] - "
#define PRINT_LINE_INFO 	printf("\n[%s : line %d : %s()]\n", __FILE__, __LINE__, __FUNCTION__);


Node_Entry *NodeTableHead;
int NodeTableSize;

Device_Entry *DeviceTableHead;
int DeviceTableSize;


void Init_Symbol_Tables()
{
	NodeTableSize = 0;
	DeviceTableSize = 0;
}


void Destroy_Symbol_Table()
{
	Delete_Node_Table();
	Delete_Device_Table();
}


void Delete_Node_Table()
{
	Node_Entry *next = NodeTableHead, *to_delete;
	while(next){
		to_delete = next;
		next = to_delete->next;
		free(to_delete);		
	}
}

void Delete_Device_Table()
{
	Device_Entry *next = DeviceTableHead, *to_delete;
	while(next){
		to_delete = next;
		next = to_delete->next;
		free(to_delete);		
	}
}

void Delete_Node_Entry(const char *name)
{
	Node_Entry *to_delete = Lookup_Node_Entry(name);
	if(to_delete){
		if(NodeTableHead == to_delete){
			NodeTableHead = to_delete->next;
			NodeTableHead->prev = NULL;			
		}
		if(to_delete->prev){
			to_delete->prev->next = to_delete->next;
			to_delete->next->prev = to_delete->prev;
		}	
		free(to_delete);
	}
}

void Delete_Device_Entry(const char *name)
{
	Device_Entry *to_delete = Lookup_Device_Entry(name);
	if(to_delete){
		if(DeviceTableHead == to_delete){
			DeviceTableHead = to_delete->next;
			DeviceTableHead->prev = NULL;
		}
		if(to_delete->prev){
			to_delete->prev->next = to_delete->next;
			to_delete->next->prev = to_delete->prev;
		}	
		free(to_delete);
	}
}

//returns null if node not found
Node_Entry* Lookup_Node_Entry(const char *name)
{
	Node_Entry *n;
	for(n = NodeTableHead; n; n = n->next){
		if(strcmp(n->name, name) == 0){
			return n;
		}
	}
	return  NULL;
}


Device_Entry* Lookup_Device_Entry(const char *name)
{
	Device_Entry *n;
	for(n = DeviceTableHead; n; n = n->next){
		if(strcmp(n->name, name) == 0){
			return n;
		}
	}
	return NULL;
}


Node_Entry* Insert_Node_Entry(const char *name)
{
	Node_Entry *ret;
	if(ret = Lookup_Node_Entry(name)){
		return ret;
	}

	ret = malloc(sizeof(Node_Entry));
	ret->name = name;
	ret->next = NodeTableHead;
	NodeTableHead->prev = ret;
	NodeTableHead = ret;
	++NodeTableSize;
	return ret;
}


Device_Entry* Insert_Device_Entry(const char *name,  const int numnodes, 
				Node_Entry **nodelist, const double value)
{
	Device_Entry *ret;
	if(ret = Lookup_Device_Entry(name)){
		return ret;
	}

	ret = malloc(sizeof(Device_Entry));
	ret->name = name;
	ret->next = DeviceTableHead;
	DeviceTableHead->prev = ret;
	DeviceTableHead = ret;
	++DeviceTableSize;
	return ret;
}


void Print_Node_Table()
{
	printf("[NodeTable ...]\n");
	Node_Entry *n;
	for(n = NodeTableHead; n; n = n->next){
		printf("name=%s, index=%d\n", n->name, n->index);
	}
	printf("\n");
}


void Print_Device_Table()
{
	printf("[DeviceTable ...]\n");
	Device_Entry *n;
	int i;
	for(n = DeviceTableHead; n; n = n->next){
		printf("name=%s, ", n->name);
		int num = n->numnodes;
		for(i = 0; i < num; i++){
			printf("node%d=%s, ", i, n->nodelist[i]->name);
		}
		printf("\n");
	}
	printf("\n");
}


int NameHash(const char *name,  int tsize)
{
	const char *s; 
	register int i = 0; 

	// In case no name
	if (!name)	return (0);
	
	for (s = name; *s; s++) {
		i += (unsigned) *s; 
	}
	
	return (i % tsize);
}


