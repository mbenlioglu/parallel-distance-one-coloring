/*
* Created by mbenlioglu, October 30, 2017
*
* Class to hold coloring order of vertices in a linkedlist form represented as array. Creation includes a sort step, 
* then the list is always kept sorted. Since the graph
*/

extern "C" {
#include "graph.h"
}
#include <utility>
#include <algorithm>
#include <vector>

#pragma once
class GraphColoring
{
public:
	GraphColoring(etype* row, vtype* col, vtype nov);
	~GraphColoring();
	int colorGraph(int colors[]);

private:
	typedef struct Node
	{
		std::vector<int> constraints; // constraints of the current node
		std::vector<std::pair<int, int> > d2Colors; // colors of distance-2 neighbours pairs of (id, color)
		int effectCnt; // how many vertices will it affect if colored
		int prev, next;
	}Node;

	// real graph
	vtype nov;
	etype *row;
	vtype *col;

	int head; // points to the start of the list (represented as -1 in the list)
	int tail; // points to the end of the list (represented as -2 in the list)
	Node *list; // sorted doubly linked list of vertices according to heuristic values
};

