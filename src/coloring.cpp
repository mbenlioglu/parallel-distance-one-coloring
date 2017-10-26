/*
*
*
*/

#include <iostream>
#include <omp.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graphio.h"
#include "graph.h"
#ifdef __cplusplus
}
#endif /*__cplusplus*/

#define DEBUG

void print_usage() {
	std::cout << "./coloring <filename>\n";
}

int main(int argc, char *argv[])
{
	etype *row_ptr;
	vtype *col_ind;
	ewtype *ewghts;
	vwtype *vwghts;
	vtype nov, source;

	if (argc != 1)
	{
		print_usage();
		return 1;
	}




	return 0;
}