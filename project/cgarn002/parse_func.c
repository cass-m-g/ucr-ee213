/************************************************************
  	parse_func.c
  	containing parsing functions for SPICE netlist parser example.

Author(s):
   	Shi, Guoyong  2015
 
	Course material for "Introduction to EDA", 2015
   	Dept of Micro/Nano-electronics
   	Shanghai Jiao Tong University (SJTU)
************************************************************/

#ifdef MATLAB
#include "mex.h"
#endif

#include "parse_func.h"
#include "Symbol_Table.h"
#include <iostream>

// Global variables defined for the parser utility functions
int nRes;
int nCap;
int nInd;
int nVsrc;
int nIsrc;
int nVCCS;


void Init_parse_util()
{
	nRes = 0;
	nCap = 0;
	nInd = 0;
	nVsrc = 0;
	nIsrc = 0;
	nVCCS = 0;
}

void ParseRes(char *name, char *node1, char *node2, double value)
{
	int numnodes;
	Node_Entry **nodelist;
	
	printf("[Resistor parsed ...]\n");
	printf("   name=%s, node+=%s, node-=%s, R=%e\n", name, node1, node2, value);
	nRes++;

	// Save the device, nodes, value info to the symbol tables.
	numnodes = 2;
	nodelist = (Node_Entry**)malloc(sizeof(Node_Entry*)*numnodes);
	nodelist[0] = Insert_Node_Entry(node1);
	nodelist[1] = Insert_Node_Entry(node2);
	Insert_Device_Entry(name, numnodes, nodelist, value, DEV_RESISTOR);
}

void ParseCap(char *name, char *node1, char *node2, double value)
{
	int numnodes;
	Node_Entry **nodelist;
	
	printf("[Capacitor parsed ...]\n");
	printf("   name=%s, node+=%s, node-=%s, C=%e\n", name, node1, node2, value);
	nCap++;

	// Save the device, nodes, value info to the symbol tables.
	numnodes = 2;
	nodelist = (Node_Entry**)malloc(sizeof(Node_Entry*)*numnodes);
	nodelist[0] = Insert_Node_Entry(node1);
	nodelist[1] = Insert_Node_Entry(node2);
	Insert_Device_Entry(name, numnodes, nodelist, value, DEV_CAPACITOR);
}

void ParseCapIC(char *name, char *node1, char *node2, double value, double init)
{
	printf("[Capacitor parsed ...]\n");
	printf("   name=%s, node+=%s, node-=%s, C=%e, init=%e\n", name, node1, node2, value, init);
	nCap++;

	// Save the device, nodes, value info to the symbol tables.
//****************************FIGURE OUT HOW TO ADD INIT**************************
	int numnodes;
	Node_Entry **nodelist;
	numnodes = 2;
	nodelist = (Node_Entry**)malloc(sizeof(Node_Entry*)*numnodes);
	nodelist[0] = Insert_Node_Entry(node1);
	nodelist[1] = Insert_Node_Entry(node2);
	Insert_Device_Entry(name, numnodes, nodelist, value, DEV_CAPACITOR);
}

void ParseInd(char *name, char *node1, char *node2, double value)
{
	int numnodes;
	Node_Entry **nodelist;
	
	printf("[Inductor parsed ...]\n");
	printf("   name=%s, node+=%s, node-=%s, L=%e\n", name, node1, node2, value);
	nInd++;

	// Save the device, nodes, value info to the symbol tables.
	numnodes = 2;
	nodelist = (Node_Entry**)malloc(sizeof(Node_Entry*)*numnodes);
	nodelist[0] = Insert_Node_Entry(node1);
	nodelist[1] = Insert_Node_Entry(node2);
	Insert_Device_Entry(name, numnodes, nodelist, value, DEV_INDUCTOR);

}

void ParseIndIC(char *name, char *node1, char *node2, double value, double init)
{
	printf("[Inductor parsed ...]\n");
	printf("   name=%s, node+=%s, node-=%s, L=%e, init=%e\n", name, node1, node2, value, init);
	nInd++;

	// Save the device, nodes, value info to the symbol tables.
//****************************FIGURE OUT HOW TO ADD INIT**************************
	int numnodes = 2;
	Node_Entry **nodelist = (Node_Entry**)malloc(sizeof(Node_Entry*)*numnodes);
	nodelist[0] = Insert_Node_Entry(node1);
	nodelist[1] = Insert_Node_Entry(node2);
	Insert_Device_Entry(name, numnodes, nodelist, value, DEV_INDUCTOR);
}

