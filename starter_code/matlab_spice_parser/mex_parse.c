/*==============================================================
 * mex_parse.c 
 * Parsing a netlist in MATLAB
 * First written on 12-30-2014
 * Compile in MATLAB:  
 	>> mex -DMATLAB mex_parse.c parse_func.c Symbol_Table.c MNA_Matrix.c

    Before compiling in MATLAB, compile the flex-bison files in CYGWIN by invoking the Makefile
    $ make parse

    The following are user functions:
	"parse_func.c"		
	"Symbol_Table.c"
	"MNA_Matrix.c"
 *
 * Copyright 2014-2015 by Guoyong SHI (c)
 *=============================================================*/

#include "mex.h"
#include "parse.yy.c"		// generated by flex
#include "parse.tab.c"		// generated by bison

void Call_Parser(char *filename)
{
	FILE *inFile;

	mexPrintf("\nFile name: %s\n", filename);

	inFile = fopen(filename, "r");
	if (!inFile) {
		mexPrintf("Did not find the file \"%s\".\n", filename);
		return;
	}

	Init_Symbol_Tables();
	Init_parse_util();	// initialize vars for utility functions

	yyin = inFile;
	yyparse();			// run parser
	fclose(inFile);

	Summarize();
	
	Index_All_Nodes();
	Print_Node_Table();
	Print_Device_Table();

	Init_MNA_System();
	Create_MNA_Matrix();
	Print_MNA_System();
	
}

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
	char *netlistname;
	double *A, *b;			// pointers for A and b  (A*x = b)
	//size_t total_bytes;
	unsigned int M;			// matrix dimension
    
	/* Check for correct number of arguments */
	if (nrhs!=1) 
		mexErrMsgTxt("One input required.");
	else if (nlhs > 2) 
		mexErrMsgTxt("Too many output arguments.");

	/* Input must be a string */
	if (mxIsChar(prhs[0]) != 1)
		mexErrMsgTxt("Input must be a string.");

	/* Input must be a row vector */
	if (mxGetM(prhs[0]) != 1)
		mexErrMsgTxt("Please make sure input is a single string.");

	/* Copy the string data from prhs[0] to the netlistname string in C. */
	netlistname = mxArrayToString(prhs[0]);

	if (netlistname == NULL) 
		mexErrMsgTxt("No file name is given.");

	/* Call the C subroutine */
	Call_Parser(netlistname);

	/** Retrieve MNA matrix and RHS vector from the parser. 
	      Place the retrieved MNA system (A, b) into the LHS variables of the mex function.
	**/

	M = Get_Matrix_Size();
	plhs[0] = mxCreateDoubleMatrix(M+1, M+1, mxREAL);	// Matrix
	plhs[1] = mxCreateDoubleMatrix(M+1, 1, mxREAL);		// RHS

	// Fetch the pointers for the MNA system
	A = (double*) mxGetPr(plhs[0]);
	b = (double*) mxGetPr(plhs[1]);
	
	// TOTAL_ELEMENTS = 4 defined as macro
	//total_bytes = 4 * mxGetElementSize(plhs[0]);
	
	//memcpy(lhs_ptr, data, total_bytes);
	
	/* Passing out the matrix and rhs from the parser. */

	Get_MNA_System(&A, &b);

	mxFree(netlistname);
	return;
}
