/*
* Created by: mbenlioglu, October 26, 2017
*
* This program executes sequential and parallel versions of distance-1 graph coloring algorithm, outputs the execution
* times of sequential and parallel versions(with 1, 2, 4, 8, 16 threads) as well as the total number of color used in
* each execution and runs a correctness check to confirm no adjacent vertices has the same color.
*
*/

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

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <omp.h>
#include <vector>
#include "GraphColoring.h"


#define DEBUG

typedef	struct perfData
{
	double prepTime, execTime;
	int colorCnt, mergeConflictCnt;
}perfData;

/*
	Traverses entire graph to find conflicts (i.e. adjecent vertices with same color), returns the number of such
	conflicts as return value, indices of these vertices are written into out vector.
*/
int detect_conflicts(etype *row, vtype *col, vtype nov, int colors[], std::vector<int> &out)
{
#pragma omp parallel for
	for (size_t i = 0; i < nov; i++)
	{
		int c = colors[i];
		int numAdj = row[i + 1] - row[i];
		for (size_t j = 0; j < numAdj; j++)
		{
			if (colors[col[j]] == c)
			{
#pragma omp critical
				out.push_back(i < col[j] ? i : col[j]);
			}
		}
	}
	return out.size() / 2;
}

inline int num_of_colors(int colors[], int size) // TODO: might not be needed, keep just in case
{
	std::sort(colors, colors + size);

	int largest, num;
	largest = -1;

	for (size_t i = 0; i < size; i++)
		if (colors[i] > largest)
			largest = colors[i];

	return largest;
}

//===========================================================================================================================
// Namespaces for Direct/Heuristic coloring functions
//===========================================================================================================================
namespace Direct
{
	/*
		Traverses the neighbours of the given vertex and returns the smallest available color for the vertex
	*/
	int getSmallestAvailableColor(etype *row, vtype *col, int vertex, int colors[])
	{
		int color, colStart, colEnd;
		bool *isColorUsed;
		colStart = row[vertex];
		colEnd = row[vertex + 1];

		// get max color in adjacents
		int max = -1;
		for (size_t i = colStart; i < colEnd; i++)
			if (colors[col[i]] > max)
				max = colors[col[i]];

		if (max < 1) return 0;

		// track whether a color is used it not
		isColorUsed = new bool[max + 1]();
		for (size_t i = colStart; i < colEnd; i++)
			isColorUsed[colors[col[i]]] = true;

		//	return the smallest unused color
		for (size_t i = 0; i < max + 1; i++)
			if (!isColorUsed[i])
				return i;
		
		// every color is used return a new color
		return max + 1;
	}

	perfData color_graph_seq(etype *row, vtype *col, vtype nov, int colors[])
	{
		perfData result;
		double startTime, endTime;
		int maxColor = 0;

		startTime = omp_get_wtime();
		for (size_t i = 0; i < nov; i++)
		{
			int c = getSmallestAvailableColor(row, col, i, colors);
			colors[i] = c;
			if (c > maxColor) maxColor = c;
		}
		endTime = omp_get_wtime();

		result.colorCnt = maxColor + 1;
		result.execTime = endTime - startTime;
		result.prepTime = result.mergeConflictCnt = 0;
		return result;
	}

	perfData color_graph_par(etype *row, vtype *col, vtype nov, int colors[])
	{
		perfData result;
		double startTime, endTime;
		
		int mergeConflictCnt = 0;
		std::vector<int> conflictedVertices;

		// first stage coloring
		startTime = omp_get_wtime();
#pragma omp for
		for (size_t i = 0; i < nov; i++)
		{
			int c = getSmallestAvailableColor(row, col, i, colors);
			colors[i] = c;
		}

		// fix conflict
		do
		{
			detect_conflicts(row, col, nov, colors, conflictedVertices);
#pragma omp for
			for (size_t i = 0; i < conflictedVertices.size(); i++)
			{
				int c = getSmallestAvailableColor(row, col, i, colors);
				colors[i] = c;
			}
			conflictedVertices.clear();
			++mergeConflictCnt;
		} while (conflictedVertices.size());
		endTime = omp_get_wtime();

		result.colorCnt = num_of_colors(colors, nov);
		result.execTime = endTime - startTime;
		result.mergeConflictCnt = mergeConflictCnt;
		result.prepTime = 0;
		return result;
	}
}

namespace Heuristic
{
	perfData color_graph_seq(etype *row, vtype *col, vtype nov, int colors[])
	{
		perfData result;
		double startTime, endTime;
		int colorCount;

		// Preprocessing
		startTime = omp_get_wtime();
		// initialize graphColoring object with the graph.
		GraphColoring gc(row, col, nov);
		endTime = omp_get_wtime();
		result.prepTime = endTime - startTime;

		// Coloring
		startTime = omp_get_wtime();
		result.colorCnt = gc.colorGraph(colors);
		endTime = omp_get_wtime();
		result.execTime = endTime - startTime;
		
		result.mergeConflictCnt = 0;
		return result;
	}

	perfData color_graph_par(etype *row, vtype *col, vtype nov, int colors[])
	{

	}
}
//===========================================================================================================================