void ParseVsrc(char *name, char *node1, char *node2, double value)
{
	int numnodes;
	Node_Entry **nodelist;
	
	printf("[Voltage Source parsed ...]\n");
	printf("   name=%s, node+=%s, node-=%s, V=%e\n", name, node1, node2, value);
	nVsrc++;

	// Save the device, nodes, value info to the symbol tables.
	numnodes = 2;
	nodelist = (Node_Entry**)malloc(sizeof(Node_Entry*)*numnodes);
	nodelist[0] = Insert_Node_Entry(node1);
	nodelist[1] = Insert_Node_Entry(node2);
	Insert_Device_Entry(name, numnodes, nodelist, value, DEV_VS);
}

void ParseVsrc(char *name, char *node1, char *node2, char *pwl)
{
	int numnodes;
	Node_Entry **nodelist;
	
	printf("[Voltage Source parsed ...]\n");
	printf("   name=%s, node+=%s, node-=%s, V=%s\n", name, node1, node2, pwl);
	nVsrc++;

	// Save the device, nodes, value info to the symbol tables.
	/*numnodes = 2;
	nodelist = (Node_Entry**)malloc(sizeof(Node_Entry*)*numnodes);
	nodelist[0] = Insert_Node_Entry(node1);
	nodelist[1] = Insert_Node_Entry(node2);
	Insert_Device_Entry(name, numnodes, nodelist, value, DEV_VS);*/
}

void ParseIsrc(char *name, char *node1, char *node2, double value)
{
	int numnodes;
	Node_Entry **nodelist;
	
	printf("[Current source parsed ...]\n");
	printf("   name=%s, node+=%s, node-=%s, I=%e\n", name, node1, node2, value);
	nIsrc++;

	// Save the device, nodes, value info to the symbol tables.
	numnodes = 2;
	nodelist = (Node_Entry**)malloc(sizeof(Node_Entry*)*numnodes);
	nodelist[0] = Insert_Node_Entry(node1);
	nodelist[1] = Insert_Node_Entry(node2);
	Insert_Device_Entry(name, numnodes, nodelist, value, DEV_CS);
}

void ParseIsrc(char *name, char *node1, char *node2, char *pwl)
{
	int numnodes;
	Node_Entry **nodelist;
	
	printf("[Current source parsed ...]\n");
	//printf("   name=%s, node+=%s, node-=%s, I=%s\n", name, node1, node2, pwl);
	nIsrc++;

	// Save the device, nodes, value info to the symbol tables.
	numnodes = 2;
	nodelist = (Node_Entry**)malloc(sizeof(Node_Entry*)*numnodes);
	nodelist[0] = Insert_Node_Entry(node1);
	nodelist[1] = Insert_Node_Entry(node2);
	Device_Entry* dev = Insert_Device_Entry(name, numnodes, nodelist, DEV_CS);
	ParsePWL(pwl, dev->pwl);
}

void ParseVCCS(char *name, char *node1, char *node2, char *node3, char *node4, double value)
{
	int numnodes;
	Node_Entry **nodelist;
	
	printf("[VCCS parsed ...]\n");
	printf("   name=%s, N+=%s, Ne-=%s,  Nc+=%s, Nc-=%s, G=%e\n", 
			name, node1, node2, node3, node4, value);
	nVCCS++;

	// Save the device, nodes, value info to the symbol tables.
	numnodes = 4;
	nodelist = (Node_Entry**)malloc(sizeof(Node_Entry*)*numnodes);
	nodelist[0] = Insert_Node_Entry(node1);
	nodelist[1] = Insert_Node_Entry(node2);
	nodelist[2] = Insert_Node_Entry(node3);
	nodelist[3] = Insert_Node_Entry(node4);
	Insert_Device_Entry(name, numnodes, nodelist, value, DEV_VCCS);
}

void ParsePWL(char *pwl, std::vector<std::pair<double, double> > &ret)
{
	std::vector<double> tmp;
	pwl[strlen(pwl)-2] = '\0';
	std::string num = "";
	for(int i = 0; pwl[i] && i < strlen(pwl) - 1; i++){
		if(isdigit(pwl[i]) || pwl[i] == '.' || (num != ""&& (pwl[i] == 'e' || pwl[i] == '-' || pwl[i] == '+')))
			num += pwl[i];
		else if(num != ""){
			double n = atof(num.c_str());
			tmp.push_back(n);
			num = "";
		}
	}
	for(int i = 0; i < tmp.size() -1; i+=2){
		ret.push_back(std::make_pair(tmp.at(i), tmp.at(i+1)));
	}
	for(int i = 0; i < ret.size(); i++){
		std::cout << ret.at(i).first << ' ' << ret.at(i).second << std::endl;
	}
}


void Summarize()
{
	printf("[Finished parsing netlist!]\n");
	printf("   #res=%d, #cap=%d, #ind=%d, #vccs=%d, #vsrc=%d, #isrc=%d\n", \
		nRes, nCap, nInd, nVCCS, nVsrc, nIsrc);
}
