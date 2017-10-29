/*
* Created by: mbenlioglu, October 26, 2017
*
* This program executes sequential and parallel versions of distance-1 graph coloring algorithm, outputs the execution
* times of sequential and parallel versions(with 1, 2, 4, 8, 16 threads) as well as the total number of color used in
* each execution and runs a correctness check to confirm no adjacent vertices has the same color.
*
*/

#include <iostream>
#include <iomanip>
#include <algorithm>
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

inline bool is_correct(etype *row, vtype *col, vtype nov, int colors[])
{
	for (size_t i = 0; i < nov; i++)
	{
		int c = colors[i];
		int numAdj = row[i + 1] - row[i];
		for (size_t j = 0; j < numAdj; j++)
		{
			if (colors[col[j]] == c)
			{
#ifdef DEBUG
				std::cout << "Same color detected vertices " << i << " and " << col[j] << "\n";
#endif // DEBUG
				return false;
			}
		}
	}
	return true;
}

inline int num_of_colors(int colors[], int size)
{
	std::sort(colors, colors + size);

	int last, num;
	last = -1;
	num = 0;

	for (size_t i = 0; i < size; i++)
	{
		if (colors[i] != last)
		{
			last = colors[i];
			num++;
		}
	}

	return num;
}

void color_graph_seq(etype *row, vtype *col, vtype nov, int colors[])
{
	// create MRV array (0 for all vertices at the beginning)

	// create most constraining vertex array (initially # of neighbours)

	// least constraining value (i.e. most common d-2 color)
}

void color_graph_par(etype *row, vtype *col, vtype nov, int colors[])
{
	// TODO: implement this
}

void print_usage()
{
	std::cout << "./coloring <filename>\n";
}

int main(int argc, char *argv[])
{
	etype *row_ptr;
	vtype *col_ind;
	ewtype *ewghts;
	vwtype *vwghts;
	vtype nov, source;

	// Graph reading
	if (argc != 2)
	{
		print_usage();
		return 1;
	}

	if (read_graph(argv[1], &row_ptr, &col_ind, &ewghts, &vwghts, &nov, 0) == -1)
	{
		std::cout << "error in graph read\n";
		return 1;
	}

	// Process input
	double startTime, endTime, execTimeSeq;
	double execTimePar[5];

	int colorCntSeq;
	int colorCntPar[5];

	int *colors = new int[nov];
	std::fill_n(colors, nov, -1);

	// Sequential
	std::cout << "Starting sequential algorithm...";
	startTime = omp_get_wtime();
	color_graph_seq(row_ptr, col_ind, nov, colors);
	endTime = omp_get_wtime();
	std::cout << "ended\n";
	execTimeSeq = endTime - startTime;
	colorCntSeq = num_of_colors(colors, nov);
#ifdef DEBUG
	std::cout << "Running correctness check..." << is_correct(row_ptr, col_ind, nov, colors) ? "correct\n" : "wrong!\n";
#endif // DEBUG
	std::fill_n(colors, nov, -1); // reinitialize

	// Parallel
	for (size_t i = 0; i < 5; i++)
	{
		omp_set_num_threads(i);
		std::cout << "Starting parallel algorithm with " << i << " threads...";
		startTime = omp_get_wtime();
		color_graph_par(row_ptr, col_ind, nov, colors);
		endTime = omp_get_wtime();
		std::cout << "ended\n";
		execTimePar[i] = endTime - startTime;
		colorCntPar[i] = num_of_colors(colors, nov);
#ifdef DEBUG
		std::cout << "Running correctness check..." << is_correct(row_ptr, col_ind, nov, colors) ? "correct\n" : "wrong!\n";
#endif // DEBUG
		std::fill_n(colors, nov, -1); // reinitialize
	}


	// Print results
	printf("| %-15s | %-12s | %-12s | %-15s |\n", "Algorithm", "Thread Count", "# of Colors", "Time");
	std::cout << std::setfill('-') << std::setw(65) << "-\n\n";
	printf("| %-15s | %-12d | %-12d | %-12.8f s |\n", "Sequential", 1, colorCntSeq, execTimeSeq);
	printf("| %-15s | %-12d | %-12d | %-12.8f s |\n", "Parallel", 1, colorCntPar[0], execTimePar[0]);
	printf("| %-15s | %-12d | %-12d | %-12.8f s |\n", "Parallel", 2, colorCntPar[1], execTimePar[1]);
	printf("| %-15s | %-12d | %-12d | %-12.8f s |\n", "Parallel", 4, colorCntPar[2], execTimePar[2]);
	printf("| %-15s | %-12d | %-12d | %-12.8f s |\n", "Parallel", 8, colorCntPar[3], execTimePar[3]);
	printf("| %-15s | %-12d | %-12d | %-12.8f s |\n", "Parallel", 16, colorCntPar[4], execTimePar[4]);


	std::cout << "Press any key to continue...";
	std::cin.get();
	std::cin.ignore();

	return 0;
}