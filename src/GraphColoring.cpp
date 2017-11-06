#include "GraphColoring.h"


GraphColoring::GraphColoring(etype * row, vtype * col, vtype nov)
{
	this->row = row;
	this->col = col;
	this->nov = nov;
	// create the list of vertices, which contains constraint count and count vertices to be effected in case of coloring, for each vertex
	// constraintCnt = 0 (no constraint at the beginning) effectCnt = num of neighbours;
	std::pair<int, int> *toBeSorted = new std::pair<int, int>[nov];
	for (size_t i = 0; i < nov; i++)
	{
		toBeSorted[i].first = i;
		toBeSorted[i].second = row[i + 1] - row[i];
	}
	// sort descending order with respect to number of neighbours
	std::sort(toBeSorted, toBeSorted + nov, [](const std::pair<int, int> &lhs, const std::pair<int, int> &rhs) -> bool {
		return lhs.second > rhs.second;
	});

	this->head = toBeSorted[0].first;
	this->tail = toBeSorted[nov - 1].first;

	this->list = new Node[nov];
	for (size_t i = 0; i < nov; i++)
	{
		list[i].effectCnt = row[i + 1] - row[i];
		list[toBeSorted[i].first].next = i != nov - 1 ? toBeSorted[i + 1].first : -2;
		list[toBeSorted[i].first].prev = i != 0 ? toBeSorted[i - 1].first : -1;

		// TODO: initialize d2-colors
	}
	delete[] toBeSorted;
}

GraphColoring::~GraphColoring()
{
}

int GraphColoring::colorGraph(std::vector<int> &colors)
{
	//TODO: on each step; get first element from list, pick best color, update list.
	return 0;
}