void print_usage()
{
	std::cout << "./coloring <filename>\n";
	std::cout << "Press Enter to continue...";
	std::cin.get();
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

	// Performance analysis
	perfData perfSeq, perfPar[5];
	double startTime, endTime;

	int *colors = new int[nov];
	std::fill_n(colors, nov, -1);

	//===========================================================================================================================
	// Direct Approach
	//===========================================================================================================================
	std::cout << std::setfill('*') << std::setw(65) << "-\n";
	std::cout << "Starting performance analysis for direct approch\n\n";
	std::cout << std::setfill('*') << std::setw(65) << "-\n";
	
	// Sequential
	std::cout << "Starting sequential algorithm...";
	perfSeq = Direct::color_graph_seq(row_ptr, col_ind, nov, colors);
	std::cout << "ended\n";
#ifdef DEBUG
	std::vector<int> out;
	std::cout << "Running correctness check..." << !detect_conflicts(row_ptr, col_ind, nov, colors, out) ? "correct\n" : "wrong!\n";
	out.clear();
#endif // DEBUG
	std::fill_n(colors, nov, -1); // reinitialize
	
	// Parallel
	for (size_t i = 0; i < 5; i++)
	{
		omp_set_num_threads(i);
		std::cout << "Starting parallel algorithm with " << i << " threads...";
		perfPar[i] = Direct::color_graph_par(row_ptr, col_ind, nov, colors);
		std::cout << "ended\n";
#ifdef DEBUG
		std::cout << "Running correctness check..." << !detect_conflicts(row_ptr, col_ind, nov, colors, out) ? "correct\n" : "wrong!\n";
		out.clear();
#endif // DEBUG
		std::fill_n(colors, nov, -1); // reinitialize
	}

	// Print results
	printf("| %-15s | %-12s | %-12s | %-15s | %-15s |\n", "Algorithm", "Thread Count", "# of Conf.Fixes", "# of Colors", "Exec. Time");
	std::cout << std::setfill('-') << std::setw(65) << "-\n\n";
	printf("| %-15s | %-12d | %-12d | %-15d | %-12.8f s |\n",
		"Sequential", 1, perfSeq.mergeConflictCnt, perfSeq.colorCnt, perfSeq.execTime);
	for (size_t i = 0; i < 5; i++)
		printf("| %-15s | %-12d | %-12d | %-15d | %-12.8f s |\n",
			"Parallel", (2 << i) / 2, perfPar[i].mergeConflictCnt, perfPar[i].colorCnt, perfPar[i].execTime);
	std::cout << "\n";

	//===========================================================================================================================
	// Heuristic Approach
	//===========================================================================================================================
	std::cout << std::setfill('*') << std::setw(65) << "-\n";
	std::cout << "Starting performance analysis for heuristic approch\n\n";
	std::cout << std::setfill('*') << std::setw(65) << "-\n";

	double prepTimeSeq, prepTimePar[5];

	// Sequential
	std::cout << "Starting sequential algorithm...";
	perfSeq = Heuristic::color_graph_seq(row_ptr, col_ind, nov, colors);
	std::cout << "ended\n";
#ifdef DEBUG
	std::cout << "Running correctness check..." << !detect_conflicts(row_ptr, col_ind, nov, colors, out) ? "correct\n" : "wrong!\n";
	out.clear();
#endif // DEBUG
	std::fill_n(colors, nov, -1); // reinitialize

	// Parallel
	for (size_t i = 0; i < 5; i++)
	{
		omp_set_num_threads(i);
		std::cout << "Starting parallel algorithm with " << i << " threads...";
		perfPar[i] = Heuristic::color_graph_par(row_ptr, col_ind, nov, colors);
		std::cout << "ended\n";
#ifdef DEBUG
		std::cout << "Running correctness check..." << !detect_conflicts(row_ptr, col_ind, nov, colors, out) ? "correct\n" : "wrong!\n";
		out.clear();
#endif // DEBUG
		std::fill_n(colors, nov, -1); // reinitialize
	}

	// Print results
	printf("| %-15s | %-12s | %-12s | %-15s | %-15s | %-15s |\n", "Algorithm", "Thread Count", "# of Conf.Fixes", "# of Colors", "Prep. Time", "Exec. Time");
	std::cout << std::setfill('-') << std::setw(65) << "-\n\n";
	printf("| %-15s | %-12d | %-12d | %-15d | %-12.8f s | %-12.8f s |\n",
		"Sequential", 1, perfSeq.mergeConflictCnt, perfSeq.colorCnt, perfSeq.prepTime, perfSeq.execTime);
	for (size_t i = 0; i < 5; i++)
		printf("| %-15s | %-12d | %-12d | %-15d | %-12.8f s | %-12.8f s |\n",
			"Parallel", (2 << i) / 2, perfPar[i].mergeConflictCnt, perfPar[i].colorCnt, perfPar[i].prepTime,perfPar[i].execTime);
	std::cout << "\n";

	std::cout << "Press Enter to continue...";
	std::cin.get();

	return 0;
}