#ifndef EDGE_H
#define EDGE_H

#include <limits>

class Edge
{
public:
	Edge(int id, int from, int to, double cost = std::numeric_limits<double>::infinity()) :
		id(id), from(from), to(to), cost(cost) {}

	int id;
	int from;
	int to;

	double cost;
};

#endif
