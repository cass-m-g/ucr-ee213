/**
	Symbol_Table.h
	For the undergraduate EDA course offered at 
	Dept of Micro/Nano-electronics

	(c) Guoyong Shi, Shanghai Jiao Tong University
	All rights reserved, 2006-2015

	If you modify the code, please keep the above copyright info.
*/

#ifndef SYMBLE_TABLE_H
#define SYMBLE_TABLE_H

#include <utility>
#include <vector>

/**
	A node struct constituting a node table.
	To be used for parsing netlist.
*/
struct node_s 
{
	const char *name;	// node name to be allocated
	int index;	// node index for accessing matrix row/column
	void *node; 	// points to a Node instance in the future
	struct node_s *prev;
	struct node_s *next;
};
typedef struct node_s Node_Entry;

typedef enum{
	DEV_RESISTOR,
	DEV_INDUCTOR,
	DEV_CAPACITOR,
	DEV_VS,
	DEV_CS,
	DEV_VCCS,
	DEV_NO_TYPE
	}device_type;

struct device_s {
	const char *name;
	int numnodes;	// length of the node list
	Node_Entry **nodelist;
	double value;
	device_type type;
	void *device;	// points to a device instance in the future
	struct device_s *prev;
	struct device_s *next;

	double **pwl; //index 0 is time
	int pwlSize;
	int pwlIndex;
	int index_i, index_j;
	bool needs_updating;
};
typedef struct device_s Device_Entry;

extern Node_Entry *NodeTableHead;
extern int NodeTableSize;

extern Device_Entry *DeviceTableHead;
extern int DeviceTableSize;

extern Device_Entry **UpdateDeviceList;
extern int UpdateDeviceListSize;


void Init_Symbol_Tables();
void Destroy_Symbol_Table();

Node_Entry*    	Lookup_Node_Entry(const char *name);
Node_Entry*    	Insert_Node_Entry(const char *name);

void Delete_Node_Entry(const char *name);
void Delete_Node_Table();
void Print_Node_Table();

Device_Entry*   	Lookup_Device_Entry(const char *name);
Device_Entry*   	Insert_Device_Entry(const char *name,  const int numnodes, 
				Node_Entry **nodelist, const double value, device_type type);
Device_Entry*   	Insert_Device_Entry(const char *name,  const int numnodes, 
				Node_Entry **nodelist,device_type type);

void Delete_Device_Entry(const char *name);
void Delete_Device_Table();
void Print_Device_Table();
const char* Print_Device_Type(device_type type);

int NameHash(const char *name, int tsize);

#endif  // SYMBLE_TABLE_H


